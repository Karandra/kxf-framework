#pragma once
#include "WebRequest/IWebSession.h"
#include "WebRequest/IWebRequest.h"
#include "WebRequest/IWebResponse.h"
#include "WebRequest/IWebAuthChallenge.h"
#include "WebRequest/WebRequestEvent.h"
#include "WebRequest/WebRequestHeader.h"

#include "WebRequest/IWebRequestOptions.h"
#include "WebRequest/IWebRequestAuthOptions.h"
#include "WebRequest/IWebRequestProxyOptions.h"
#include "WebRequest/IWebRequestSecurityOptions.h"

#include "kxf/Core/OptionalPtr.h"

namespace kxf
{
	class IAsyncTaskExecutor;
}

namespace kxf::Network
{
	KXF_API std::unique_ptr<IWebSession> CreateWebSession(const URI& uri, std::shared_ptr<IAsyncTaskExecutor> taskExecutor = {});
}
