#include "kxf-pch.h"
#include "String.h"
#include "RegEx.h"
#include "IEncodingConverter.h"
#include "kxf/IO/IStream.h"
#include "kxf/Utility/Common.h"
#include "kxf/wxWidgets/String.h"
#include <cctype>

namespace
{
	std::strong_ordering DoCompareStrings(std::string_view left, std::string_view right, bool ignoreCase) noexcept
	{
		if (ignoreCase)
		{
			auto a = kxf::String::FromUnknownEncoding(left);
			auto b = kxf::String::FromUnknownEncoding(right);

			return a.MakeLower().CompareTo(b.MakeLower());
		}
		else
		{
			return left <=> right;
		}
	}
	std::strong_ordering DoCompareStrings(std::wstring_view left, std::wstring_view right, bool ignoreCase) noexcept
	{
		if (ignoreCase)
		{
			constexpr size_t maxLength = std::numeric_limits<int>::max();
			wxASSERT_MSG(left.length() <= maxLength && right.length() <= maxLength, __FUNCTION__ ": strings are too long to be compared using 'CompareStringOrdinal'");

			switch (::CompareStringOrdinal(left.data(), left.length(), right.data(), right.length(), ignoreCase))
			{
				case CSTR_LESS_THAN:
				{
					return std::strong_ordering::less;
				}
				case CSTR_EQUAL:
				{
					return std::strong_ordering::equal;
				}
				case CSTR_GREATER_THAN:
				{
					return std::strong_ordering::greater;
				}
			};

			// We shouldn't get here, but in case we did, compare with case
			return left <=> right;
		}
		else
		{
			return left <=> right;
		}
	}

	template<class T>
	bool IsNameInExpressionImpl(std::basic_string_view<T> name,
								std::basic_string_view<T> expression,
								bool ignoreCase,
								const T dotChar,
								const T starChar,
								const T questionChar,
								const T DOS_STAR,
								const T DOS_QM,
								const T DOS_DOT
	)
	{
		// Check whether Name matches Expression
		// Expression can contain "?"(any one character) and "*" (any string)
		// when IgnoreCase is true, do case insensitive matching
		//
		// http://msdn.microsoft.com/en-us/library/ff546850(v=VS.85).aspx
		// * (asterisk) Matches zero or more characters.
		// ? (question mark) Matches a single character.
		//
		// DOS_DOT Matches either a period or zero characters beyond the name string.
		// DOS_QM Matches any single character or, upon encountering a period or end
		//        of name string, advances the expression to the end of the set of
		//        contiguous DOS_QMs.
		// DOS_STAR Matches zero or more characters until encountering and matching
		//          the final . in the name.

		// Both name and expression are empty, return true.
		if (name.empty() && expression.empty())
		{
			return true;
		}

		// Expression is empty or it's star char ('*'), return true.
		if (expression.empty() || (expression.length() == 1 && expression[0] == starChar))
		{
			return true;
		}

		// Empty name or empty expression, return false.
		if (name.empty() || expression.empty())
		{
			return false;
		}

		size_t nameIndex = 0;
		size_t expressionIndex = 0;
		while (expressionIndex < expression.length() && nameIndex < name.length())
		{
			if (expression[expressionIndex] == starChar)
			{
				expressionIndex++;
				if (expressionIndex >= expression.length())
				{
					return true;
				}

				while (nameIndex < name.length())
				{
					if (IsNameInExpressionImpl(expression.substr(expressionIndex), name.substr(nameIndex), ignoreCase, dotChar, starChar, questionChar, DOS_STAR, DOS_QM, DOS_DOT))
					{
						return true;
					}
					nameIndex++;
				}
			}
			else if (expression[expressionIndex] == DOS_STAR)
			{
				expressionIndex++;
				size_t lastDot = 0;

				size_t position = nameIndex;
				while (position < name.length())
				{
					if (name[position] == dotChar)
					{
						lastDot = position;
					}
					position++;
				}

				bool endReached = false;
				do
				{
					endReached = nameIndex >= name.length() || nameIndex == lastDot;
					if (!endReached)
					{
						if (IsNameInExpressionImpl(expression.substr(expressionIndex), name.substr(nameIndex), ignoreCase, dotChar, starChar, questionChar, DOS_STAR, DOS_QM, DOS_DOT))
						{
							return true;
						}
						nameIndex++;
					}
				}
				while (!endReached);
			}
			else if (expression[expressionIndex] == DOS_QM)
			{
				expressionIndex++;
				if (name[nameIndex] != dotChar)
				{
					nameIndex++;
				}
				else
				{
					size_t position = nameIndex + 1;
					while (position < name.length())
					{
						if (name[position] == dotChar)
						{
							break;
						}
						position++;
					}

					if (name[position] == dotChar)
					{
						nameIndex++;
					}
				}
			}
			else if (expression[expressionIndex] == DOS_DOT)
			{
				expressionIndex++;
				if (name[nameIndex] == dotChar)
				{
					nameIndex++;
				}
			}
			else
			{
				if (expression[expressionIndex] == questionChar || (ignoreCase && kxf::UniChar(expression[expressionIndex]).CompareNoCase(name[nameIndex]) == 0) || (!ignoreCase && expression[expressionIndex] == name[nameIndex]))
				{
					expressionIndex++;
					nameIndex++;
				}
				else
				{
					return false;
				}
			}
		}
		return !expression[expressionIndex] && !name[nameIndex] ? true : false;
	}
	bool IsNameInExpression(std::string_view name, std::string_view expression, bool ignoreCase)
	{
		constexpr char DOS_STAR = '<';
		constexpr char DOS_QM = '>';
		constexpr char DOS_DOT = '"';

		constexpr char dotChar = '.';
		constexpr char starChar = '*';
		constexpr char questionChar = '?';

		return IsNameInExpressionImpl(name, expression, ignoreCase, dotChar, starChar, questionChar, DOS_STAR, DOS_QM, DOS_DOT);
	}
	bool IsNameInExpression(std::wstring_view name, std::wstring_view expression, bool ignoreCase)
	{
		constexpr wchar_t DOS_STAR = L'<';
		constexpr wchar_t DOS_QM = L'>';
		constexpr wchar_t DOS_DOT = L'"';

		constexpr wchar_t dotChar = L'.';
		constexpr wchar_t starChar = L'*';
		constexpr wchar_t questionChar = L'?';

		return IsNameInExpressionImpl(name, expression, ignoreCase, dotChar, starChar, questionChar, DOS_STAR, DOS_QM, DOS_DOT);
	}

	template<class T, class TFunc>
	bool ConvertToInteger(T& value, int base, std::basic_string_view<kxf::XChar> buffer, TFunc&& func) noexcept
	{
		bool negate = false;
		if (base < 0)
		{
			if constexpr(std::is_signed_v<T>)
			{
				if (buffer.starts_with('-'))
				{
					negate = true;
					buffer.remove_prefix(1);
				}
			}

			const auto temp = buffer;
			if (buffer.starts_with('0'))
			{
				buffer.remove_prefix(1);
				if (buffer.starts_with('x') || buffer.starts_with('X'))
				{
					buffer.remove_prefix(1);
					base = 16;
				}
				else if (buffer.starts_with('o') || buffer.starts_with('O'))
				{
					buffer.remove_prefix(1);
					base = 8;
				}
				else if (buffer.starts_with('b') || buffer.starts_with('B'))
				{
					buffer.remove_prefix(1);
					base = 2;
				}
			}

			if (base < 0)
			{
				base = 10;
				buffer = temp;
			}
		}

		if (!buffer.empty())
		{
			const kxf::XChar* start = buffer.data();
			kxf::XChar* end = nullptr;

			errno = 0;
			auto result = std::invoke(func, start, &end, base);

			if (end != start && errno == 0)
			{
				if constexpr(std::is_signed_v<T>)
				{
					if (negate)
					{
						result = -result;
					}
				}

				value = result;
				return true;
			}
		}
		return false;
	}

	template<class T, class TFunc>
	bool ConvertToFloat(T& value, const kxf::XChar* start, TFunc&& func) noexcept
	{
		errno = 0;
		kxf::XChar* end = nullptr;
		auto result = std::invoke(func, start, &end);

		if (end != start && errno == 0)
		{
			value = result;
			return true;
		}
		return false;
	}
}

namespace kxf
{
	const String NullString;

	std::basic_string_view<XChar> StringViewOf(const String& string) noexcept
	{
		return {string.data(), string.length()};
	}
	std::basic_string_view<XChar> StringViewOf(const wxString& string) noexcept
	{
		return {string.wx_str(), string.length()};
	}
}

namespace kxf
{
	// Comparison
	std::strong_ordering String::DoCompare(std::string_view left, std::string_view right, FlagSet<StringActionFlag> flags) noexcept
	{
		return DoCompareStrings(left, right, flags & StringActionFlag::IgnoreCase);
	}
	std::strong_ordering String::DoCompare(std::wstring_view left, std::wstring_view right, FlagSet<StringActionFlag> flags) noexcept
	{
		return DoCompareStrings(left, right, flags & StringActionFlag::IgnoreCase);
	}
	std::strong_ordering String::DoCompare(UniChar left, UniChar right, FlagSet<StringActionFlag> flags) noexcept
	{
		if (flags & StringActionFlag::IgnoreCase)
		{
			return left.CompareNoCase(right);
		}
		else
		{
			return left <=> right;
		}
	}

	bool String::DoMatchesWildcards(std::string_view name, std::string_view expression, FlagSet<StringActionFlag> flags) noexcept
	{
		return IsNameInExpression(name, expression, flags & StringActionFlag::IgnoreCase);
	}
	bool String::DoMatchesWildcards(std::wstring_view name, std::wstring_view expression, FlagSet<StringActionFlag> flags) noexcept
	{
		return IsNameInExpression(name, expression, flags & StringActionFlag::IgnoreCase);
	}

	// Conversions
	UniChar String::FromUTF8(char8_t c)
	{
		const char8_t data[2] = {c, '\0'};
		String result = FromUTF8({data, 1});
		if (!result.IsEmpty())
		{
			return result.front();
		}
		return {};
	}
	String String::FromUTF8(CStrViewAdapter utf8)
	{
		return EncodingConverter_UTF8.ToWideChar(utf8.GetView());
	}
	String String::FromASCII(CStrViewAdapter ascii)
	{
		return EncodingConverter_ASCII.ToWideChar(ascii.GetView());
	}
	String String::FromLocalEncoding(CStrViewAdapter local)
	{
		return EncodingConverter_Local.ToWideChar(local.GetView());
	}
	String String::FromUnknownEncoding(CStrViewAdapter unknown)
	{
		return EncodingConverter_WhateverWorks.ToWideChar(unknown.GetView());
	}
	String String::FromFloatingPoint(double value, int precision)
	{
		return wxString::FromCDouble(value, precision);
	}

	// String length
	bool String::IsEmptyOrWhitespace() const noexcept
	{
		if (m_String.empty())
		{
			return true;
		}
		else
		{
			for (UniChar c: m_String)
			{
				if (!c.IsWhitespace())
				{
					return false;
				}
			}
			return true;
		}
	}

	// Comparison
	bool String::DoStartsWith(std::string_view pattern, String* rest, FlagSet<StringActionFlag> flags) const
	{
		String patternCopy = FromUnknownEncoding(pattern);
		return StartsWith(patternCopy, rest, flags);
	}
	bool String::DoStartsWith(std::wstring_view pattern, String* rest, FlagSet<StringActionFlag> flags) const
	{
		if (pattern.empty())
		{
			return false;
		}

		const size_t pos = Find(pattern, flags);
		if (pos == 0)
		{
			if (rest)
			{
				*rest = m_String.substr(pos, pattern.length());
			}
			return true;
		}
		return false;
	}
	
	bool String::DoEndsWith(std::string_view pattern, String* rest, FlagSet<StringActionFlag> flags) const
	{
		String patternCopy = FromUnknownEncoding(pattern);
		return EndsWith(patternCopy, rest, flags);
	}
	bool String::DoEndsWith(std::wstring_view pattern, String* rest, FlagSet<StringActionFlag> flags) const
	{
		if (pattern.empty())
		{
			return false;
		}

		const size_t pos = ReverseFind(pattern, flags);
		if (pos == m_String.length() - pattern.length())
		{
			if (rest)
			{
				*rest = m_String.substr(pos, npos);
			}
			return true;
		}
		return false;
	}

	// Construction
	String::String(const wxString& other) noexcept
		:m_String(StringViewOf(other))
	{
	}
	String::String(wxString&& other) noexcept
	{
		wxWidgets::MoveWxString(m_String, std::move(other));
	}

	// Conversions
	std::string String::ToUTF8() const
	{
		return EncodingConverter_UTF8.ToMultiByte(m_String);
	}
	std::string String::ToASCII(char replaceWith) const
	{
		std::string ascii;
		ascii.reserve(m_String.length());

		for (UniChar c: m_String)
		{
			ascii += c.ToASCII().value_or(replaceWith);
		}
		return ascii;
	}
	std::string String::ToLocalEncoding() const
	{
		return EncodingConverter_Local.ToMultiByte(m_String);
	}
	std::string String::ToEncoding(IEncodingConverter& encodingConverter) const
	{
		return encodingConverter.ToMultiByte(m_String);
	}

	// Concatenation
	String& String::DoAppend(std::string_view other)
	{
		auto converted = FromUnknownEncoding(other);
		m_String.append(converted.view());

		return *this;
	}
	String& String::DoPrepend(std::string_view other)
	{
		auto converted = FromUnknownEncoding(other);
		m_String.insert(0, converted.view());

		return *this;
	}
	String& String::DoInsert(size_t pos, std::string_view other)
	{
		auto converted = FromUnknownEncoding(other);
		m_String.insert(pos, converted.view());

		return *this;
	}

	// Substring extraction
	String String::AfterFirst(UniChar c, String* rest, FlagSet<StringActionFlag> flags) const
	{
		const size_t pos = Find(c, flags);
		if (pos != npos)
		{
			if (rest)
			{
				if (pos != 0)
				{
					*rest = SubRange(0, pos - 1);
				}
				else
				{
					rest->clear();
				}
			}
			return SubMid(pos + 1);
		}
		return {};
	}
	String String::AfterLast(UniChar c, String* rest, FlagSet<StringActionFlag> flags) const
	{
		const size_t pos = ReverseFind(c, flags);
		if (pos != npos)
		{
			if (rest)
			{
				if (pos != 0)
				{
					*rest = SubRange(0, pos - 1);
				}
				else
				{
					rest->clear();
				}
			}
			return SubMid(pos + 1);
		}
		return {};
	}

	String String::BeforeFirst(UniChar c, String* rest, FlagSet<StringActionFlag> flags) const
	{
		const size_t pos = Find(c, flags);
		if (pos != npos)
		{
			if (rest)
			{
				*rest = SubMid(pos + 1);
			}
			if (pos != 0)
			{
				return SubRange(0, pos - 1);
			}
		}
		return {};
	}
	String String::BeforeLast(UniChar c, String* rest, FlagSet<StringActionFlag> flags) const
	{
		const size_t pos = ReverseFind(c, flags);
		if (pos != npos)
		{
			if (rest)
			{
				*rest = SubMid(pos + 1);
			}
			if (pos != 0)
			{
				return SubRange(0, pos - 1);
			}
		}
		return {};
	}

	// Case conversion
	String& String::MakeLower() noexcept
	{
		::CharLowerBuffW(m_String.data(), m_String.length());
		return *this;
	}
	String& String::MakeUpper() noexcept
	{
		::CharUpperBuffW(m_String.data(), m_String.length());
		return *this;
	}

	// Searching and replacing
	size_t String::DoFind(std::string_view pattern, size_t offset, FlagSet<StringActionFlag> flags, bool reverse) const
	{
		String patternCopy = FromUnknownEncoding(pattern);
		return DoFind(StringViewOf(patternCopy), offset, flags, reverse);
	}
	size_t String::DoFind(std::wstring_view pattern, size_t offset, FlagSet<StringActionFlag> flags, bool reverse) const
	{
		if (!m_String.empty())
		{
			if (flags & StringActionFlag::IgnoreCase)
			{
				auto sourceL = ToLower();
				auto patternL = String(pattern).MakeLower();

				if (reverse)
				{
					return sourceL.m_String.rfind(patternL.view(), offset);
				}
				else
				{
					return sourceL.m_String.find(patternL.view(), offset);
				}
			}
			else
			{
				if (reverse)
				{
					return m_String.rfind(pattern, offset);
				}
				else
				{
					return m_String.find(pattern, offset);
				}
			}
		}
		return npos;
	}
	size_t String::DoFind(UniChar pattern, size_t offset, FlagSet<StringActionFlag> flags, bool reverse) const noexcept
	{
		if (m_String.empty())
		{
			return npos;
		}

		if (reverse)
		{
			if (offset >= m_String.length())
			{
				offset = 0;
			}

			for (size_t i = m_String.length() - 1 - offset; i != 0; i--)
			{
				if (Compare(m_String[i], pattern, flags) == 0)
				{
					return i;
				}
			}
		}
		else
		{
			if (offset >= m_String.length())
			{
				return npos;
			}

			for (size_t i = offset; i < m_String.length(); i++)
			{
				if (Compare(m_String[i], pattern, flags) == 0)
				{
					return i;
				}
			}
		}
		return npos;
	}

	size_t String::DoReplace(std::string_view pattern, std::string_view replacement, size_t offset, FlagSet<StringActionFlag> flags, bool reverse)
	{
		String patternCopy = FromUnknownEncoding(pattern);
		String replacementCopy = FromUnknownEncoding(replacement);
		return Replace(StringViewOf(patternCopy), StringViewOf(replacementCopy), offset, flags);
	}
	size_t String::DoReplace(std::string_view pattern, std::wstring_view replacement, size_t offset, FlagSet<StringActionFlag> flags, bool reverse)
	{
		auto patternConverted = FromUnknownEncoding(pattern);
		return DoReplace(patternConverted.view(), replacement, offset, flags, reverse);
	}
	size_t String::DoReplace(std::wstring_view pattern, std::string_view replacement, size_t offset, FlagSet<StringActionFlag> flags, bool reverse)
	{
		auto replacementConverted = FromUnknownEncoding(replacement);
		return DoReplace(pattern, replacementConverted.view(), offset, flags, reverse);
	}
	size_t String::DoReplace(std::wstring_view pattern, std::wstring_view replacement, size_t offset, FlagSet<StringActionFlag> flags, bool reverse)
	{
		const size_t replacementLength = replacement.length();
		const size_t patternLength = pattern.length();

		if (m_String.empty() || patternLength == 0 || offset >= m_String.length())
		{
			return 0;
		}

		size_t replacementCount = 0;
		size_t pos = wxString::npos;

		String sourceL;
		String patternL;
		if (flags & StringActionFlag::IgnoreCase)
		{
			sourceL = ToLower();
			patternL = String(pattern).MakeLower();

			if (reverse)
			{
				pos = sourceL.m_String.rfind(patternL.view(), offset);
			}
			else
			{
				pos = sourceL.m_String.find(patternL.view(), offset);
			}
		}
		else
		{
			if (reverse)
			{
				pos = m_String.rfind(pattern, offset);
			}
			else
			{
				pos = m_String.find(pattern, offset);
			}
		}

		while (pos != wxString::npos)
		{
			m_String.replace(pos, patternLength, replacement.data(), replacement.length());
			replacementCount++;

			if (flags & StringActionFlag::FirstMatchOnly)
			{
				return replacementCount;
			}

			if (flags & StringActionFlag::IgnoreCase)
			{
				if (reverse)
				{
					pos = sourceL.m_String.rfind(patternL.view(), pos + replacementLength);
				}
				else
				{
					pos = sourceL.m_String.find(patternL.view(), pos + replacementLength);
				}
			}
			else
			{
				if (reverse)
				{
					pos = m_String.rfind(pattern, pos + replacementLength);
				}
				else
				{
					pos = m_String.find(pattern, pos + replacementLength);
				}
			}
		}
		return replacementCount;
	}
	size_t String::DoReplace(UniChar pattern, UniChar replacement, size_t offset, FlagSet<StringActionFlag> flags, bool reverse) noexcept
	{
		if (m_String.empty() || offset >= m_String.length())
		{
			return 0;
		}

		size_t replacementCount = 0;
		auto TestAndReplace = [&](XChar& c)
		{
			if (Compare(c, pattern, flags) == 0)
			{
				c = *replacement;
				replacementCount++;

				return !flags.Contains(StringActionFlag::FirstMatchOnly);
			}
			return true;
		};

		if (reverse)
		{
			for (size_t i = m_String.length() - 1 - offset; i != 0; i--)
			{
				if (!TestAndReplace(m_String[i]))
				{
					return replacementCount;
				}
			}
		}
		else
		{
			for (size_t i = offset; i < m_String.length(); i++)
			{
				if (!TestAndReplace(m_String[i]))
				{
					return replacementCount;
				}
			}
		}
		return replacementCount;
	}

	String& String::ReplaceRange(size_t offset, size_t length, std::string_view replacement)
	{
		auto converted = FromUnknownEncoding(replacement);
		m_String.replace(offset, length, converted.view());

		return *this;
	}
	String& String::ReplaceRange(iterator first, iterator last, std::string_view replacement)
	{
		auto converted = FromUnknownEncoding(replacement);
		m_String.replace(first, last, converted.view());

		return *this;
	}

	bool String::DoContainsAnyOfCharacters(std::string_view pattern, FlagSet<StringActionFlag> flags) const noexcept
	{
		for (auto c: pattern)
		{
			if (DoFind(c, 0, flags, false) != npos)
			{
				return true;
			}
		}
		return false;
	}
	bool String::DoContainsAnyOfCharacters(std::wstring_view pattern, FlagSet<StringActionFlag> flags) const noexcept
	{
		for (auto c: pattern)
		{
			if (DoFind(c, 0, flags, false) != npos)
			{
				return true;
			}
		}
		return false;
	}

	// Conversion to numbers
	bool String::DoToFloatingPoint(float& value) const noexcept
	{
		return ConvertToFloat(value, wc_str(), std::wcstof);
	}
	bool String::DoToFloatingPoint(double& value) const noexcept
	{
		return ConvertToFloat(value, wc_str(), std::wcstod);
	}
	bool String::DoToSignedInteger(int64_t& value, int base) const noexcept
	{
		return ConvertToInteger(value, base, view(), std::wcstoll);
	}
	bool String::DoToUnsignedInteger(uint64_t& value, int base) const noexcept
	{
		return ConvertToInteger(value, base, view(), std::wcstoull);
	}

	// Miscellaneous
	size_t kxf::String::TrimScan(const String& chars, FlagSet<StringActionFlag> flags, bool left) const
	{
		auto ScanChar = [&chars, &flags](size_t& count, UniChar c)
		{
			if (chars.IsEmpty())
			{
				if (c.IsWhitespace())
				{
					count++;
					return CallbackCommand::Continue;
				}
				else
				{
					return CallbackCommand::Terminate;
				}
			}
			else
			{
				bool found = false;
				for (auto charsC: chars)
				{
					if (flags.Contains(StringActionFlag::IgnoreCase) ? c.CompareNoCase(charsC) == 0 : c.Compare(charsC) == 0)
					{
						count++;
						found = true;
						break;
					}
				}
				return found ? CallbackCommand::Continue : CallbackCommand::Terminate;
			}
		};

		size_t count = 0;
		if (left)
		{
			for (auto it = m_String.begin(); it != m_String.end(); ++it)
			{
				if (ScanChar(count, *it) == CallbackCommand::Terminate)
				{
					break;
				}
			}
		}
		else
		{
			for (auto it = m_String.rbegin(); it != m_String.rend(); ++it)
			{
				if (ScanChar(count, *it) == CallbackCommand::Terminate)
				{
					break;
				}
			}
		}

		return count;
	}
	String& String::TrimLeft(const String& chars, FlagSet<StringActionFlag> flags)
	{
		auto count = TrimScan(chars, flags, true);
		Remove(0, count);

		return *this;
	}
	String& String::TrimRight(const String& chars, FlagSet<StringActionFlag> flags)
	{
		auto count = TrimScan(chars, flags, false);
		RemoveRight(count);

		return *this;
	}
	String& String::TrimBoth(const String& chars, FlagSet<StringActionFlag> flags)
	{
		auto left = TrimScan(chars, flags, true);
		auto right = TrimScan(chars, flags, false);

		if (!flags.Contains(StringActionFlag::Symmetrical) || left == right)
		{
			Remove(0, left);
			RemoveRight(right);
		}
		return *this;
	}

	// Conversion
	String::operator wxString() const
	{
		auto buffer = wc_str();
		return wxString(buffer.data(), buffer.length());
	}

	// Comparison
	std::strong_ordering String::operator<=>(const wxString& other) const noexcept
	{
		return view() <=> StringViewOf(other);
	}
}

namespace kxf
{
	uint64_t BinarySerializer<String>::Serialize(IOutputStream& stream, const String& value) const
	{
		return Serialization::WriteObject(stream, value.ToUTF8());
	}
	uint64_t BinarySerializer<String>::Deserialize(IInputStream& stream, String& value) const
	{
		std::string buffer;
		auto read = Serialization::ReadObject(stream, buffer);
		value = String::FromUTF8(buffer);

		return read;
	}
}
