#pragma once
#include "Common.h"
#include <string>
#include <wx/string.h>
#include <wx/buffer.h>

namespace kxf
{
	template<class T>
	std::basic_string_view<T> StringViewOf(const wxScopedCharTypeBuffer<T>& buffer) noexcept
	{
		return {buffer.data(), buffer.length()};
	}
}

namespace kxf::wxWidgets
{
	inline const std::basic_string<wxChar>& GetWxStringImpl(const wxString& string) noexcept
	{
		return string.ToStdWstring();
	}
	inline std::basic_string<wxChar>& GetWxStringImpl(wxString& string) noexcept
	{
		return const_cast<std::basic_string<wxChar>&>(string.ToStdWstring());
	}

	inline void MoveWxString(wxString& destination, wxString&& source) noexcept
	{
		if (&source != &destination)
		{
			// Also see a comment in the next overload
			GetWxStringImpl(destination) = std::move(GetWxStringImpl(source));
		}
	}
	inline void MoveWxString(wxString& destination, std::basic_string<wxChar>&& source) noexcept
	{
		// wxString contains an extra buffer (m_convertedTo[W]Char) to hold converted string
		// returned by 'wxString::AsCharBuf' but it seems it can be left untouched since wxString
		// always rewrites its content when requested to make conversion and only changes its size
		// when needed.

		if (&source != &GetWxStringImpl(destination))
		{
			GetWxStringImpl(destination) = std::move(source);
		}
	}
	inline void MoveWxString(std::basic_string<wxChar>& destination, wxString&& source) noexcept
	{
		if (&GetWxStringImpl(source) != &destination)
		{
			// Also see a comment in the next overload
			destination = std::move(GetWxStringImpl(source));
		}
	}
}
