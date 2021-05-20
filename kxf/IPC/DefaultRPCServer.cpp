#include "KxfPCH.h"
#include "DefaultRPCServer.h"
#include "DefaultRPCEvent.h"
#include "Private/DefaultRPCExchangerWindow.h"
#include "kxf/Serialization/BinarySerializer.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/NullStream.h"

namespace kxf
{
	// DefaultRPCServer
	void DefaultRPCServer::Notify(const EventID& eventID)
	{
		DefaultRPCEvent event(*this);
		m_EvtHandler->ProcessEvent(event, eventID);
	}
	void DefaultRPCServer::NotifyClients(const EventID& eventID)
	{
		BroadcastProcedure(eventID);
	}

	bool DefaultRPCServer::DoStartServer(bool notify)
	{
		if (m_SessionMutex.CreateAcquired(GetSessionMutexName()) && m_ControlBuffer.AllocateGlobal(GetControlBufferSize(), MemoryProtection::RW, GetControlBufferName()))
		{
			m_ReceivingWindow = new DefaultRPCExchangerWindow(*this, m_SessionID);

			// Write out everything that the client will need to connect to the server
			auto stream = m_ControlBuffer.GetOutputStream();
			Serialization::WriteObject(*stream, static_cast<uint32_t>(::GetCurrentProcessId()));
			Serialization::WriteObject(*stream, reinterpret_cast<void*>(m_ReceivingWindow->GetHandle()));

			// Notify server started
			if (notify)
			{
				Notify(IRPCEvent::EvtServerStarted);
				NotifyClients(IRPCEvent::EvtServerStarted);
			}
			return true;
		}

		DoTerminateServer();
		return false;
	}
	void DefaultRPCServer::DoTerminateServer(bool notify)
	{
		if (m_SessionMutex && notify)
		{
			Notify(IRPCEvent::EvtServerTerminated);
			NotifyClients(IRPCEvent::EvtServerTerminated);
		}

		OnTerminate();
		m_UserEvtHandler = nullptr;
		m_ServiceEvtHandler.SetNextHandler(nullptr);
		m_Clients.clear();
	}

	// Private::DefaultRPCExchanger
	void DefaultRPCServer::OnDataRecieved(IInputStream& stream)
	{
		DefaultRPCEvent event(*this);
		DefaultRPCExchanger::OnDataRecievedCommon(stream, event);
	}

	DefaultRPCServer::DefaultRPCServer()
	{
		m_ServiceEvtHandler.Bind(IRPCEvent::EvtClientConnected, [&](IRPCEvent& event)
		{
			m_Clients.emplace(static_cast<DefaultRPCEvent&>(event).GetProcedure().GetOriginHandle());
		});
		m_ServiceEvtHandler.Bind(IRPCEvent::EvtClientDisconnected, [&](IRPCEvent& event)
		{
			m_Clients.erase(static_cast<DefaultRPCEvent&>(event).GetProcedure().GetOriginHandle());
		});
	}
	DefaultRPCServer::~DefaultRPCServer()
	{
		DoTerminateServer(true);
	}

	// IRPCServer
	bool DefaultRPCServer::IsServerRunning() const
	{
		return !m_SessionMutex.IsNull();
	}
	bool DefaultRPCServer::StartServer(const UniversallyUniqueID& sessionID, IEvtHandler& evtHandler)
	{
		if (!m_SessionMutex)
		{
			m_UserEvtHandler = &evtHandler;
			m_ServiceEvtHandler.SetNextHandler(&evtHandler);

			OnInitialize(sessionID, m_ServiceEvtHandler);
			return DoStartServer(true);
		}
		return false;
	}
	void DefaultRPCServer::TerminateServer()
	{
		DoTerminateServer(true);
	}

	void DefaultRPCServer::RawBroadcastProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount)
	{
		if (!m_Clients.empty() && m_SessionMutex && procedureID)
		{
			DefaultRPCProcedure procedure(procedureID, m_ReceivingWindow->GetHandle(), parametersCount);

			MemoryOutputStream stream;
			Serialization::WriteObject(stream, procedure);
			if (procedure.HasParameters())
			{
				stream.Write(parameters);
			}

			for (void* handle: m_Clients)
			{
				SendData(handle, procedure, stream.GetStreamBuffer());
			}
		}
	}
}
