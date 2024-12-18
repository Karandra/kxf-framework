#pragma once
#include "Common.h"

namespace kxf::UI::DataView
{
	class Row final
	{
		public:
			using TValue = size_t;
			using Vector = std::vector<TValue>;

		private:
			constexpr static TValue GetInvalidValue() noexcept
			{
				return std::numeric_limits<TValue>::max();
			}

		private:
			TValue m_Value = GetInvalidValue();

		public:
			Row(TValue value = GetInvalidValue()) noexcept
				:m_Value(value)
			{
			}
			Row(const Row& other) noexcept
				:m_Value(other.m_Value)
			{
			}
			Row(Row&& other) noexcept
				:m_Value(other.m_Value)
			{
				other.MakeNull();
			}

		public:
			bool IsNull() const noexcept
			{
				return m_Value == GetInvalidValue();
			}
			void MakeNull() noexcept
			{
				m_Value = GetInvalidValue();
			}

			TValue GetValue() const noexcept
			{
				return m_Value;
			}
			void SetValue(TValue value) noexcept
			{
				m_Value = value;
			}
			TValue operator*() const noexcept
			{
				return m_Value;
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			bool operator==(const Row& other) const noexcept
			{
				return m_Value == other.m_Value;
			}
			bool operator!=(const Row& other) const noexcept
			{
				return m_Value != other.m_Value;
			}

			bool operator<(const Row& other) const noexcept
			{
				return m_Value < other.m_Value;
			}
			bool operator<=(const Row& other) const noexcept
			{
				return m_Value <= other.m_Value;
			}
			bool operator>(const Row& other) const noexcept
			{
				return m_Value > other.m_Value;
			}
			bool operator>=(const Row& other) const noexcept
			{
				return m_Value >= other.m_Value;
			}

			Row& operator+=(const Row& other) noexcept
			{
				m_Value += other.m_Value;
				return *this;
			}
			Row& operator-=(const Row& other) noexcept
			{
				m_Value -= other.m_Value;
				return *this;
			}
			Row operator+(const Row& other) const noexcept
			{
				return m_Value + other.m_Value;
			}
			Row operator-(const Row& other) const noexcept
			{
				return m_Value - other.m_Value;
			}

			Row& operator++() noexcept
			{
				++m_Value;
				return *this;
			}
			Row& operator--() noexcept
			{
				--m_Value;
				return *this;
			}
			Row operator++(int) noexcept
			{
				size_t oldValue = m_Value + 1;
				++m_Value;
				return oldValue;
			}
			Row operator--(int) noexcept
			{
				size_t oldValue = m_Value - 1;
				--m_Value;
				return oldValue;
			}

			Row& operator=(const Row& other) noexcept
			{
				m_Value = other.m_Value;
				return *this;
			}
			Row& operator=(Row&& other) noexcept
			{
				m_Value = other.m_Value;
				other.MakeNull();
				return *this;
			}
	};
}
