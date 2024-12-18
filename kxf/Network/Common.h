#pragma once
#include "kxf/Common.hpp"
#include "kxf/Core/String.h"
#include "kxf/Core/DateTime.h"
#include "kxf/Core/DataSize.h"
#include "kxf/Utility/Literals.h"

namespace kxf
{
	class URI;
	using TransferRate = DataSize;

	enum class NetworkHostType
	{
		None = -1,

		RegisteredName = 0, // Host is a registered name: 'www.mysite.com'
		IPvFuture = 1, // Host is an IPvFuture address
		IPv4 = 4, // Host is IPv4 address: '192.168.1.100:5050'
		IPv6 = 6, // Host is IPv6 address: '[aa:aa:aa:aa::aa:aa]:5050'
	};
}

namespace kxf::Network
{
	KXF_API bool IsInternetAvailable() noexcept;
	KXF_API String LookupIP(const URI& uri, NetworkHostType ip);
}
