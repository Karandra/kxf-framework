#pragma once

#ifdef KXF_DYNAMIC_LIBRARY
	#ifdef KXF_LIBRARY
		#define KXF_API __declspec(dllexport)
	#else
		#define KXF_API __declspec(dllimport)
	#endif
#elif defined KXF_STATIC_LIBRARY
	#define KXF_API
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
#include "kxf/Win32/UndefMacros.h"

// Forward declarations for some core classes
namespace kxf
{
	class String;

	template<class TValue>
	class Enumerator;
}
