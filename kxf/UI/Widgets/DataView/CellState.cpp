#include "kxf-pch.h"
#include "CellState.h"
#include "../../IWidget.h"

namespace kxf::DataView
{
	FlagSet<NativeWidgetFlag> CellState::ToNativeWidgetFlags(const IWidget& widget) const noexcept
	{
		FlagSet<NativeWidgetFlag> itemState;
		itemState.Add(NativeWidgetFlag::Selected, IsSelected());
		itemState.Add(NativeWidgetFlag::Focused, IsHotTracked() || widget.HasFocus());
		itemState.Add(NativeWidgetFlag::Current, IsHotTracked() || IsCurrent());

		return itemState;
	}
}
