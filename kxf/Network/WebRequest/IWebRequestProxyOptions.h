#pragma once
#include "Common.h"
#include "Options.h"

namespace kxf
{
	class WebRequestHeader;
}

namespace kxf
{
	class IWebRequestProxyOptions: public RTTI::Interface<IWebRequestProxyOptions>
	{
		kxf_RTTI_DeclareIID(IWebRequestProxyOptions, {0xc469b7d0, 0x28a9, 0x4f90, {0x88, 0x2d, 0x56, 0xd8, 0x55, 0xfb, 0x9f, 0xfc}});

		public:
			virtual bool SetProxyURI(const URI& uri) = 0;
			virtual bool SetServiceName(const String& name) = 0;

			virtual bool SetProxyHeader(const WebRequestHeader& header, FlagSet<WebRequestHeaderFlag> flags) = 0;
			virtual void ClearProxyHeaders() = 0;

			virtual bool SetProxyUserName(const String& userName) = 0;
			virtual bool SetProxyUserPassword(const String& userPassword) = 0;

			virtual bool SetProxyAuthMethod(WebRequestAuthMethod method) = 0;
			virtual bool SetProxyAuthMethods(FlagSet<WebRequestAuthMethod> methods) = 0;

			virtual bool SetProxySSLVersion(WebRequestSSLVersion version) = 0;
			virtual bool SetProxyMaxSSLVersion(WebRequestSSLVersion version) = 0;

			virtual bool SetProxyVerifyPeer(WebRequestOption2 option) = 0;
			virtual bool SetProxyVerifyHost(WebRequestOption2 option) = 0;
	};
}
