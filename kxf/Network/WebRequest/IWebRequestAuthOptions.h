#pragma once
#include "Common.h"
#include "Options.h"

namespace kxf
{
	class IWebRequestAuthOptions: public RTTI::Interface<IWebRequestAuthOptions>
	{
		kxf_RTTI_DeclareIID(IWebRequestAuthOptions, {0x268dd52, 0x8f20, 0x40c0, {0x81, 0x5f, 0xaf, 0x36, 0x93, 0x8, 0x61, 0xac}});

		public:
			virtual bool SetAuthMethod(WebRequestAuthMethod method) = 0;
			virtual bool SetAuthMethods(FlagSet<WebRequestAuthMethod> methods) = 0;

			virtual bool SetUserName(const String& userName) = 0;
			virtual bool SetUserPassword(const String& userPassword) = 0;
	};
}
