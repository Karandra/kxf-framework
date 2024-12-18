#include "kxf-pch.h"
#include "StdID.h"
#include <wx/defs.h>

namespace kxf
{
	static_assert(ToInt(StdID::WX_AUTO_LOWEST) == wxID_AUTO_LOWEST);
	static_assert(ToInt(StdID::WX_AUTO_HIGHEST) == wxID_AUTO_HIGHEST);
	static_assert(ToInt(StdID::WX_LOWEST) == wxID_LOWEST);
	static_assert(ToInt(StdID::WX_HIGHEST) == wxID_HIGHEST);

	static_assert(ToInt(StdID::None) == wxID_NONE);
	static_assert(ToInt(StdID::Separator) == wxID_SEPARATOR);
	static_assert(ToInt(StdID::Any) == wxID_ANY);
	static_assert(ToInt(StdID::Open) == wxID_OPEN);
	static_assert(ToInt(StdID::Edit) == wxID_EDIT);

	static_assert(ToInt(StdID::OK) == wxID_OK);
	static_assert(ToInt(StdID::SystemMenu) == wxID_SYSTEM_MENU);
	static_assert(ToInt(StdID::MDIWindowFirst) == wxID_MDI_WINDOW_FIRST);
	static_assert(ToInt(StdID::MDIWindowLast) == wxID_MDI_WINDOW_LAST);

	static_assert(ToInt(StdID::FileDialog) == wxID_FILEDLGG);
	static_assert(ToInt(StdID::FileControl) == wxID_FILECTRL);
	static_assert(ToInt(StdID::KX_LOWEST) == wxID_HIGHEST + 1);
}

namespace kxf
{
	StdID FromWxStdID(int id) noexcept
	{
		return static_cast<StdID>(id);
	}
	int ToWxStdID(StdID id) noexcept
	{
		return static_cast<int>(id);
	}
}
