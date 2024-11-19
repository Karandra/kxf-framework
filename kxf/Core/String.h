#pragma once
#include "Common.h"
#include "UniChar.h"
#include "kxf/System/UndefWindows.h"
#include "kxf/Serialization/BinarySerializer.h"
#include <format>
#include <string>
#include <string_view>
class wxString;

namespace kxf
{
	class IEncodingConverter;

	using XChar = wchar_t;
	using StringView = std::basic_string_view<XChar>;
	KX_API extern const String NullString;

	#define kxS(x)	L ## x

	enum class StringActionFlag: uint32_t
	{
		None = 0,
		IgnoreCase = 1 << 0,
		Symmetrical = 1 << 1,
		FirstMatchOnly = 1 << 2,
	};
	KxFlagSet_Declare(StringActionFlag);
}

namespace kxf
{
	std::basic_string_view<XChar> StringViewOf(const String& string) noexcept;
	std::basic_string_view<XChar> StringViewOf(const wxString& string) noexcept;

	template<class T>
	constexpr std::basic_string_view<T> StringViewOf(const std::basic_string<T>& string) noexcept
	{
		return {string.data(), string.length()};
	}

	template<class T>
	constexpr std::basic_string_view<T> StringViewOf(std::basic_string_view<T> view) noexcept
	{
		return view;
	}

	template<class T> requires(std::is_same_v<T, char> || std::is_same_v<T, wchar_t>)
	constexpr std::basic_string_view<T> StringViewOf(const T* ptr) noexcept
	{
		if (ptr)
		{
			return ptr;
		}
		return {};
	}

	template<class T> requires(std::is_bounded_array_v<T>)
	constexpr auto StringViewOf(const T& buffer) noexcept
	{
		using Tx = std::remove_pointer_t<std::decay_t<T>>;

		auto size = std::size(buffer);
		return std::basic_string_view<Tx>(std::data(buffer), size != 0 ? size - 1 : size);
	}

	constexpr inline UniChar UniCharOf(char c) noexcept
	{
		return c;
	}
	constexpr inline UniChar UniCharOf(wchar_t c) noexcept
	{
		return c;
	}
	constexpr inline UniChar UniCharOf(UniChar c) noexcept
	{
		return c;
	}
}

namespace kxf
{
	class KX_API String final
	{
		friend struct std::hash<String>;
		friend struct BinarySerializer<String>;

		public:
			using value_type = XChar;
			using string_type = std::basic_string<XChar>;
			using traits_type = std::char_traits<XChar>;
			using allocator_type = std::allocator<XChar>;
			
			using iterator = string_type::iterator;
			using const_iterator = string_type::const_iterator;
			using reverse_iterator = string_type::reverse_iterator;
			using const_reverse_iterator = string_type::const_reverse_iterator;

		public:
			static constexpr size_t npos = StringView::npos;

		private:
			// Utility
			template<class T>
			static constexpr bool IsAnyCharType() noexcept
			{
				using Tx = std::remove_const_t<std::remove_reference_t<T>>;
				return !std::is_array_v<T> &&
					!std::is_pointer_v<T> &&
					(std::is_same_v<Tx, char> || std::is_same_v<Tx, wchar_t> || std::is_same_v<Tx, UniChar>);
			}

			template<class T>
			static constexpr bool IsAnyStringType() noexcept
			{
				return !IsAnyCharType<T>();
			}

			template<class TChar>
			static size_t CalcStringLength(const TChar* data, size_t length) noexcept
			{
				if (data)
				{
					if (length == npos)
					{
						return std::char_traits<TChar>::length(data);
					}
					return length;
				}
				return 0;
			}

		private:
			// Comparison
			static std::strong_ordering DoCompare(std::string_view left, std::string_view right, FlagSet<StringActionFlag> flags = {}) noexcept;
			static std::strong_ordering DoCompare(std::wstring_view left, std::wstring_view right, FlagSet<StringActionFlag> flags = {}) noexcept;
			static std::strong_ordering DoCompare(UniChar left, UniChar right, FlagSet<StringActionFlag> flags = {}) noexcept;
			
		public:
			template<class T1, class T2>
			static std::strong_ordering Compare(T1&& left, T2&& right, FlagSet<StringActionFlag> flags = {}) noexcept
			{
				if constexpr(IsAnyCharType<T1>() && IsAnyCharType<T2>())
				{
					return DoCompare(UniCharOf(std::forward<T1>(left)), UniCharOf(std::forward<T2>(right)), flags);
				}
				else
				{
					return DoCompare(StringViewOf(std::forward<T1>(left)), StringViewOf(std::forward<T2>(right)), flags);
				}
			}

		private:
			static bool DoMatchesWildcards(std::string_view name, std::string_view expression, FlagSet<StringActionFlag> flags = {}) noexcept;
			static bool DoMatchesWildcards(std::wstring_view name, std::wstring_view expression, FlagSet<StringActionFlag> flags = {}) noexcept;

		public:
			template<class T1, class T2>
			static int MatchesWildcards(T1&& name, T2&& expression, FlagSet<StringActionFlag> flags = {}) noexcept
			{
				return DoMatchesWildcards(StringViewOf(std::forward<T1>(name)), StringViewOf(std::forward<T2>(expression)), flags);
			}

			// Conversions
			static UniChar FromUTF8(char c);
			static UniChar FromUTF8(char8_t c)
			{
				return FromUTF8(static_cast<char>(c));
			}
			static String FromUTF8(const char* utf8, size_t length = npos);
			static String FromUTF8(const char8_t* utf8, size_t length = npos)
			{
				return FromUTF8(reinterpret_cast<const char*>(utf8), length);
			}
			static String FromUTF8(const std::string& utf8)
			{
				return FromUTF8(utf8.data(), utf8.length());
			}
			static String FromUTF8(std::string_view utf8)
			{
				return FromUTF8(utf8.data(), utf8.length());
			}
			static std::string ToUTF8(std::wstring_view utf16);

			static String FromASCII(const char* ascii, size_t length = npos);
			static String FromASCII(std::string_view ascii);
			static String FromLocalEncoding(std::string_view ascii);
			static String FromEncoding(std::string_view source, IEncodingConverter& encodingConverter);

			static String FromFloatingPoint(double value, int precision = -1);

			// Substring extraction
			template<class TFunc>
			static size_t SplitBySeparator(const String& string, const String& sep, TFunc&& func, FlagSet<StringActionFlag> flags = {})
			{
				const StringView view = string.xc_view();

				if (sep.empty() && !string.empty())
				{
					std::invoke(func, view);
					return 1;
				}

				size_t separatorPos = string.Find(sep, flags);
				if (separatorPos == String::npos)
				{
					std::invoke(func, view);
					return 1;
				}

				size_t pos = 0;
				size_t count = 0;
				while (pos < string.length() && separatorPos <= string.length())
				{
					StringView stringPiece = view.substr(pos, separatorPos - pos);
					const size_t stringPieceLength = stringPiece.length();

					if (!stringPiece.empty())
					{
						count++;
						if (!std::invoke(func, std::move(stringPiece)))
						{
							return count;
						}
					}

					pos += stringPieceLength + sep.length();
					separatorPos = string.Find(sep, flags, pos);

					// No separator found, but this is not the last element
					if (separatorPos == String::npos && pos < string.length())
					{
						separatorPos = string.length();
					}
				}
				return count;
			}

			template<class TFunc>
			static size_t SplitByLength(const String& string, size_t length, TFunc&& func)
			{
				if (length != 0)
				{
					const StringView view = string.xc_view();

					size_t count = 0;
					for (size_t i = 0; i < view.length(); i += length)
					{
						StringView stringPiece = view.substr(i, length);
						if (!stringPiece.empty())
						{
							count++;
							if (!std::invoke(func, std::move(stringPiece)))
							{
								return count;
							}
						}
					}
					return count;
				}
				else
				{
					std::invoke(func, string.xc_view());
					return 1;
				}
				return 0;
			}

			// Concatenation
			template<class... Args>
			static String Concat(Args&&... arg)
			{
				return (String(std::forward<Args>(arg)) + ...);
			}

			template<class... Args>
			static String ConcatWithSeparator(const String& sep, Args&&... arg)
			{
				String value = ((String(std::forward<Args>(arg)) + sep) + ...);
				value.RemoveRight(sep.length());
				return value;
			}

		private:
			string_type m_String;
			mutable std::unique_ptr<std::string> m_Converted;

		public:
			String() = default;
			String(const String& other)
				:m_String(other.m_String)
			{
			}
			String(String&&) noexcept = default;

			String(const wxString& other) noexcept;
			String(wxString&& other) noexcept;

			// Char/wchar_t pointers
			String(const char* data, size_t length = npos);
			String(const char8_t* data, size_t length = npos)
				:String(FromUTF8(data, CalcStringLength(data, length)))
			{
			}
			String(const wchar_t* data, size_t length = npos)
				:m_String(data, CalcStringLength(data, length))
			{
			}
			
			// std::[w]string[_view]
			String(const std::string& other);
			String(const std::wstring& other)
				:m_String(other)
			{
			}
			String(std::basic_string<XChar>&& other)
				:m_String(std::move(other))
			{
			}

			String(std::string_view other);
			String(std::wstring_view other)
				:m_String(other.data(), other.length())
			{
			}

			// Single char
			String(char c, size_t count = 1)
				:m_String(count, static_cast<XChar>(c))
			{
			}
			String(wchar_t c, size_t count = 1)
				:m_String(count, static_cast<XChar>(c))
			{
			}
			String(UniChar c, size_t count = 1)
				:m_String(count, static_cast<XChar>(*c))
			{
			}
			
			~String() = default;

		public:
			// String length
			bool IsEmpty() const noexcept
			{
				return m_String.empty();
			}
			bool IsEmptyOrWhitespace() const noexcept;
			size_t GetLength() const noexcept
			{
				return m_String.length();
			}
			size_t GetCapacity() const noexcept
			{
				return m_String.capacity();
			}

			// Character access
			const string_type& impl_str() const noexcept
			{
				return m_String;
			}
			string_type& impl_str() noexcept
			{
				return m_String;
			}

			wchar_t* data() noexcept
			{
				return m_String.data();
			}
			const wchar_t* data() const noexcept
			{
				return m_String.data();
			}

			std::basic_string_view<char> nc_view() const
			{
				m_Converted = std::make_unique<std::string>(ToLocalEncoding());
				return *m_Converted;
			}
			std::basic_string_view<char> utf8_view() const
			{
				m_Converted = std::make_unique<std::string>(ToUTF8());
				return *m_Converted;
			}
			std::basic_string_view<wchar_t> wc_view() const noexcept(std::is_same_v<XChar, wchar_t>)
			{
				return StringViewOf(m_String);
			}
			std::basic_string_view<XChar> xc_view() const noexcept(std::is_same_v<XChar, wchar_t>)
			{
				return StringViewOf(m_String);
			}

			const char* nc_str() const
			{
				return nc_view().data();
			}
			const char* utf8_str() const
			{
				return utf8_view().data();
			}
			const wchar_t* wc_str() const noexcept(std::is_same_v<XChar, wchar_t>)
			{
				return m_String.c_str();
			}
			const XChar* xc_str() const noexcept(std::is_same_v<XChar, wchar_t>)
			{
				return m_String.c_str();
			}

			XChar& operator[](size_t i) noexcept
			{
				return m_String[i];
			}
			const XChar& operator[](size_t i) const noexcept
			{
				return m_String[i];
			}

			// Conversions
			std::string ToUTF8() const
			{
				return ToUTF8(StringViewOf(m_String));
			}
			std::string ToASCII(char replaceWith = '_') const;
			std::string ToLocalEncoding() const;
			std::string ToEncoding(IEncodingConverter& encodingConverter) const;

			// Concatenation and formatting
		private:
			String& DoAppend(std::string_view other);
			String& DoAppend(std::wstring_view other)
			{
				m_String.append(other);
				return *this;
			}
			String& DoAppend(UniChar c, size_t count = 1)
			{
				m_String.append(count, *c);
				return *this;
			}

			template<class OutputIt, class... Args>
			OutputIt FormatTo(OutputIt outputIt, std::string_view format, Args&&... arg);

			template<class OutputIt, class... Args>
			OutputIt FormatTo(OutputIt outputIt, std::wstring_view format, Args&&... arg);

		public:
			template<class T> requires(IsAnyStringType<T>())
			String& Append(T&& other)
			{
				return DoAppend(StringViewOf(std::forward<T>(other)));
			}

			template<class T> requires(IsAnyCharType<T>())
			String& Append(T&& other, size_t count = 1)
			{
				return DoAppend(UniCharOf(std::forward<T>(other)), count);
			}

			template<class T>
			String& operator+=(T&& other)
			{
				return Append(std::forward<T>(other));
			}

			template<class TFormat, class... Args>
			String& Format(const TFormat& format, Args&&... arg);

			template<class TFormat, class... Args>
			String& FormatAt(size_t position, const TFormat& format, Args&&... arg);

		private:
			String& DoPrepend(std::string_view other);
			String& DoPrepend(std::wstring_view other)
			{
				m_String.insert(0, other.data(), other.length());
				return *this;
			}
			String& DoPrepend(UniChar c, size_t count = 1)
			{
				m_String.insert(0, count, *c);
				return *this;
			}
			
		public:
			template<class T> requires(IsAnyStringType<T>())
			String& Prepend(T&& other)
			{
				return DoPrepend(StringViewOf(std::forward<T>(other)));
			}

			template<class T> requires(IsAnyCharType<T>())
			String& Prepend(T&& other, size_t count = 1)
			{
				return DoPrepend(UniCharOf(std::forward<T>(other)), count);
			}

		private:
			String& DoInsert(size_t pos, std::string_view other);
			String& DoInsert(size_t pos, std::wstring_view other)
			{
				m_String.insert(pos, other);
				return *this;
			}
			String& DoInsert(size_t pos, UniChar c, size_t count = 1)
			{
				m_String.insert(pos, count, *c);
				return *this;
			}
			
		public:
			template<class T> requires(IsAnyStringType<T>())
			String& Insert(size_t pos, T&& other)
			{
				return DoInsert(pos, StringViewOf(std::forward<T>(other)));
			}

			template<class T> requires(IsAnyCharType<T>())
			String& Insert(size_t pos, T&& other, size_t count = 1)
			{
				return DoInsert(pos, UniCharOf(std::forward<T>(other)), count);
			}

		private:
			// Comparison
			std::strong_ordering DoCompareTo(std::string_view other, FlagSet<StringActionFlag> flags = {}) const noexcept(std::is_same_v<XChar, char>)
			{
				return Compare(*this, String(other), flags);
			}
			std::strong_ordering DoCompareTo(std::wstring_view other, FlagSet<StringActionFlag> flags = {}) const noexcept(std::is_same_v<XChar, wchar_t>)
			{
				return Compare(wc_view(), other, flags);
			}
			std::strong_ordering DoCompareTo(UniChar other, FlagSet<StringActionFlag> flags = {}) const noexcept
			{
				const XChar c[2] = {other.GetAs<XChar>(), 0};
				return Compare(wc_view(), StringViewOf(c), flags);
			}

		public:
			template<class T>
			std::strong_ordering CompareTo(T&& other, FlagSet<StringActionFlag> flags = {}) const
			{
				if constexpr(IsAnyCharType<T>())
				{
					return DoCompareTo(UniCharOf(std::forward<T>(other)), flags);
				}
				else
				{
					return DoCompareTo(StringViewOf(std::forward<T>(other)), flags);
				}
			}

			template<class T>
			bool IsSameAs(T&& other, FlagSet<StringActionFlag> flags = {}) const
			{
				return CompareTo(std::forward<T>(other), flags) == 0;
			}

		private:
			bool DoStartsWith(std::string_view pattern, String* rest = nullptr, FlagSet<StringActionFlag> flags = {}) const;
			bool DoStartsWith(std::wstring_view pattern, String* rest = nullptr, FlagSet<StringActionFlag> flags = {}) const;
			bool DoStartsWith(UniChar c, String* rest = nullptr, FlagSet<StringActionFlag> flags = {}) const noexcept
			{
				const XChar pattern[2] = {c.GetAs<XChar>(), 0};
				return StartsWith(StringViewOf(pattern), rest, flags);
			}

		public:
			template<class T>
			bool StartsWith(T&& pattern, String* rest = nullptr, FlagSet<StringActionFlag> flags = {}) const
			{
				if constexpr(IsAnyCharType<T>())
				{
					return DoStartsWith(UniCharOf(std::forward<T>(pattern)), rest, flags);
				}
				else
				{
					return DoStartsWith(StringViewOf(std::forward<T>(pattern)), rest, flags);
				}
			}

		private:
			bool DoEndsWith(std::string_view pattern, String* rest = nullptr, FlagSet<StringActionFlag> flags = {}) const;
			bool DoEndsWith(std::wstring_view pattern, String* rest = nullptr, FlagSet<StringActionFlag> flags = {}) const;
			bool DoEndsWith(UniChar c, String* rest = nullptr, FlagSet<StringActionFlag> flags = {}) const noexcept
			{
				const XChar pattern[2] = {c.GetAs<XChar>(), 0};
				return EndsWith(StringViewOf(pattern), rest, flags);
			}

		public:
			template<class T>
			bool EndsWith(T&& pattern, String* rest = nullptr, FlagSet<StringActionFlag> flags = {}) const
			{
				if constexpr(IsAnyCharType<T>())
				{
					return DoEndsWith(UniCharOf(std::forward<T>(pattern)), rest, flags);
				}
				else
				{
					return DoEndsWith(StringViewOf(std::forward<T>(pattern)), rest, flags);
				}
			}

		private:
			bool DoMatchesWildcards(std::string_view expression, FlagSet<StringActionFlag> flags = {}) const noexcept
			{
				auto converted = FromUTF8(expression);
				return DoMatchesWildcards(wc_view(), StringViewOf(converted), flags);
			}
			bool DoMatchesWildcards(std::wstring_view expression, FlagSet<StringActionFlag> flags = {}) const noexcept
			{
				return DoMatchesWildcards(wc_view(), expression, flags);
			}
			bool DoMatchesWildcards(UniChar c, FlagSet<StringActionFlag> flags = {}) const noexcept
			{
				const XChar expression[2] = {c.GetAs<XChar>(), 0};
				return DoMatchesWildcards(wc_view(), StringViewOf(expression), flags);
			}

		public:
			template<class T>
			bool MatchesWildcards(T&& expression, FlagSet<StringActionFlag> flags = {}) const
			{
				if constexpr(IsAnyCharType<T>())
				{
					return DoMatchesWildcards(UniCharOf(std::forward<T>(expression)), flags);
				}
				else
				{
					return DoMatchesWildcards(StringViewOf(std::forward<T>(expression)), flags);
				}
			}

			// Substring extraction
			String SubMid(size_t offset, size_t count = String::npos) const
			{
				if (offset < m_String.length())
				{
					return m_String.substr(offset, count);
				}
				return {};
			}
			String SubLeft(size_t count) const
			{
				return m_String.substr(0, count);
			}
			String SubRight(size_t count) const
			{
				size_t offset = m_String.length() - count;
				if (offset < m_String.length())
				{
					return m_String.substr(offset, count);
				}
				return {};
			}
			String SubRange(size_t from, size_t to) const
			{
				size_t length = m_String.length();
				if (from < to && from < length && to < length)
				{
					return m_String.substr(from, to - from + 1);
				}
				return {};
			}

			String AfterFirst(UniChar c, String* rest = nullptr, FlagSet<StringActionFlag> flags = {}) const;
			String AfterLast(UniChar c, String* rest = nullptr, FlagSet<StringActionFlag> flags = {}) const;

			String BeforeFirst(UniChar c, String* rest = nullptr, FlagSet<StringActionFlag> flags = {}) const;
			String BeforeLast(UniChar c, String* rest = nullptr, FlagSet<StringActionFlag> flags = {}) const;

			template<class TFunc>
			size_t SplitBySeparator(const String& sep, TFunc&& func, FlagSet<StringActionFlag> flags = {}) const
			{
				return SplitBySeparator(*this, sep, std::forward<TFunc>(func), flags);
			}

			template<class TFunc>
			size_t SplitByLength(size_t length, TFunc&& func) const
			{
				return SplitByLength(wc_view(), length, std::forward<TFunc>(func));
			}

			// Case conversion
			String& MakeLower() noexcept;
			String& MakeUpper() noexcept;
			String ToLower() const
			{
				return Clone().MakeLower();
			}
			String ToUpper() const
			{
				return Clone().MakeUpper();
			}

			String& MakeCapitalized() noexcept
			{
				if (!m_String.empty())
				{
					m_String[0] = *UniChar(m_String[0]).ToUpperCase();
				}
				return *this;
			}
			String Capitalize() const
			{
				return Clone().MakeCapitalized();
			}

			// Searching and replacing
		private:
			size_t DoFind(std::string_view pattern, size_t offset, FlagSet<StringActionFlag> flags, bool reverse) const;
			size_t DoFind(std::wstring_view pattern, size_t offset, FlagSet<StringActionFlag> flags, bool reverse) const;
			size_t DoFind(UniChar pattern, size_t offset, FlagSet<StringActionFlag> flags, bool reverse) const noexcept;
			
		public:
			template<class T>
			size_t Find(T&& pattern, FlagSet<StringActionFlag> flags = {}, size_t offset = 0) const
			{
				if constexpr(IsAnyCharType<T>())
				{
					return DoFind(UniCharOf(std::forward<T>(pattern)), offset, flags, false);
				}
				else
				{
					return DoFind(StringViewOf(std::forward<T>(pattern)), offset, flags, false);
				}
			}

			template<class T>
			size_t ReverseFind(T&& pattern, FlagSet<StringActionFlag> flags = {}, size_t offset = npos) const
			{
				if constexpr(IsAnyCharType<T>())
				{
					return DoFind(UniCharOf(std::forward<T>(pattern)), offset, flags, true);
				}
				else
				{
					return DoFind(StringViewOf(std::forward<T>(pattern)), offset, flags, true);
				}
			}

		private:
			size_t DoReplace(std::string_view pattern, std::string_view replacement, size_t offset, FlagSet<StringActionFlag> flags, bool reverse = false);
			size_t DoReplace(std::string_view pattern, std::wstring_view replacement, size_t offset, FlagSet<StringActionFlag> flags, bool reverse = false);
			size_t DoReplace(std::wstring_view pattern, std::string_view replacement, size_t offset, FlagSet<StringActionFlag> flags, bool reverse = false);
			size_t DoReplace(std::wstring_view pattern, std::wstring_view replacement, size_t offset, FlagSet<StringActionFlag> flags, bool reverse = false);
			size_t DoReplace(UniChar c, UniChar replacement, size_t offset, FlagSet<StringActionFlag> flags, bool reverse = false) noexcept;
			size_t DoReplace(UniChar c, std::string_view replacement, size_t offset, FlagSet<StringActionFlag> flags, bool reverse = false) noexcept
			{
				const char pattern[2] = {c.GetAs<char>(), 0};
				return Replace(StringViewOf(pattern), replacement, offset, flags);
			}
			size_t DoReplace(UniChar c, std::wstring_view replacement, size_t offset, FlagSet<StringActionFlag> flags, bool reverse = false) noexcept
			{
				const wchar_t pattern[2] = {c.GetAs<XChar>(), 0};
				return Replace(StringViewOf(pattern), replacement, offset, flags);
			}
			
		public:
			template<class T1, class T2>
			size_t Replace(T1&& pattern, T2&& replacement, size_t offset = 0, FlagSet<StringActionFlag> flags = {})
			{
				if constexpr(IsAnyStringType<T1>() && IsAnyStringType<T2>())
				{
					return DoReplace(StringViewOf(std::forward<T1>(pattern)), StringViewOf(std::forward<T2>(replacement)), offset, flags);
				}
				else if constexpr(IsAnyCharType<T1>() && IsAnyCharType<T2>())
				{
					return DoReplace(UniCharOf(std::forward<T1>(pattern)), UniCharOf(std::forward<T2>(replacement)), offset, flags);
				}
				else if constexpr(IsAnyCharType<T1>() && IsAnyStringType<T2>())
				{
					return DoReplace(UniCharOf(std::forward<T1>(pattern)), StringViewOf(std::forward<T2>(replacement)), offset, flags);
				}
				else
				{
					static_assert(sizeof(T1*) == 0, "invalid argument types");
				}
			}

			String& ReplaceRange(size_t offset, size_t length, const String& replacement)
			{
				m_String.replace(offset, length, replacement.wc_view());
				return *this;
			}
			String& ReplaceRange(size_t offset, size_t length, std::string_view replacement);
			String& ReplaceRange(size_t offset, size_t length, std::wstring_view replacement)
			{
				m_String.replace(offset, length, replacement);
				return *this;
			}

			String& ReplaceRange(iterator first, iterator last, const String& replacement)
			{
				m_String.replace(first, last, replacement.wc_view());
				return *this;
			}
			String& ReplaceRange(iterator first, iterator last, std::string_view replacement);
			String& ReplaceRange(iterator first, iterator last, std::wstring_view replacement)
			{
				m_String.replace(first, last, replacement);
				return *this;
			}

			template<class T>
			bool Contains(T&& pattern, FlagSet<StringActionFlag> flags = {}) const
			{
				return Find(std::forward<T>(pattern), flags) != npos;
			}

		private:
			bool DoContainsAnyOfCharacters(std::string_view pattern, FlagSet<StringActionFlag> flags = {}) const noexcept;
			bool DoContainsAnyOfCharacters(std::wstring_view pattern, FlagSet<StringActionFlag> flags = {}) const noexcept;

		public:
			template<class T>
			bool ContainsAnyOfCharacters(T&& pattern, FlagSet<StringActionFlag> flags = {}) const noexcept
			{
				return DoContainsAnyOfCharacters(StringViewOf(std::forward<T>(pattern)), flags);
			}

		private:
			// Conversion to numbers
			bool DoToFloatingPoint(float& value) const noexcept;
			bool DoToFloatingPoint(double& value) const noexcept;
			bool DoToSignedInteger(int64_t& value, int base) const noexcept;
			bool DoToUnsignedInteger(uint64_t& value, int base) const noexcept;

			// Misc
			size_t TrimScan(const String& chars, FlagSet<StringActionFlag> flags, bool left) const;

		public:
			template<class T = double> requires(std::is_floating_point_v<T>)
			std::optional<T> ToFloatingPoint() const noexcept
			{
				if constexpr(std::is_same_v<T, float>)
				{
					float value = 0;
					if (DoToFloatingPoint(value))
					{
						return static_cast<T>(value);
					}
				}
				else
				{
					double value = 0;
					if (DoToFloatingPoint(value))
					{
						return static_cast<T>(value);
					}
				}
				return {};
			}

			template<class T = int> requires(std::is_integral_v<T>)
			std::optional<T> ToInteger(int base = 10) const noexcept
			{
				using Limits = std::numeric_limits<T>;
				using TInt = std::conditional_t<std::is_unsigned_v<T>, uint64_t, int64_t>;

				TInt value = 0;
				bool isSuccess = false;
				if constexpr(std::is_unsigned_v<T>)
				{
					isSuccess = DoToUnsignedInteger(value, base);
				}
				else
				{
					isSuccess = DoToSignedInteger(value, base);
				}

				if (isSuccess && value == std::clamp<TInt>(value, Limits::min(), Limits::max()))
				{
					return static_cast<T>(value);
				}
				return {};
			}

			// Misc
			bool IsASCII() const noexcept
			{
				for (const auto& c: m_String)
				{
					if (!UniChar(c).IsASCII())
					{
						return false;
					}
				}
				return true;
			}
			String& Remove(size_t offset, size_t count)
			{
				if (count != 0 && offset < m_String.length())
				{
					m_String.erase(offset, count);
				}
				return *this;
			}
			String& RemoveRight(size_t count)
			{
				size_t offset = m_String.length() - count;
				if (count != 0 && offset < m_String.length())
				{
					m_String.erase(offset, count);
				}
				return *this;
			}
			String& Truncate(size_t length)
			{
				if (length < m_String.length())
				{
					m_String.resize(length);
				}
				return *this;
			}

			size_t TrimScanLeft(const String& chars = {}, FlagSet<StringActionFlag> flags = {}) const
			{
				return TrimScan(chars, flags, true);
			}
			size_t TrimScanRight(const String& chars = {}, FlagSet<StringActionFlag> flags = {}) const
			{
				return TrimScan(chars, flags, false);
			}
			String& TrimLeft(const String& chars = {}, FlagSet<StringActionFlag> flags = {});
			String& TrimRight(const String& chars = {}, FlagSet<StringActionFlag> flags = {});
			String& TrimBoth(const String& chars = {}, FlagSet<StringActionFlag> flags = {});

			String Clone() const
			{
				return m_String;
			}

			// Iterator interface
			iterator begin() noexcept
			{
				return m_String.begin();
			}
			iterator end() noexcept
			{
				return m_String.end();
			}
			const_iterator begin() const noexcept
			{
				return m_String.begin();
			}
			const_iterator end() const noexcept
			{
				return m_String.end();
			}
			const_iterator cbegin() const noexcept
			{
				return m_String.cbegin();
			}
			const_iterator cend() const noexcept
			{
				return m_String.cend();
			}
			
			reverse_iterator rbegin() noexcept
			{
				return m_String.rbegin();
			}
			reverse_iterator rend() noexcept
			{
				return m_String.rend();
			}
			const_reverse_iterator rbegin() const noexcept
			{
				return m_String.rbegin();
			}
			const_reverse_iterator rend() const noexcept
			{
				return m_String.rend();
			}
			const_reverse_iterator crend() const noexcept
			{
				return m_String.crend();
			}
			const_reverse_iterator crbegin() const noexcept
			{
				return m_String.crbegin();
			}

			// STL interface (incomplete)
			bool empty() const noexcept
			{
				return m_String.empty();
			}
			size_t size() const noexcept
			{
				return m_String.size();
			}
			size_t length() const noexcept
			{
				return m_String.length();
			}
			size_t capacity() const noexcept
			{
				return m_String.capacity();
			}
			size_t max_size() const noexcept
			{
				return m_String.max_size();
			}
			void clear() noexcept
			{
				m_String.clear();
			}
			void reserve(size_t capacity)
			{
				m_String.reserve(capacity);
			}
			void resize(size_t capacity, XChar c = 0)
			{
				m_String.resize(capacity, c);
			}
			void assign(const XChar* data, size_t length = npos)
			{
				m_String.assign(data, CalcStringLength(data, length));
			}
			void shrink_to_fit()
			{
				m_String.shrink_to_fit();
			}

			const XChar& at(size_t i) const
			{
				return m_String.at(i);
			}
			XChar& at(size_t i)
			{
				return m_String.at(i);
			}

			const XChar& front() const
			{
				return m_String.front();
			}
			XChar& front()
			{
				return m_String.front();
			}

			const XChar& back() const
			{
				return m_String.back();
			}
			XChar& back()
			{
				return m_String.back();
			}

		public:
			String& operator=(const String& other)
			{
				m_String = other.m_String;
				m_Converted = nullptr;

				return *this;
			}
			String& operator=(String&&) noexcept = default;

			// Conversion
			operator std::basic_string<XChar>() && noexcept
			{
				return std::move(m_String);
			}
			operator wxString() const;

			// Comparison
			std::strong_ordering operator<=>(const String& other) const noexcept
			{
				return xc_view() <=> other.xc_view();
			}
			std::strong_ordering operator<=>(const wxString& other) const noexcept;
			std::strong_ordering operator<=>(const char* other) const
			{
				return CompareTo(other);
			}
			std::strong_ordering operator<=>(const wchar_t* other) const noexcept
			{
				return CompareTo(other);
			}
			std::strong_ordering operator<=>(std::string_view other) const
			{
				return CompareTo(other);
			}
			std::strong_ordering operator<=>(std::wstring_view other) const noexcept
			{
				return CompareTo(other);
			}

			bool operator==(const String& other) const noexcept
			{
				return *this <=> other == 0;
			}
			bool operator==(const wxString& other) const noexcept
			{
				return *this <=> other == 0;
			}
			bool operator==(const char* other) const
			{
				return *this <=> other == 0;
			}
			bool operator==(const wchar_t* other) const
			{
				return *this <=> other == 0;
			}
			bool operator==(std::string_view other) const
			{
				return IsSameAs(other);
			}
			bool operator==(std::wstring_view other) const noexcept
			{
				return IsSameAs(other);
			}

			bool operator==(const UniChar& other) const noexcept
			{
				return IsSameAs(other);
			}
			bool operator==(char other) const noexcept
			{
				return IsSameAs(other);
			}
			bool operator==(wchar_t other) const noexcept
			{
				return IsSameAs(other);
			}
	};
}

namespace kxf
{
	// Concatenation
	inline String operator+(const String& left, const String& right)
	{
		return left.Clone().Append(right);
	}

	template<class T> requires(std::is_constructible_v<String, T>)
	String operator+(const String& left, T&& right)
	{
		return left.Clone().Append(std::forward<T>(right));
	}

	// Conversion
	template<class T> requires(std::is_arithmetic_v<T>)
	String ToString(T value)
	{
		if constexpr(std::is_same_v<XChar, char>)
		{
			return std::to_string(value);
		}
		else if constexpr(std::is_same_v<XChar, wchar_t>)
		{
			return std::to_wstring(value);
		}
		else
		{
			static_assert(sizeof(T*) == 0, "Unsupported char type");
		}
	}

	template<class T> requires(std::is_enum_v<T>)
	String ToString(T value)
	{
		return ToString(static_cast<std::underlying_type_t<T>>(value));
	}

	// String literal operators
	inline String operator"" _s(const char* ptr, size_t length)
	{
		return String::FromUTF8(ptr, length);
	}
	inline String operator"" _s(const char8_t* ptr, size_t length)
	{
		return String::FromUTF8(ptr, length);
	}
	inline String operator"" _s(const wchar_t* ptr, size_t length)
	{
		return String(ptr, length);
	}

	inline std::basic_string_view<char> operator"" _sv(const char* ptr, size_t length)
	{
		return {ptr, length};
	}
	inline std::basic_string_view<char8_t> operator"" _sv(const char8_t* ptr, size_t length)
	{
		return {ptr, length};
	}
	inline std::basic_string_view<wchar_t> operator"" _sv(const wchar_t* ptr, size_t length)
	{
		return {ptr, length};
	}
}

namespace std
{
	template<>
	struct hash<kxf::String> final
	{
		size_t operator()(const kxf::String& string) const noexcept
		{
			return std::hash<kxf::StringView>()(string.xc_view());
		}
	};
}

namespace kxf
{
	template<>
	struct KX_API BinarySerializer<String> final
	{
		uint64_t Serialize(IOutputStream& stream, const String& value) const;
		uint64_t Deserialize(IInputStream& stream, String& value) const;
	};
}

#include "Private/Format.h"
#include "Private/StringFormatters.h"
