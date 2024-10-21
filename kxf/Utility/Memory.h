#pragma once
#include "kxf/Common.hpp"
#include "kxf/Core/NativeUUID.h"
#include <cstdint>
#include <cstring>
#include <memory>
#include <new>
#include "kxf/System/UndefWindows.h"

namespace kxf::Utility
{
	KX_API void SecureZeroMemory(void* ptr, size_t size) noexcept;

	template<class T, class... Args>
	requires(std::is_constructible_v<T, Args...>)
	constexpr T* ConstructAt(void* buffer, Args&&... arg) noexcept(std::is_nothrow_constructible_v<T, Args...>)
	{
		return std::construct_at<T>(reinterpret_cast<T*>(buffer), std::forward<Args>(arg)...);
	}

	template<class T, class... Args>
	requires(std::is_constructible_v<T, Args...>)
	constexpr T* AlignAndConstructAt(void* buffer, size_t size, size_t alignment, Args&&... arg) noexcept(std::is_nothrow_constructible_v<T, Args...>)
	{
		void* ptr = buffer;
		size_t space = size;
		if (std::align(alignment, size, ptr, space))
		{
			return ConstructAt<T>(ptr, std::forward<Args>(arg)...);
		}
		return nullptr;
	}

	template<class T, class... Args>
	requires(std::is_constructible_v<T, Args...>)
	constexpr T* AlignAndConstructAt(void* buffer, size_t size, Args&&... arg) noexcept(std::is_nothrow_constructible_v<T, Args...>)
	{
		return AlignAndConstructAt<T>(buffer, size, alignof(T), std::forward<Args>(arg)...);
	}

	template<class T>
	constexpr void DestroyAt(void* buffer) noexcept
	{
		std::destroy_at(static_cast<T*>(buffer));
	}

	template<class T>
	constexpr void DestroyAt(T* ptr) noexcept
	{
		std::destroy_at(ptr);
	}

	template<class TFunc>
	requires(std::is_member_function_pointer_v<TFunc>)
	NativeUUID StoreMemberFunction(TFunc func) noexcept
	{
		static_assert(sizeof(func) <= sizeof(NativeUUID), "Member function size must be less or equal to the size of 'NativeUUID' type");

		NativeUUID uuid;
		std::memcpy(&uuid, &func, sizeof(func));
		return uuid;
	}
}

namespace kxf::Utility
{
	template<class T>
	class AlignedHeapBuffer final
	{
		public:
			using ValueType = T;

		private:
			std::allocator<uint8_t> m_Allocator;

			void* m_Source = nullptr;
			void* m_Aligned = nullptr;
			size_t m_Size = 0;

		public:
			AlignedHeapBuffer(size_t size = alignof(T))
				:m_Size(size)
			{
				m_Source = m_Allocator.allocate(size + alignof(ValueType));

				void* toAlign = m_Source;
				m_Aligned = std::align(size, sizeof(T), toAlign, m_Size);
			}
			AlignedHeapBuffer(const AlignedHeapBuffer&) = delete;
			AlignedHeapBuffer(AlignedHeapBuffer&& other) noexcept
			{
				*this = std::move(other);
			}
			~AlignedHeapBuffer()
			{
				m_Allocator.deallocate(reinterpret_cast<uint8_t*>(m_Source), m_Size + alignof(ValueType));
			}

		public:
			void* GetSource() const noexcept
			{
				return m_Source;
			}
			void* GetAligned() const noexcept
			{
				return m_Aligned;
			}
			size_t GetSize() const noexcept
			{
				return m_Size;
			}

		public:
			explicit operator bool() const noexcept
			{
				return m_Aligned != nullptr;
			}
			bool operator!() const noexcept
			{
				return m_Aligned == nullptr;
			}

			AlignedHeapBuffer& operator=(const AlignedHeapBuffer&) = delete;
			AlignedHeapBuffer& operator=(AlignedHeapBuffer&& other) noexcept
			{
				m_Allocator = std::move(other.m_Allocator);

				m_Source = other.m_Source;
				other.m_Source = nullptr;

				m_Aligned = other.m_Aligned;
				other.m_Aligned = nullptr;

				m_Size = other.m_Size;
				other.m_Size = nullptr;

				return *this;
			}
	};
}

namespace kxf::Utility
{
	template<class TLow_, class THigh_ = TLow_, bool isSigned = false>
	requires(std::is_integral_v<TLow_> && std::is_integral_v<THigh_> && sizeof(TLow_) == sizeof(THigh_))
	class CompositeInteger final
	{
		public:
			using TLow = TLow_;
			using THigh = THigh_;
			using TFull = typename
				std::conditional_t<sizeof(TLow_) == sizeof(uint8_t), std::conditional_t<isSigned, int16_t, uint16_t>,
				std::conditional_t<sizeof(TLow_) == sizeof(uint16_t), std::conditional_t<isSigned, int32_t, uint32_t>,
				std::conditional_t<sizeof(TLow_) == sizeof(uint32_t), std::conditional_t<isSigned, int64_t, uint64_t>,
				void>>
			>;

		private:
			struct
			{
				TLow Low;
				THigh High;
			} m_Value;

		public:
			constexpr CompositeInteger() noexcept
			{
				std::memset(&m_Value, 0, sizeof(m_Value));
			}
			constexpr CompositeInteger(TLow low, THigh high) noexcept
			{
				m_Value.Low = low;
				m_Value.High = high;
			}

			template<std::enable_if_t<!std::is_void_v<TFull>, int> = 0>
			constexpr CompositeInteger(TFull fullValue) noexcept
			{
				m_Value = std::bit_cast<decltype(m_Value)>(fullValue);
			}

		public:
			constexpr bool IsNull() const noexcept
			{
				constexpr CompositeInteger null{};
				return std::memcmp(&m_Value, &null, sizeof(null)) == 0;
			}
			constexpr bool IsSigned() const noexcept
			{
				return isSigned;
			}
			constexpr bool IsIntegerRepresentable() const noexcept
			{
				return !std::is_void_v<TFull>;
			}

			constexpr size_t GetSize() noexcept
			{
				return sizeof(m_Value);
			}
			constexpr const void* GetData() const noexcept
			{
				return &m_Value;
			}
			constexpr void* GetData() noexcept
			{
				return &m_Value;
			}

			constexpr void SwapParts() noexcept
			{
				std::swap(m_Value.Low, m_Value.High);
			}
			constexpr TLow GetLow() const noexcept
			{
				return m_Value.Low;
			}
			constexpr THigh GetHigh() const noexcept
			{
				return m_Value.High;
			}

			template<std::enable_if_t<!std::is_void_v<TFull>, int> = 0>
			constexpr TFull GetFull() const noexcept
			{
				return std::bit_cast<TFull>(m_Value);
			}

		public:
			constexpr std::strong_ordering operator<=>(const CompositeInteger& other) const noexcept
			{
				auto result = std::memcmp(&m_Value, &other.m_Value, sizeof(m_Value));
				if (result < 0)
				{
					return std::strong_ordering::less;
				}
				else if (result > 0)
				{
					return std::strong_ordering::greater;
				}
				else
				{
					return std::strong_ordering::equal;
				}
			}
			constexpr bool operator==(const CompositeInteger& other) const noexcept
			{
				return (*this <=> other) == 0;
			}
			constexpr bool operator!=(const CompositeInteger& other) const noexcept
			{
				return (*this <=> other) != 0;
			}

			template<std::enable_if_t<!std::is_void_v<TFull>, int> = 0>
			constexpr TFull operator*() const noexcept
			{
				return GetFull();
			}
	};
}
