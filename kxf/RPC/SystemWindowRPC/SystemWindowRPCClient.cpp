#include "kxf-pch.h"
#include "SystemWindowRPCClient.h"
#include "SystemWindowRPCEvent.h"
#include "Private/SystemWindowRPCExchangerTarget.h"
#include "kxf/Log/ScopedLogger.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/NullStream.h"
#include "kxf/IO/MemoryStream.h"
#include "kxf/Serialization/BinarySerializer.h"

namespace kxf
{
	// SystemWindowRPCClient
	void SystemWindowRPCClient::Notify(const EventID& eventID)
	{
		SystemWindowRPCEvent event(*this);
		m_EvtHandler->ProcessEvent(event, eventID);
	}
	void SystemWindowRPCClient::NotifyServer(const EventID& eventID)
	{
		InvokeProcedure(eventID);
	}

	bool SystemWindowRPCClient::DoConnectToServer()
	{
		try
		{
			if (m_SessionMutex.Open(GetSessionMutexName(), m_KernelScope) && m_ControlBuffer.Open(GetControlBufferName(), 0, MemoryProtection::Read, m_KernelScope))
			{
				// Read control parameters
				size_t initialControlBufferSize = GetControlBufferSize();
				MemoryInputStream stream = m_ControlBuffer.GetInputStreamUnchecked(initialControlBufferSize);

				uint64_t controlBufferSize = 0;
				Serialization::ReadObject(stream, controlBufferSize);
				if (controlBufferSize == initialControlBufferSize)
				{
					Serialization::ReadObject(stream, m_ServerPID);
					Serialization::ReadObject(stream, m_ServerHandle);

					if (SystemWindow(m_ServerHandle).DoesExist() && m_ReceivingTarget.Create(m_SessionID))
					{
						Notify(RPCEvent::EvtClientConnected);
						NotifyServer(RPCEvent::EvtClientConnected);
						return true;
					}
				}
			}
		}
		catch (const BinarySerializerException& e)
		{
			Log::Error("Serialization exception: {}", e.what());
		}

		DoDisconnectFromServer(false);
		return false;
	}
	void SystemWindowRPCClient::DoDisconnectFromServer(bool notify)
	{
		if (m_SessionMutex && notify)
		{
			Notify(RPCEvent::EvtClientDisconnected);
			NotifyServer(RPCEvent::EvtClientDisconnected);
		}

		m_ServerPID = 0;
		m_ServerHandle = nullptr;
		m_UserEvtHandler = nullptr;
		m_ServiceEvtHandler.SetNextHandler(nullptr);
		OnTerminate();
	}

	// SystemWindowRPCExchanger
	void SystemWindowRPCClient::OnDataRecieved(IInputStream& stream)
	{
		if (m_SessionMutex)
		{
			SystemWindowRPCEvent event(*this);
			SystemWindowRPCExchanger::OnDataRecievedCommon(stream, event, m_ClientID);
		}
	}
	bool SystemWindowRPCClient::OnDataRecievedFilter(const SystemWindowRPCProcedure& procedure)
	{
		return procedure.m_OriginHandle == m_ServerHandle;
	}

	// SystemWindowRPCClient
	SystemWindowRPCClient::SystemWindowRPCClient()
	{
		// TODO: Watch server status using the provided PID and initiate disconnect event
		// when the server terminates without proper notifications (i.e crashes).
	}
	SystemWindowRPCClient::~SystemWindowRPCClient()
	{
		DoDisconnectFromServer(true);
	}

	// IRPCClient
	bool SystemWindowRPCClient::IsConnectedToServer() const
	{
		return !m_SessionMutex.IsNull();
	}
	bool SystemWindowRPCClient::ConnectToServer(const String& sessionID, IEvtHandler& evtHandler, const String& clientID, std::shared_ptr<IThreadPool> threadPool, FlagSet<RPCExchangeFlag> flags )
	{
		if (!m_SessionMutex)
		{
			m_ClientID = clientID;
			m_UserEvtHandler = &evtHandler;
			m_ServiceEvtHandler.SetNextHandler(&evtHandler);

			OnInitialize(sessionID, m_ServiceEvtHandler, std::move(threadPool), flags);
			return DoConnectToServer();
		}
		return false;
	}
	void SystemWindowRPCClient::DisconnectFromServer()
	{
		DoDisconnectFromServer(true);
	}

	MemoryInputStream SystemWindowRPCClient::RawInvokeProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult)
	{
		if (m_SessionMutex && procedureID)
		{
			SystemWindowRPCProcedure procedure(procedureID, m_ReceivingTarget.GetWindow(), parametersCount, hasResult);
			procedure.m_ClientID = m_ClientID;

			MemoryOutputStream stream;
			Serialization::WriteObject(stream, procedure);
			if (procedure.HasParameters())
			{
				stream.Write(parameters);
			}
			return SendData(m_ServerHandle, procedure, stream.GetStreamBuffer());
		}
		return {};
	}
}
