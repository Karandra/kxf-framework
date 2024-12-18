#pragma once
#include "Common.h"
#include "RPCEvent.h"
#include "Private/RPCExchange.h"

namespace kxf
{
	class IRPCClient;
	class IEvtHandler;
	class IThreadPool;
}

namespace kxf
{
	class IRPCServer: public RTTI::Interface<IRPCServer>
	{
		kxf_RTTI_DeclareIID(IRPCServer, {0x6b534745, 0x90a7, 0x4e91, {0x87, 0xab, 0x44, 0x8b, 0x99, 0xcc, 0x66, 0xb0}});

		public:
			virtual bool IsServerRunning() const = 0;
			virtual bool StartServer(const String& sessionID, IEvtHandler& evtHandler, std::shared_ptr<IThreadPool> threadPool = {}, FlagSet<RPCExchangeFlag> flags = {}) = 0;
			virtual void TerminateServer() = 0;
			virtual String GetSessionID() const = 0;

			virtual MemoryInputStream RawInvokeProcedure(const String& clientID, const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult) = 0;
			virtual void RawBroadcastProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount) = 0;

		public:
			template<class TReturn = void, class... Args>
			TReturn InvokeProcedure(const String& clientID, const EventID& procedureID, Args&&... arg)
			{
				return IPC::Private::InvokeProcedure<TReturn>([&](MemoryOutputStream& parametersStream, size_t parametersCount, bool hasResult)
				{
					MemoryInputStream parametersInputStream(parametersStream.DetachStreamBuffer());
					return RawInvokeProcedure(clientID, procedureID, parametersInputStream, parametersCount, hasResult);
				}, std::forward<Args>(arg)...);
			}

			template<class... Args>
			void BroadcastProcedure(const EventID& procedureID, Args&&... arg)
			{
				return IPC::Private::InvokeProcedure<void>([&](MemoryOutputStream& parametersStream, size_t parametersCount, bool hasResult)
				{
					MemoryInputStream parametersInputStream(parametersStream.DetachStreamBuffer());
					RawBroadcastProcedure(procedureID, parametersInputStream, parametersCount);
				}, std::forward<Args>(arg)...);
			}
	};
}
