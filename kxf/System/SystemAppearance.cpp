#include "kxf-pch.h"
#include "SystemAppearance.h"
#include <wx/settings.h>

namespace kxf::SystemAppearance
{
	String GetName()
	{
		return wxSystemSettings::GetAppearance().GetName();
	}
	bool IsDark()
	{
		return wxSystemSettings::GetAppearance().IsDark();
	}
	bool IsUsingDarkBackground()
	{
		return wxSystemSettings::GetAppearance().IsUsingDarkBackground();
	}
}
