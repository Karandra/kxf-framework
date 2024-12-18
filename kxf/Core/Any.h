#pragma once
#include "Common.h"
#include "String.h"
#include "kxf/Utility/TypeTraits.h"
#include <any>

namespace kxf
{
	class KXF_API Any final
	{
		private:
			std::any m_Any;

		private:
			template<class T>
			static bool ConvertAnyTo(const Any& any, T& value) noexcept
			{
				if constexpr(std::is_same_v<T, String>)
				{
					// Convert to a string
					if (auto charString = any.AsPtr<const char*>())
					{
						value = *charString;
						return true;
					}
					else if (auto wcharString = any.AsPtr<const wchar_t*>())
					{
						value = *wcharString;
						return true;
					}
					else if (auto charPtr = any.AsPtr<char>())
					{
						value = *charPtr;
						return true;
					}
					else if (auto wcharPtr = any.AsPtr<wchar_t>())
					{
						value = *wcharPtr;
						return true;
					}
					else if (auto stringView = any.AsPtr<std::string_view>())
					{
						value = String(*stringView);
						return true;
					}
					else if (auto wstringView = any.AsPtr<std::wstring_view>())
					{
						value = String(*wstringView);
						return true;
					}
					else if (value = any.IntToString(); !value.IsEmpty())
					{
						return true;
					}
					else if (value = any.FloatToString(); !value.IsEmpty())
					{
						return true;
					}
					else if (value = any.BoolToString(); !value.IsEmpty())
					{
						return true;
					}
				}
				else if constexpr(std::is_same_v<T, bool>)
				{
					// Convert to a bool value
					if (auto ptr = any.AsPtr<uint64_t>())
					{
						value = *ptr != 0;
						return true;
					}
					else if (auto ptr = any.AsPtr<double>())
					{
						value = *ptr != 0;
						return true;
					}
					else if (auto opt = any.StringToBool())
					{
						value = *opt;
						return true;
					}
				}
				else if constexpr(std::is_integral_v<T> || std::is_enum_v<T>)
				{
					using TIntType = Utility::UnderlyingTypeEx_t<T>;

					// Convert to an integer
					if constexpr(std::is_signed_v<TIntType>)
					{
						// Convert to signed int
						if (const String* ptr = any.AsPtr<String>())
						{
							if (auto iValue = ptr->ToInteger<int64_t>())
							{
								value = static_cast<T>(*iValue);
								return true;
							}
						}
					}
					else if constexpr(std::is_unsigned_v<TIntType>)
					{
						// Convert to unsigned int
						if (const String* ptr = any.AsPtr<String>())
						{
							if (auto iValue = ptr->ToInteger<uint64_t>())
							{
								value = static_cast<T>(*iValue);
								return true;
							}
						}
					}

					if (auto ptr = any.AsPtr<double>())
					{
						value = static_cast<T>(*ptr);
						return true;
					}
					else if (const bool* ptr = any.AsPtr<bool>())
					{
						value = static_cast<T>(*ptr ? 1 : 0);
						return true;
					}
				}
				else if constexpr(std::is_floating_point_v<T>)
				{
					// Convert to float or double
					if (const String* ptr = any.AsPtr<String>())
					{
						if (auto fValue = ptr->ToFloatingPoint<double>())
						{
							value = *fValue;
							return true;
						}
					}
					else if (auto ptr = any.AsPtr<int64_t>())
					{
						value = *ptr;
						return true;
					}
					else if (const bool* ptr = any.AsPtr<bool>())
					{
						value = *ptr ? 1 : 0;
						return true;
					}
				}
				return false;
			}

			template<class TFunc>
			bool ResetOnException(TFunc&& func) noexcept
			{
				try
				{
					std::invoke(func);
					return true;
				}
				catch (...)
				{
					MakeNull();
					return false;
				}
			}

			template<class T>
			T* AsPtr() noexcept
			{
				return std::any_cast<T>(&m_Any);
			}

			template<class T>
			const T* AsPtr() const noexcept
			{
				return std::any_cast<T>(&m_Any);
			}

			String IntToString() const noexcept;
			String FloatToString() const noexcept;
			String BoolToString() const noexcept;
			std::optional<bool> StringToBool() const noexcept;

		public:
			Any() = default;
			Any(const Any& other) noexcept
			{
				Assign(other);
			}
			Any(Any&&) noexcept = default;
			explicit Any(const std::any& other) noexcept
			{
				Assign(other);
			}
			explicit Any(std::any&& other) noexcept
				:m_Any(std::move(other))
			{
			}

			template<class T>
			Any(T&& value) noexcept
			{
				Assign(std::forward<T>(value));
			}

		public:
			bool IsNull() const noexcept
			{
				return !m_Any.has_value();
			}
			bool HasValue() const noexcept
			{
				return m_Any.has_value();
			}
			void MakeNull() noexcept
			{
				m_Any.reset();
			}

			void Swap(Any& other) noexcept
			{
				m_Any.swap(other.m_Any);
			}
			void Swap(std::any& other) noexcept
			{
				m_Any.swap(other);
			}

			const std::type_info& GetTypeInfo() const noexcept
			{
				return m_Any.type();
			}
			bool HasSameType(const Any& other) const noexcept
			{
				return GetTypeInfo() == other.GetTypeInfo();
			}

			template<class T>
			bool CheckType() const noexcept
			{
				static_assert(!std::is_void_v<T>, "T must not be void");

				if constexpr(std::is_reference_v<T>)
				{
					return GetTypeInfo() == typeid(std::remove_reference_t<T>*);
				}
				else
				{
					return GetTypeInfo() == typeid(T);
				}
			}

			// Getters
			const std::any& GetAny() const noexcept
			{
				return m_Any;
			}
			std::any& GetAny() noexcept
			{
				return m_Any;
			}

			// Retrieve the stored value, throws 'std::bad_any_cast' on type mismatch
			template<class T>
			T As() const&
			{
				return std::any_cast<T>(m_Any);
			}

			template<class T>
			T As() &&
			{
				return std::any_cast<T>(std::move(m_Any));
			}

			// Try to retrieve the stored value, on type mismatch the empty optional object is returned
			template<class T>
			std::optional<T> QueryAs() const& noexcept(std::is_nothrow_copy_constructible_v<T>)
			{
				if (auto ptr = AsPtr<T>())
				{
					return *ptr;
				}
				return {};
			}

			template<class T>
			std::optional<T> QueryAs() && noexcept(std::is_nothrow_move_constructible_v<T>)
			{
				if (auto ptr = AsPtr<T>())
				{
					return std::move(*ptr);
				}
				return {};
			}

			// Try to retrieve the stored value or convert it to desired type if possible
			template<class T>
			bool GetAs(T& value) const& noexcept(std::is_nothrow_copy_assignable_v<T>)
			{
				if (const T* ptr = AsPtr<T>())
				{
					value = *ptr;
					return true;
				}
				return ConvertAnyTo(*this, value);
			}

			template<class T>
			bool GetAs(T& value) && noexcept(std::is_nothrow_move_assignable_v<T>)
			{
				if (T* ptr = AsPtr<T>())
				{
					value = std::move(*ptr);
					return true;
				}
				return ConvertAnyTo(*this, value);
			}

			// Overload of 'GetAs' for default-constructible types
			template<class T, class = std::enable_if_t<std::is_default_constructible_v<T>>>
			T GetAs() const noexcept(std::is_nothrow_default_constructible_v<T>)
			{
				T value;
				GetAs(value);
				return value;
			}

			template<class T, class = std::enable_if_t<std::is_default_constructible_v<T>>>
			T GetAs() noexcept(std::is_nothrow_default_constructible_v<T>)
			{
				T value;
				GetAs(value);
				return value;
			}

			// Assignment
			template<class T>
			Any& Assign(T&& value) noexcept
			{
				static_assert(std::is_copy_constructible_v<std::decay_t<T>>, "copy constructible type is required");

				ResetOnException([&]()
				{
					if constexpr(std::is_enum_v<T>)
					{
						m_Any = static_cast<std::underlying_type_t<T>>(value);
					}
					else
					{
						m_Any = std::forward<T>(value);
					}
				});
				return *this;
			}

			Any& Assign(Any&& other) noexcept
			{
				m_Any = std::move(other.m_Any);
				return *this;
			}
			Any& Assign(std::any&& other) noexcept
			{
				m_Any = std::move(other);
				return *this;
			}
			Any& Assign(const Any& other) noexcept
			{
				ResetOnException([&]()
				{
					m_Any = other.m_Any;
				});
				return *this;
			}
			Any& Assign(const std::any& other) noexcept
			{
				ResetOnException([&]()
				{
					m_Any = other;
				});
				return *this;
			}

		public:
			explicit operator bool() const noexcept
			{
				return HasValue();
			}
			bool operator!() noexcept
			{
				return IsNull();
			}

			Any& operator=(const Any& other) noexcept
			{
				return Assign(other);
			}
			Any& operator=(Any&&) noexcept = default;

			template<class T>
			Any& operator=(T&& value) noexcept
			{
				return Assign(std::forward<T>(value));
			}
	};
}

namespace std
{
	inline void swap(kxf::Any& left, kxf::Any& right) noexcept
	{
		left.Swap(right);
	}
	inline void swap(kxf::Any& left, std::any& right) noexcept
	{
		left.Swap(right);
	}
	inline void swap(std::any& left, kxf::Any& right) noexcept
	{
		right.Swap(left);
	}
}
