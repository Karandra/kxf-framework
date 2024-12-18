#include "kxf-pch.h"
#include "Common.h"

namespace kxf::UI::DataView
{
	bool ColumnWidth::AssignValue(int value) noexcept
	{
		m_Value = value;
		if (!IsSpecialValue())
		{
			m_Value = std::clamp(m_Value, 0, wxSystemSettings::GetMetric(wxSYS_SCREEN_X));
			return m_Value == value;
		}
		return true;
	}
}
