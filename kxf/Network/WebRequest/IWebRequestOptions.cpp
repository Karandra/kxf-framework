#include "kxf-pch.h"
#include "IWebRequestOptions.h"

namespace
{
	constexpr struct
	{
		static constexpr char OPTIONS[] = "OPTIONS";
		static constexpr char GET[] = "GET";
		static constexpr char HEAD[] = "HEAD";
		static constexpr char POST[] = "POST";
		static constexpr char PUT[] = "PUT";
		static constexpr char DELETE[] = "DELETE";
		static constexpr char TRACE[] = "TRACE";
		static constexpr char CONNECT[] = "CONNECT";
		static constexpr char PATCH[] = "PATCH";
	} g_MethodNames;
}

namespace kxf
{
	bool IWebRequestOptions::SetMethod(WebRequestMethod method)
	{
		switch (method)
		{
			case WebRequestMethod::None:
			{
				return SetMethod({});
			}
			case WebRequestMethod::OPTIONS:
			{
				return SetMethod(g_MethodNames.OPTIONS);
			}
			case WebRequestMethod::GET:
			{
				return SetMethod(g_MethodNames.GET);
			}
			case WebRequestMethod::HEAD:
			{
				return SetMethod(g_MethodNames.HEAD);
			}
			case WebRequestMethod::POST:
			{
				return SetMethod(g_MethodNames.POST);
			}
			case WebRequestMethod::PUT:
			{
				return SetMethod(g_MethodNames.PUT);
			}
			case WebRequestMethod::DELETE:
			{
				return SetMethod(g_MethodNames.DELETE);
			}
			case WebRequestMethod::TRACE:
			{
				return SetMethod(g_MethodNames.TRACE);
			}
			case WebRequestMethod::CONNECT:
			{
				return SetMethod(g_MethodNames.CONNECT);
			}
			case WebRequestMethod::PATCH:
			{
				return SetMethod(g_MethodNames.PATCH);
			}
		};
		return false;
	}
}
