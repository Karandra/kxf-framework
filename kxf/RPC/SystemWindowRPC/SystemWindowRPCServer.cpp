#include "kxf-pch.h"
#include "SystemWindowRPCServer.h"
#include "SystemWindowRPCEvent.h"
#include "Private/SystemWindowRPCExchangerTarget.h"
#include "kxf/Log/ScopedLogger.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/NullStream.h"
#include "kxf/System/SystemProcess.h"
#include "kxf/Serialization/BinarySerializer.h"

namespace kxf
{
	// SystemWindowRPCServer
	void SystemWindowRPCServer::Notify(const EventID& eventID)
	{
		SystemWindowRPCEvent event(*this);
		m_EvtHandler->ProcessEvent(event, eventID);
	}
	void SystemWindowRPCServer::NotifyClients(const EventID& eventID)
	{
		BroadcastProcedure(eventID);
	}

	void SystemWindowRPCServer::HandleClientEvent(SystemWindowRPCEvent& event, bool add)
	{
		const auto& procedure = event.GetProcedure();
		if (auto id = procedure.GetClientID(); !id.IsEmpty())
		{
			if (add)
			{
				if (m_UniqueClients.emplace(id, procedure.GetOriginWindow()).second)
				{
					event.Skip();
				}
			}
			else
			{
				if (m_UniqueClients.erase(id) != 0)
				{
					event.Skip();
				}
			}
		}
		else if (auto window = procedure.GetOriginWindow())
		{
			if (add)
			{
				if (m_AnonymousClients.emplace(window).second)
				{
					event.Skip();
				}
			}
			else
			{
				if (m_AnonymousClients.erase(window) != 0)
				{
					event.Skip();
				}
			}
		}
	}
	void SystemWindowRPCServer::CleanupClients()
	{
		// Remove any invalid clients
		for (auto it = m_UniqueClients.begin(); it != m_UniqueClients.end();)
		{
			if (!it->second.DoesExist())
			{
				it = m_UniqueClients.erase(it);
			}
			else
			{
				++it;
			}
		}

		for (auto it = m_AnonymousClients.begin(); it != m_AnonymousClients.end();)
		{
			if (!it->DoesExist())
			{
				it = m_AnonymousClients.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	bool SystemWindowRPCServer::DoStartServer()
	{
		try
		{
			size_t controlBufferSize = GetControlBufferSize();
			if (m_SessionMutex.CreateAcquired(GetSessionMutexName(), m_KernelScope) && m_ControlBuffer.Allocate(controlBufferSize, MemoryProtection::RW, GetControlBufferName(), m_KernelScope))
			{
				if (m_ReceivingTarget.Create(m_SessionID))
				{
					// Write out everything that the client will need to connect to the server
					MemoryOutputStream stream = m_ControlBuffer.GetOutputStream();
					Serialization::WriteObject(stream, static_cast<uint64_t>(controlBufferSize));
					Serialization::WriteObject(stream, SystemProcess::GetCurrentProcess().GetID());
					Serialization::WriteObject(stream, m_ReceivingTarget.GetWindow().GetHandle());

					// Notify server started
					Notify(RPCEvent::EvtServerStarted);
					NotifyClients(RPCEvent::EvtServerStarted);
					return true;
				}
			}
		}
		catch (const BinarySerializerException& e)
		{
			Log::Error("Serialization exception: {}", e.what());
		}

		DoTerminateServer(false);
		return false;
	}
	void SystemWindowRPCServer::DoTerminateServer(bool notify)
	{
		if (m_SessionMutex && notify)
		{
			Notify(RPCEvent::EvtServerTerminated);
			NotifyClients(RPCEvent::EvtServerTerminated);
		}

		OnTerminate();
		m_UserEvtHandler = nullptr;
		m_ServiceEvtHandler.SetNextHandler(nullptr);
		m_UniqueClients.clear();
		m_AnonymousClients.clear();
	}
	MemoryInputStream SystemWindowRPCServer::DoInvokeProcedure(const String& clientID, const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult)
	{
		CleanupClients();

		if (m_SessionMutex && procedureID)
		{
			auto PrepareData = [&](IOutputStream& stream)
			{
				SystemWindowRPCProcedure procedure(procedureID, m_ReceivingTarget.GetWindow(), parametersCount, hasResult);

				Serialization::WriteObject(stream, procedure);
				if (procedure.HasParameters())
				{
					stream.Write(parameters);
				}

				return procedure;
			};

			if (!clientID.IsEmpty())
			{
				if (auto it = m_UniqueClients.find(clientID); it != m_UniqueClients.end())
				{
					MemoryOutputStream stream;
					auto procedure = PrepareData(stream);

					return SendData(it->second, procedure, stream.GetStreamBuffer());
				}
			}
			else
			{
				MemoryOutputStream stream;
				auto procedure = PrepareData(stream);

				for (auto&& [id, window]: m_UniqueClients)
				{
					SendData(window, procedure, stream.GetStreamBuffer(), true);
				}
				for (auto window: m_AnonymousClients)
				{
					SendData(window, procedure, stream.GetStreamBuffer(), true);
				}
			}
		}
		return {};
	}

	// SystemWindowRPCExchanger
	void SystemWindowRPCServer::OnDataRecieved(IInputStream& stream)
	{
		if (m_SessionMutex)
		{
			SystemWindowRPCEvent event(*this);
			SystemWindowRPCExchanger::OnDataRecievedCommon(stream, event);
		}
	}
	bool SystemWindowRPCServer::OnDataRecievedFilter(const SystemWindowRPCProcedure& procedure)
	{
		if (!procedure.m_ClientID.IsEmpty())
		{
			if (auto it = m_UniqueClients.find(procedure.m_ClientID); it != m_UniqueClients.end())
			{
				return it->second.GetHandle() == procedure.m_OriginHandle;
			}
			else
			{
				return procedure.m_ProcedureID == RPCEvent::EvtClientConnected;
			}
		}
		else
		{
			return m_AnonymousClients.count(procedure.m_OriginHandle) != 0 || procedure.m_ProcedureID == RPCEvent::EvtClientConnected;
		}
		return false;
	}

	// SystemWindowRPCServer
	SystemWindowRPCServer::SystemWindowRPCServer()
	{
		m_ServiceEvtHandler.Bind(RPCEvent::EvtClientConnected, [&](RPCEvent& event)
		{
			HandleClientEvent(static_cast<SystemWindowRPCEvent&>(event), true);
		});
		m_ServiceEvtHandler.Bind(RPCEvent::EvtClientDisconnected, [&](RPCEvent& event)
		{
			HandleClientEvent(static_cast<SystemWindowRPCEvent&>(event), false);
		}, BindEventFlag::AlwaysSkip);
	}
	SystemWindowRPCServer::~SystemWindowRPCServer()
	{
		DoTerminateServer(true);
	}

	// IRPCServer
	bool SystemWindowRPCServer::IsServerRunning() const
	{
		return !m_SessionMutex.IsNull();
	}
	bool SystemWindowRPCServer::StartServer(const String& sessionID, IEvtHandler& evtHandler, std::shared_ptr<IThreadPool> threadPool, FlagSet<RPCExchangeFlag> flags)
	{
		if (!m_SessionMutex)
		{
			m_UserEvtHandler = &evtHandler;
			m_ServiceEvtHandler.SetNextHandler(&evtHandler);

			OnInitialize(sessionID, m_ServiceEvtHandler, std::move(threadPool), flags);
			return DoStartServer();
		}
		return false;
	}
	void SystemWindowRPCServer::TerminateServer()
	{
		DoTerminateServer(true);
	}

	MemoryInputStream SystemWindowRPCServer::RawInvokeProcedure(const String& clientID, const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult)
	{
		if (!clientID.IsEmpty())
		{
			return DoInvokeProcedure(clientID, procedureID, parameters, parametersCount, hasResult);
		}
		return {};
	}
	void SystemWindowRPCServer::RawBroadcastProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount)
	{
		DoInvokeProcedure({}, procedureID, parameters, parametersCount, false);
	}

	// SystemWindowRPCServer
	size_t SystemWindowRPCServer::EnumUniqueClients(std::move_only_function<CallbackCommand(const String&, SystemWindow)> func) const
	{
		if (!func)
		{
			return 0;
		}

		size_t count = 0;
		for (auto it = m_UniqueClients.begin(); it != m_UniqueClients.end();)
		{
			if (it->second.DoesExist())
			{
				auto command = std::invoke(func, it->first, it->second);
				count++;

				if (command != CallbackCommand::Continue)
				{
					break;
				}
			}
		}

		return count;
	}
	size_t SystemWindowRPCServer::EnumAnonymousClients(std::move_only_function<CallbackCommand(SystemWindow)> func) const
	{
		if (!func)
		{
			return 0;
		}

		size_t count = 0;
		for (auto it = m_AnonymousClients.begin(); it != m_AnonymousClients.end();)
		{
			if (it->DoesExist())
			{
				auto command = std::invoke(func, *it);
				count++;

				if (command != CallbackCommand::Continue)
				{
					break;
				}
			}
		}

		return count;
	}
}
