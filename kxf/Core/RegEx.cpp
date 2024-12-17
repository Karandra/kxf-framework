#include "KxfPCH.h"
#include "RegEx.h"
#include "kxf/wxWidgets/String.h"
#include <wx/regex.h>

namespace
{
	struct HackWxRegEx final
	{
		wxRegExImpl* m_Impl = nullptr;
	};
}

namespace kxf
{
	String RegEx::EscapeMeta(const String& value)
	{
		return wxRegEx::QuoteMeta(value);
	}

	void RegEx::MoveFrom(RegEx& other) noexcept
	{
		// Construct an uninitialized object or destroy and reconstruct an already used one
		if (!m_RegEx.IsConstructed())
		{
			m_RegEx.Construct();
		}
		else if (m_RegEx->IsValid())
		{
			m_RegEx.Destroy();
			m_RegEx.Construct();
		}

		if (other.m_RegEx.IsConstructed())
		{
			auto thisPtr = reinterpret_cast<HackWxRegEx*>(m_RegEx.get());
			auto otherPtr = reinterpret_cast<HackWxRegEx*>(other.m_RegEx.get());

			thisPtr->m_Impl = otherPtr->m_Impl;
			otherPtr->m_Impl = nullptr;
		}
	}

	RegEx::~RegEx() noexcept
	{
		m_RegEx.Destroy();
	}

	bool RegEx::IsNull() const noexcept
	{
		return !m_RegEx.IsConstructed() || !m_RegEx->IsValid();
	}
	bool RegEx::Compile(const String& expression, FlagSet<RegExFlag> flags)
	{
		if (!m_RegEx.IsConstructed())
		{
			m_RegEx.Construct();
		}

		constexpr auto MapRegExFlag = [](FlagSet<RegExFlag> flags) noexcept
		{
			FlagSet<int> nativeFlags = wxRE_EXTENDED|wxRE_ADVANCED;
			nativeFlags.Add(wxRE_ICASE, flags & RegExFlag::IgnoreCase);
			nativeFlags.Add(wxRE_NOSUB, flags & RegExFlag::NoSubstitution);
			nativeFlags.Add(wxRE_NEWLINE, flags & RegExFlag::NewLine);

			return nativeFlags;
		};
		return m_RegEx->Compile(expression, *MapRegExFlag(flags));
	}

	bool RegEx::Matches(const String& text, FlagSet<RegExCompileFlag> flags) const
	{
		if (IsNull())
		{
			return false;
		}

		constexpr auto MapRegExCompileFlag = [](FlagSet<RegExCompileFlag> flags) noexcept
		{
			FlagSet<int> nativeFlags;
			nativeFlags.Add(wxRE_NOTBOL, flags & RegExCompileFlag::NotBegin);
			nativeFlags.Add(wxRE_NOTEOL, flags & RegExCompileFlag::NotEnd);

			return nativeFlags;
		};
		return m_RegEx->Matches(text, *MapRegExCompileFlag(flags));
	}
	size_t RegEx::GetMatchCount() const noexcept
	{
		if (IsNull())
		{
			return 0;
		}

		return m_RegEx->GetMatchCount();
	}
	String RegEx::GetMatch(const String& text, size_t index) const
	{
		if (IsNull())
		{
			return {};
		}

		return m_RegEx->GetMatch(text, index);
	}
	CallbackResult<size_t> RegEx::EnumMatches(const String& text, CallbackFunction<String> func) const
	{
		if (IsNull())
		{
			return {};
		}

		size_t matchCount = m_RegEx->GetMatchCount();
		for (size_t i = 0; i < matchCount; i++)
		{
			size_t start = 0;
			size_t length = 0;
			if (m_RegEx->GetMatch(&start, &length, i))
			{
				if (func.Invoke(text.SubMid(start, length)).ShouldTerminate())
				{
					break;
				}
			}
		}
		return func.Finalize(matchCount);
	}
	bool RegEx::GetMatch(size_t& start, size_t& length, size_t index) const noexcept
	{
		if (IsNull())
		{
			return false;
		}

		return m_RegEx->GetMatch(&start, &length, index);
	}

	size_t RegEx::Replace(String& text, const String& replacement, size_t maxMatches) const
	{
		if (IsNull())
		{
			return 0;
		}

		wxString temp;
		wxWidgets::MoveWxString(temp, std::move(text.impl_str()));

		auto result = std::clamp(m_RegEx->Replace(&temp, replacement, maxMatches), 0, std::numeric_limits<int>::max());
		wxWidgets::MoveWxString(text.impl_str(), std::move(temp));

		return result;
	}
	size_t RegEx::ReplaceAll(String& text, const String& replacement) const
	{
		return Replace(text, replacement, 0);
	}
	size_t RegEx::ReplaceFirst(String& text, const String& replacement) const
	{
		return Replace(text, replacement, 1);
	}
}
