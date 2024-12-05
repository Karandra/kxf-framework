#pragma once
#include "kxf/Common.hpp"
#include "TypeTraits.h"
#include <type_traits>
#include <algorithm>
#include <numeric>
#include <utility>
#include <array>

namespace kxf::Utility
{
	template<class TFunc, class... Args>
	constexpr void ForEachParameterPackItem(TFunc&& func, Args&&... arg) noexcept
	{
		std::initializer_list<int>{(std::invoke(func, std::forward<Args>(arg)), 0)...};
	}

	template<class... Args>
	constexpr size_t CountOfParameterPack() noexcept
	{
		return sizeof...(Args);
	}

	template<class... Args>
	constexpr bool IsEmptyParameterPack() noexcept
	{
		return CountOfParameterPack<Args...>() == 0;
	}

	template<class... Args>
	constexpr size_t SizeOfParameterPackValues() noexcept
	{
		const constexpr size_t count = CountOfParameterPack<Args...>();
		const constexpr std::array<size_t, count> sizes = {sizeof(Args)...};

		return std::accumulate(sizes.begin(), sizes.end(), static_cast<size_t>(0));
	}

	template<class TPointer, class TValue>
	constexpr void SetIfNotNull(TPointer* ptr, TValue&& value) noexcept
	{
		using T = std::remove_reference_t<TValue>;
		static_assert(std::is_same_v<TPointer, T> || std::is_convertible_v<T, TPointer>, "incompatible types");

		if (ptr)
		{
			*ptr = static_cast<TPointer>(std::forward<TValue>(value));
		}
	}

	template<class TPointer>
	std::unique_ptr<TPointer> AssumeUniquePtr(TPointer* ptr) noexcept
	{
		return std::unique_ptr<TPointer>(ptr);
	}

	template<class TTarget, class TSource>
	std::unique_ptr<TTarget> StaticCastUniquePtr(std::unique_ptr<TSource> source) noexcept
	{
		return std::unique_ptr<TTarget>(static_cast<TTarget*>(source.release()));
	}

	template<class TTarget, class TSource>
	std::unique_ptr<TTarget> DynamicCastUniquePtr(std::unique_ptr<TSource> source) noexcept
	{
		if (auto ptr = dynamic_cast<TTarget*>(source.get()))
		{
			std::unique_ptr<TTarget> result(ptr);
			source.release();

			return result;
		}
		return nullptr;
	}

	template<class T, class... Args> requires((std::is_integral_v<T> || std::is_enum_v<T>) && (std::is_enum_v<std::remove_const_t<std::remove_reference_t<Args>>> && ...))
	constexpr T CombineEnumFlags(Args&&... arg) noexcept
	{
		return static_cast<T>((static_cast<std::underlying_type_t<std::remove_const_t<std::remove_reference_t<Args>>>>(arg) | ...));
	}
}

namespace kxf::Utility
{
	template<typename>
	struct ArraySize;

	template<typename T, size_t N>
	struct ArraySize<T[N]>
	{
		static constexpr size_t value = N;
	};

	template<typename T, size_t N>
	struct ArraySize<std::array<T, N>>
	{
		static constexpr size_t value = N;
	};
}
