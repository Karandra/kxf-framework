#include "kxf-pch.h"
#include "GDIImageListRenderer.h"
#include "../View.h"
#include "../Node.h"
#include "../Column.h"

namespace kxf::UI::DataView
{
	bool GDIImageListValue::FromAny(Any& value)
	{
		if (TextValue::FromAny(value) || std::move(value).GetAs(*this))
		{
			return true;
		}
		else if (const GDIImageList* imageList = nullptr; value.GetAs(imageList))
		{
			WithImageList::SetImageList(imageList);
			return true;
		}
		return false;
	}
}

namespace kxf::UI::DataView
{
	bool GDIImageListRenderer::SetDisplayValue(Any value)
	{
		m_Value = {};
		return m_Value.FromAny(value);
	}
}
