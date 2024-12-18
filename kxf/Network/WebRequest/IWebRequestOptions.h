#pragma once
#include "Common.h"
#include "Options.h"

namespace kxf
{
	class IWebRequestOptions: public RTTI::Interface<IWebRequestOptions>
	{
		kxf_RTTI_DeclareIID(IWebRequestOptions, {0x6a4c956d, 0x2842, 0x4a2f, {0xaa, 0x30, 0x31, 0x16, 0xb1, 0x58, 0xe9, 0x37}});

		public:
			virtual bool SetURI(const URI& uri) = 0;
			virtual bool SetPort(uint16_t port) = 0;
			virtual bool SetMethod(const String& method) = 0;
			virtual bool SetDefaultProtocol(const String& protocol) = 0;
			virtual bool SetAllowedProtocols(FlagSet<WebRequestProtocol> protocols) = 0;
			virtual bool SetHTTPVersion(WebRequestHTTPVersion option) = 0;
			virtual bool SetIPVersion(WebRequestIPVersion option) = 0;

			virtual bool SetServiceName(const String& name) = 0;
			virtual bool SetAllowRedirection(WebRequestOption2 option) = 0;
			virtual bool SetRedirectionProtocols(FlagSet<WebRequestProtocol> protocols) = 0;
			virtual bool SetResumeOffset(DataSize offset) = 0;

			virtual bool SetRequestTimeout(const TimeSpan& timeout) = 0;
			virtual bool SetConnectionTimeout(const TimeSpan& timeout) = 0;

			virtual bool SetMaxSendRate(const TransferRate& rate) = 0;
			virtual bool SetMaxReceiveRate(const TransferRate& rate) = 0;

			virtual bool SetKeepAlive(WebRequestOption2 option) = 0;
			virtual bool SetKeepAliveIdle(const TimeSpan& interval) = 0;
			virtual bool SetKeepAliveInterval(const TimeSpan& interval) = 0;

		public:
			bool SetMethod(WebRequestMethod method);
	};
}
