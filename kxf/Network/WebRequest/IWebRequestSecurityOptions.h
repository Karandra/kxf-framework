#pragma once
#include "Common.h"
#include "Options.h"

namespace kxf
{
	class IWebRequestSecurityOptions: public RTTI::Interface<IWebRequestSecurityOptions>
	{
		kxf_RTTI_DeclareIID(IWebRequestSecurityOptions, {0xa6c7cb4d, 0x3695, 0x4d0d, {0xbb, 0xbf, 0xfa, 0x4d, 0xd7, 0x9f, 0x90, 0xc3}});

		public:
			virtual bool SetUseSSL(WebRequestOption3 option) = 0;
			virtual bool SetSSLVersion(WebRequestSSLVersion version) = 0;
			virtual bool SetMaxSSLVersion(WebRequestSSLVersion version) = 0;

			virtual bool SetVerifyPeer(WebRequestOption2 option) = 0;
			virtual bool SetVerifyHost(WebRequestOption2 option) = 0;
			virtual bool SetVerifyStatus(WebRequestOption2 option) = 0;
	};
}
