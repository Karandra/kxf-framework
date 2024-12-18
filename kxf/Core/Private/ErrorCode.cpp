#include "kxf-pch.h"
#include "ErrorCode.h"
#include "../IErrorCode.h"

namespace kxf::Private
{
	String FormatErrorCode(const IErrorCode& error)
	{
		String formatted;

		// Error code type
		if (auto info = error.QueryInterface<RTTI::ClassInfo>())
		{
			formatted += info->GetClassName();
		}
		else
		{
			formatted += "<ErrorCode>";
		}

		// Value in hex and dec
		auto value = error.GetValue();
		formatted.Format(":[0x{:08x}/{} -> {}", value, value, error.IsSuccess() ? "Success" : "Fail");

		// Symbolic name for the value (for example 'ERROR_ACCESS_DENIED' for Win32Error = 5)
		if (auto str = error.ToString(); !str.IsEmpty())
		{
			formatted.Format(", '{}'", str);
		}

		// Localized error message (trying English first)
		if (auto message = error.GetMessage("en-us"); !message.IsEmpty())
		{
			formatted.Format(", '{}'", message);
		}
		else if (message = error.GetMessage(); !message.IsEmpty())
		{
			formatted.Format(", '{}'", message);
		}

		formatted += ']';
		return formatted;
	}
}
