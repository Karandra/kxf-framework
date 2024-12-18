#pragma once
#include "Common.h"

namespace kxf::DataView
{
	class Row final
	{
		private:
			static constexpr auto npos = std::numeric_limits<size_t>::max();

		private:
			size_t m_Value = npos;

		private:
			constexpr bool IsNull() const noexcept
			{
				return m_Value == npos;
			}

		public:
			constexpr Row(size_t value = npos) noexcept
				:m_Value(value)
			{
			}

		public:
			constexpr size_t GetValue() const noexcept
			{
				return m_Value;
			}
			constexpr void SetValue(size_t value) noexcept
			{
				m_Value = value;
			}
			constexpr size_t operator*() const noexcept
			{
				return m_Value;
			}

		public:
			constexpr explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			constexpr bool operator!() const noexcept
			{
				return IsNull();
			}

			constexpr auto operator<=>(const Row&) const noexcept = default;

			constexpr Row& operator+=(const Row& other) noexcept
			{
				m_Value += other.m_Value;
				return *this;
			}
			constexpr Row& operator-=(const Row& other) noexcept
			{
				m_Value -= other.m_Value;
				return *this;
			}
			constexpr Row operator+(const Row& other) const noexcept
			{
				return m_Value + other.m_Value;
			}
			constexpr Row operator-(const Row& other) const noexcept
			{
				return m_Value - other.m_Value;
			}

			constexpr Row& operator++() noexcept
			{
				++m_Value;
				return *this;
			}
			constexpr Row& operator--() noexcept
			{
				--m_Value;
				return *this;
			}
			constexpr Row operator++(int) noexcept
			{
				auto value = m_Value + 1;
				++m_Value;

				return value;
			}
			constexpr Row operator--(int) noexcept
			{
				auto value = m_Value - 1;
				--m_Value;

				return value;
			}
	};
}
