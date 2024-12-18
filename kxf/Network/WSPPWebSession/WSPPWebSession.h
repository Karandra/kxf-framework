#pragma once
#include "Common.h"
#include "kxf/Core/ILibraryInfo.h"
#include "../Private/BasicWebSession.h"

namespace kxf
{
	class KXF_API WSPPWebSession final: public RTTI::DynamicImplementation<WSPPWebSession, Private::BasicWebSession, ILibraryInfo>
	{
		friend class WSPPWebRequest;

		private:
			IFileSystem* m_FileSystem = nullptr;
			std::vector<WebRequestHeader> m_CommonHeaders;
			URI m_BaseURI;

		private:
			URI ResolveURI(const URI& uri) const
			{
				return URI(uri).Resolve(m_BaseURI);
			}

		public:
			WSPPWebSession(std::shared_ptr<IAsyncTaskExecutor> taskExecutor = {});

		public:
			// IWebSession
			void SetBaseURI(const URI& uri) override
			{
				m_BaseURI = uri;
			}
			std::shared_ptr<IWebRequest> CreateRequest(const URI& uri) override;

			bool SetCommonHeader(const WebRequestHeader& header, FlagSet<WebRequestHeaderFlag> flags) override
			{
				return SetHeader(m_CommonHeaders, header, flags);
			}
			void ClearCommonHeaders() override
			{
				m_CommonHeaders.clear();
			}

			IFileSystem& GetFileSystem() const override;
			void SetFileSystem(IFileSystem& fileSystem) override;

			String GetDefaultUserAgent() const override;
			void* GetNativeHandle() const override
			{
				return nullptr;
			}

			// ILibraryInfo
			String GetName() const override;
			Version GetVersion() const override;
			URI GetHomePage() const override;
			uint32_t GetAPILevel() const override;

			String GetLicense() const override;
			String GetLicenseName() const override;
			String GetCopyright() const override;
	};
}
