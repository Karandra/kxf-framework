#pragma once
#include "Common.h"
#include "StringFormaterTraits.h"
#include "String.h"

namespace KxFramework::StringFormater
{
	class KX_API StringFormaterBase
	{
		private:
			String m_String;
			size_t m_CurrentAnchor = 1;
			bool m_IsUpperCase = false;

		protected:
			const String& GetString() const noexcept
			{
				return m_String;
			}
			String& GetString() noexcept
			{
				return m_String;
			}

			void ReplaceNext(StringView string);
			void ReplaceAnchor(StringView string, size_t index, size_t startAt = 0);
			bool DoReplace(StringView string, StringView index, size_t startAt, size_t& next);

			void FormatString(std::string_view arg, int fieldWidth, wxUniChar fillChar);
			void FormatString(std::wstring_view arg, int fieldWidth, wxUniChar fillChar);
			void FormatChar(wxUniChar arg, int fieldWidth, wxUniChar fillChar);
			void FormatBool(bool arg, int fieldWidth, wxUniChar fillChar);

			void FormatDouble(double arg, int precision, int fieldWidth, wxUniChar format, wxUniChar fillChar);
			void FormatPointer(const void* arg, int fieldWidth, wxUniChar fillChar);

			template<class T>
			static String FormatIntWithBase(T value, int base = 10, bool upper = false)
			{
				static_assert(std::is_integral_v<T>);

				constexpr wxChar digitsL[] = wxS("0123456789abcdefghijklmnopqrstuvwxyz");
				constexpr wxChar digitsU[] = wxS("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
				const wxChar* digits = upper ? digitsU : digitsL;

				// Make positive
				bool isNegative = false;
				if constexpr(std::is_signed_v<T>)
				{
					if (value < 0)
					{
						isNegative = true;
						value = -value;
					}
				}

				String result;
				if (base >= 2 && base <= 36)
				{
					do
					{
						result.Prepend(digits[value % base]);
						value /= base;
					}
					while (value);

					if (isNegative)
					{
						result.Prepend(wxS('-'));
					}
				}
				return result;
			};

		public:
			StringFormaterBase(const String& format)
				:m_String(format)
			{
			}
			StringFormaterBase(String&& format) noexcept
				:m_String(std::move(format))
			{
			}

		protected:
			~StringFormaterBase() noexcept = default;

		public:
			String ToString() const&
			{
				return m_String;
			}
			String ToString() const&&
			{
				return std::move(m_String);
			}
			operator String() const&
			{
				return ToString();
			}
			operator String() const&&
			{
				return ToString();
			}

			size_t GetCurrentAnchor() const noexcept
			{
				return m_CurrentAnchor;
			}

			bool IsUpperCase() const noexcept
			{
				return m_IsUpperCase;
			}
			StringFormaterBase& UpperCase(bool value = true) noexcept
			{
				m_IsUpperCase = value;
				return *this;
			}
			StringFormaterBase& LowerCase(bool value = true) noexcept
			{
				m_IsUpperCase = !value;
				return *this;
			}
	};
}

namespace KxFramework::StringFormater
{
	template<class FmtTraits = DefaultFormatTraits>
	class StringFormater: public StringFormaterBase
	{
		public:
			using FormatTraits = FmtTraits;
			
			template<class T>
			using TypeTraits = TypeTraits<T>;

		protected:
			template<class T>
			void FormatInt(T&& arg, int fieldWidth, int base, wxUniChar fillChar)
			{
				String formatted = FormatIntWithBase(arg, base, IsUpperCase());
				FormatString(StringViewOf(formatted), fieldWidth, fillChar);
			}

		public:
			StringFormater(const String& format)
				:StringFormaterBase(format)
			{
			}
			StringFormater(String&& format) noexcept
				:StringFormaterBase(std::move(format))
			{
			}

		public:
			StringFormater& UpperCase(bool value = true) noexcept
			{
				StringFormaterBase::UpperCase(value);
				return *this;
			}
			StringFormater& LowerCase(bool value = true) noexcept
			{
				StringFormaterBase::LowerCase(value);
				return *this;
			}

		public:
			template<class T>
			typename std::enable_if_t<TypeTraits<T>::FmtString(), StringFormater&> operator()(const T& arg,
																							  int fieldWidth = FormatTraits::StringFiledWidth(),
																							  wxUniChar fillChar = FormatTraits::StringFillChar())
			{
				using Traits = TypeTraits<T>;
				if constexpr(Traits::IsBool())
				{
					FormatBool(arg, fieldWidth, fillChar);
				}
				else if constexpr(Traits::IsConstructibleToWxUniChar())
				{
					FormatChar(arg, fieldWidth, fillChar);
				}
				else if constexpr(Traits::IsConstructibleToWxString() || Traits::IsConstructibleToString())
				{
					FormatString(StringViewOf(arg), fieldWidth, fillChar);
				}
				else if constexpr(Traits::IsStringView() || Traits::IsWStringView())
				{
					FormatString(arg, fieldWidth, fillChar);
				}
				else
				{
					static_assert(false, "StringFormater: unsupported type for string formatting");
				}
				return *this;
			}
			
			template<class T>
			typename std::enable_if_t<TypeTraits<T>::FmtInteger(), StringFormater&> operator()(T arg,
																							   int fieldWidth = FormatTraits::IntFiledWidth(),
																							   int base = FormatTraits::IntBase(),
																							   wxUniChar fillChar = FormatTraits::IntFillChar())
			{
				if constexpr(TypeTraits<T>().IsEnum())
				{
					FormatInt(static_cast<std::underlying_type_t<T>>(arg), fieldWidth, base, fillChar);
				}
				else
				{
					FormatInt(arg, fieldWidth, base, fillChar);
				}
				return *this;
			}

			template<class T>
			typename std::enable_if_t<TypeTraits<T>::FmtPointer(), StringFormater&> operator()(T arg,
																							   int fieldWidth = FormatTraits::PtrFiledWidth(),
																							   wxUniChar fillChar = FormatTraits::PtrFillChar())
			{
				FormatPointer(arg, fieldWidth, fillChar);
				return *this;
			}

			template<class T>
			typename std::enable_if_t<TypeTraits<T>::FmtFloat(), StringFormater&> operator()(T arg,
																							 int precision = FormatTraits::FloatPrecision(),
																							 int fieldWidth = FormatTraits::FloatFiledWidth(),
																							 wxUniChar format = FormatTraits::FloatFormat(),
																							 wxUniChar fillChar = FormatTraits::FloatFillChar())
			{
				FormatDouble(arg, precision, fieldWidth, format, fillChar);
				return *this;
			}
	};
}
