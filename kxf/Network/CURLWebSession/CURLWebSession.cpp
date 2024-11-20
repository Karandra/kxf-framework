#include "KxfPCH.h"
#include "CURLWebSession.h"
#include "CURLWebRequest.h"
#include "LibCURL.h"
#include "kxf/FileSystem/IFileSystem.h"

namespace
{
	constexpr char g_Name[] = "libcurl";
	constexpr char g_License[] = R"~~~(COPYRIGHT AND PERMISSION NOTICE

Copyright (c) 1996 - 2021, Daniel Stenberg, daniel@haxx.se, and many contributors, see the THANKS file.

All rights reserved.

Permission to use, copy, modify, and distribute this software for any purpose with or without fee is hereby granted, provided that the above copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of a copyright holder shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization of the copyright holder.)~~~";
}

namespace kxf
{
	CURLWebSession::CURLWebSession(std::shared_ptr<IAsyncTaskExecutor> taskExecutor)
	{
		if (CURL::Private::Initialize())
		{
			BasicWebSession::DoInitialize(std::move(taskExecutor));
		}
	}

	// IWebSession
	std::shared_ptr<IWebRequest> CURLWebSession::CreateRequest(const URI& uri)
	{
		auto request = std::make_shared<CURLWebRequest>(*this, m_CommonHeaders, uri);
		if (!request->IsNull())
		{
			request->WeakRef(request);
			return request;
		}
		return nullptr;
	}

	IFileSystem& CURLWebSession::GetFileSystem() const
	{
		return m_FileSystem ? *m_FileSystem : FileSystem::GetNullFileSystem();
	}
	void CURLWebSession::SetFileSystem(IFileSystem& fileSystem)
	{
		if (!fileSystem.IsNull() && fileSystem.IsLookupScoped())
		{
			m_FileSystem = &fileSystem;
		}
		else
		{
			m_FileSystem = nullptr;
		}
	}

	String CURLWebSession::GetDefaultUserAgent() const
	{
		return Format("{}/{}", g_Name, LIBCURL_VERSION);
	}

	// ILibraryInfo
	String CURLWebSession::GetName() const
	{
		return g_Name;
	}
	Version CURLWebSession::GetVersion() const
	{
		return LIBCURL_VERSION;
	}
	URI CURLWebSession::GetHomePage() const
	{
		return "https://curl.se";
	}
	uint32_t CURLWebSession::GetAPILevel() const
	{
		return LIBCURL_VERSION_NUM;
	}

	String CURLWebSession::GetLicense() const
	{
		return g_License;
	}
	String CURLWebSession::GetLicenseName() const
	{
		return "MIT/X inspired";
	}
	String CURLWebSession::GetCopyright() const
	{
		return LIBCURL_COPYRIGHT;
	}
}
