#include "kxf-pch.h"
#include "SystemWindowRPCExchanger.h"
#include "../SystemWindowRPCEvent.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/NullStream.h"
#include "kxf/System/Win32Error.h"
#include "kxf/Threading/IThreadPool.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "kxf/Application/ICoreApplication.h"
#include <Windows.h>

namespace
{
	constexpr kxf::XChar g_SharedPrefix[] = kxfS("kxf::SystemWindowRPCExchanger");
}

namespace kxf
{
	void SystemWindowRPCExchanger::OnDataRecievedCommon(IInputStream& stream, SystemWindowRPCEvent& event, const String& clientID)
	{
		auto& procedure = event.m_Procedure;
		Serialization::ReadObject(stream, procedure);

		if (procedure)
		{
			if (!OnDataRecievedFilter(procedure))
			{
				return;
			}

			// Override client ID
			if (!clientID.IsEmpty())
			{
				procedure.m_ClientID = clientID;
			}

			// Set stream with serialized parameters for an event handler to read from
			if (procedure.HasParameters())
			{
				// The stream is not at its initial position at this point (we've read the procedure info)
				// so 'RawSetParameters' saves its position inside the event object.
				event.RawSetParameters(stream);
			}

			// Call event handler if any
			if (m_EvtHandler->ProcessEvent(event, procedure.GetProcedureID()) && procedure.HasResult())
			{
				// If we've processed the event, get the serialized result and write it into shared result buffer
				if (MemoryInputStream resultStream = event.RawGetResult())
				{
					// Allocate shared buffer for the result and the result's size
					const uint64_t resultSize = resultStream.GetSize().ToBytes();
					m_ResultBuffer.Allocate(resultSize + sizeof(resultSize), MemoryProtection::RW, GetResultBufferName(), m_KernelScope);

					// Write result size and the result
					MemoryOutputStream stream = m_ResultBuffer.GetOutputStream();
					Serialization::WriteObject(stream, resultSize);
					stream.Write(resultStream);
				}
			}
		}
	}

	size_t SystemWindowRPCExchanger::GetControlBufferSize() const
	{
		return 64;
	}
	String SystemWindowRPCExchanger::GetControlBufferName() const
	{
		return Format("{}:{}-ControlBuffer", g_SharedPrefix, m_SessionID);
	}
	String SystemWindowRPCExchanger::GetResultBufferName() const
	{
		return Format("{}:{}-ResultBuffer", g_SharedPrefix, m_SessionID);
	}
	String SystemWindowRPCExchanger::GetSessionMutexName() const
	{
		return Format("{}:{}-SessionMutex", g_SharedPrefix, m_SessionID);
	}

	void SystemWindowRPCExchanger::OnInitialize(const String& sessionID, IEvtHandler& evtHandler, std::shared_ptr<IThreadPool> threadPool, FlagSet<RPCExchangeFlag> flags)
	{
		m_SessionID = sessionID;
		m_EvtHandler = &evtHandler;
		m_KernelScope = flags.Contains(RPCExchangeFlag::GlobalSession) ? KernelObjectNamespace::Global : KernelObjectNamespace::Local;
	}
	void SystemWindowRPCExchanger::OnTerminate()
	{
		m_ReceivingTarget.Destroy();
		m_SessionMutex.Destroy();
		m_ControlBuffer.Free();
	}

	MemoryInputStream SystemWindowRPCExchanger::SendData(SystemWindow window, const SystemWindowRPCProcedure& procedure, const MemoryStreamBuffer& buffer, bool discardResult)
	{
		m_ResultBuffer.ZeroBuffer();

		COPYDATASTRUCT parametersBufferData = {};
		parametersBufferData.lpData = const_cast<void*>(buffer.GetBufferStart());
		parametersBufferData.cbData = buffer.GetBufferSize();
		auto sendResult = window.SendMessage(WM_COPYDATA, 0, reinterpret_cast<intptr_t>(&parametersBufferData));

		if (!discardResult && procedure.HasResult() && sendResult && m_ResultBuffer.Open(GetResultBufferName(), 0, MemoryProtection::RW, m_KernelScope))
		{
			// Retrieve the actual size of the result
			uint64_t actualSize = 0;

			// Assume at least 'sizeof(actualSize)' here since we don't know the size and we don't need to read more anyway
			MemoryInputStream stream(m_ResultBuffer.GetBuffer(), sizeof(actualSize));
			auto read = Serialization::ReadObject(stream, actualSize);

			if (actualSize != 0)
			{
				return MemoryInputStream(reinterpret_cast<const uint8_t*>(m_ResultBuffer.GetBuffer()) + read, actualSize);
			}
		}
		return {};
	}
}
