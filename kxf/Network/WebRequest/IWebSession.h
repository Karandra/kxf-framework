#pragma once
#include "Common.h"
#include "kxf/EventSystem/IEvtHandler.h"

namespace kxf
{
	class IWebRequest;
	class WebRequestHeader;
}

namespace kxf
{
	class IWebSession: public RTTI::ExtendInterface<IWebSession, IEvtHandler>
	{
		kxf_RTTI_DeclareIID(IWebSession, {0x46f42b07, 0xd313, 0x475e, {0x82, 0x41, 0x63, 0xf6, 0x64, 0x4a, 0x85, 0x8c}});

		public:
			virtual void SetBaseURI(const URI& uri) = 0;
			virtual std::shared_ptr<IWebRequest> CreateRequest(const URI& uri) = 0;

			virtual bool SetCommonHeader(const WebRequestHeader& header, FlagSet<WebRequestHeaderFlag> flags) = 0;
			virtual void ClearCommonHeaders() = 0;

			virtual IFileSystem& GetFileSystem() const = 0;
			virtual void SetFileSystem(IFileSystem& fileSystem) = 0;

			virtual String GetDefaultUserAgent() const = 0;
			virtual void* GetNativeHandle() const = 0;
	};
}
