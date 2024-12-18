#include "kxf-pch.h"
#include "Common.h"
#include "URI.h"
#include "kxf/Utility/ScopeGuard.h"

#include <WinINet.h>
#include <WinDNS.h>
#include <ws2tcpip.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf::Network
{
	bool IsInternetAvailable() noexcept
	{
		DWORD flags = 0;
		return ::InternetGetConnectedState(&flags, 0);
	}
	String LookupIP(const URI& uri, NetworkHostType ip)
	{
		if (ip == NetworkHostType::IPv4 || ip == NetworkHostType::IPv6)
		{
			DNS_RECORD* infoDNS = nullptr;
			Utility::ScopeGuard atExit([&]()
			{
				if (infoDNS)
				{
					DnsRecordListFree(infoDNS, DnsFreeRecordListDeep);
				}
			});

			String hostName = uri.GetServer();
			if (::DnsQuery_W(hostName.wc_str(), ip == NetworkHostType::IPv6 ? DNS_TYPE_A6 : DNS_TYPE_A, DNS_QUERY_STANDARD|DNS_QUERY_BYPASS_CACHE, nullptr, &infoDNS, nullptr) == 0)
			{
				bool isSuccess = false;
				wchar_t buffer[64] = {};

				if (ip == NetworkHostType::IPv6)
				{
					isSuccess = ::InetNtopW(AF_INET6, &infoDNS->Data.AAAA.Ip6Address, buffer, std::size(buffer)) != nullptr;
				}
				else
				{
					isSuccess = ::InetNtopW(AF_INET, &infoDNS->Data.A.IpAddress, buffer, std::size(buffer)) != nullptr;
				}

				if (isSuccess)
				{
					return buffer;
				}
			}
		}
		return {};
	}
}
