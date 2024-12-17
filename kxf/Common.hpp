#pragma once

#ifdef KXF_DYNAMIC_LIBRARY
	#ifdef KXF_LIBRARY
		#define KX_API __declspec(dllexport)
	#else
		#define KX_API __declspec(dllimport)
	#endif
#elif defined KXF_STATIC_LIBRARY
	#define KX_API
#endif

// Standard library
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <new>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <atomic>
#include <locale>
#include <optional>
#include <exception>
#include <stdexcept>
#include <type_traits>
#include <algorithm>
#include <numeric>
#include <utility>
#include <compare>
#include <tuple>
#include <span>
#include <bit>

// Essential headers
#include "kxf/wxWidgets/Setup.h"
#include "kxf/Core/StdID.h"
#include "kxf/Core/FlagSet.h"
#include "kxf/Utility/Literals.h"
#include "kxf/System/UndefWindows.h"

// Forward declarations for some core classes
namespace kxf
{
	class String;

	template<class TValue>
	class Enumerator;
}

// wxWidgets
// Link some libraries that aren't linked by wxWidgets itself for some reason

// Required by pretty much any UI component
#pragma comment(lib, "Comctl32.lib")

// Required by various GPU-related functions
#pragma comment(lib, "dxgi.lib")

// Required by network module
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "DNSAPI.lib")
#pragma comment(lib, "WinINet.lib")
#pragma comment(lib, "Wldap32.lib")

// Multimedia
#pragma comment(lib, "Winmm.lib")

// Required by UUID/GUID functions
#pragma comment(lib, "Rpcrt4.lib")

// Required by Crypto module
#pragma comment(lib, "Crypt32.lib")
