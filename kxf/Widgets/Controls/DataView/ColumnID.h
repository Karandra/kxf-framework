#pragma once
#include "Common.h"

namespace kxf::UI::DataView
{
	class ColumnID final
	{
		public:
			using TValue = int32_t;

		private:
			constexpr static TValue GetInvalidValue() noexcept
			{
				return std::numeric_limits<TValue>::min();
			}
			
			template<class T>
			constexpr static bool CheckType()
			{
				return std::is_integral_v<T> || std::is_enum_v<T> || std::is_same_v<T, ColumnID>;
			}
			
			template<class T>
			static void AssertType()
			{
				static_assert(CheckType<T>(), "only integer types or ColumnID are allowed");
			}

		private:
			TValue m_Value;

		public:
			ColumnID(TValue value = GetInvalidValue()) noexcept
				:m_Value(value)
			{
			}
			ColumnID(const ColumnID& other) noexcept
			{
				*this = other;
			}
			ColumnID(ColumnID&& other) noexcept
			{
				*this = std::move(other);
			}
			
			template<class T, typename std::enable_if<CheckType<T>() && !std::is_same_v<std::decay_t<T>, ColumnID>, int>::type = 0>
			ColumnID(const T& value) noexcept
				:m_Value(static_cast<TValue>(value))
			{
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
			
			template<class T = TValue>
			T GetValue() const noexcept
			{
				AssertType<T>();

				return static_cast<T>(m_Value);
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

			bool operator==(const ColumnID& other) const noexcept
			{
				return m_Value == other.m_Value;
			}
			bool operator!=(const ColumnID& other) const noexcept
			{
				return !(*this == other.m_Value);
			}

			template<class T>
			bool operator==(const T& other) const noexcept
			{
				AssertType<T>();
				return m_Value == static_cast<TValue>(other);
			}

			template<class T>
			bool operator!=(const T& other) const noexcept
			{
				return !(*this == other);
			}
			
			ColumnID& operator=(const ColumnID& other) noexcept
			{
				m_Value = other.m_Value;
				return *this;
			}
			ColumnID& operator=(ColumnID&& other) noexcept
			{
				m_Value = other.m_Value;
				other.MakeNull();

				return *this;
			}
	};
}
