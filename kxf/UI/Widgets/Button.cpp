#include "kxf-pch.h"
#include "Button.h"
#include "WXUI/Button.h"
#include "kxf/Drawing/ArtProvider.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/Drawing/GDIRenderer/GDIBitmap.h"
#include "kxf/Core/Private/Mapping.h"

namespace kxf::Widgets
{
	// Button
	Button::Button()
	{
		InitializeWxWidget();
	}
	Button::~Button() = default;

	// IWidget
	bool Button::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	// IButtonWidget
	String Button::GetLabel(FlagSet<WidgetTextFlag> flags) const
	{
		if (flags.Contains(WidgetTextFlag::WithMnemonics))
		{
			return Get()->GetLabel();
		}
		else
		{
			return Get()->GetLabelText();
		}
	}
	void Button::SetLabel(const String& label, FlagSet<WidgetTextFlag> flags)
	{
		if (flags.Contains(WidgetTextFlag::WithMnemonics))
		{
			Get()->SetLabel(label);
		}
		else
		{
			Get()->SetLabelText(label);
		}
	}

	BitmapImage Button::GetIcon() const
	{
		return GDIBitmap(Get()->GetBitmap());
	}
	void Button::SetIcon(const BitmapImage& icon, FlagSet<Direction> direction)
	{
		Get()->SetBitmap(icon.ToGDIBitmap().ToWxBitmap(), *Private::MapDirection(direction));
	}
	void Button::SetStdIcon(FlagSet<StdIcon> stdIcon, FlagSet<Direction> direction)
	{
		if (stdIcon.Contains(StdIcon::Authentication))
		{
			Button::SetIcon(ArtProvider::GetMessageBoxResource(StdIcon::Authentication), direction);
		}
		else
		{
			Button::SetIcon({}, direction);
		}
	}

	bool Button::IsDefault() const
	{
		return Get()->IsDefault();
	}
	std::shared_ptr<IButtonWidget> Button::SetDefault()
	{
		if (auto button = Get()->SetDefault())
		{
			if (auto widget = Private::FindByWXObject(*button))
			{
				return widget->QueryInterface<IButtonWidget>();
			}
		}
		return {};
	}

	bool Button::IsDropdownEnabled() const
	{
		return Get()->IsDropdownEnabled();
	}
	void Button::SetDropdownEnbled(bool enabled)
	{
		Get()->SetDropdownEnbled(enabled);
	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> Button::GetActiveGraphicsRenderer() const
	{
		if (!m_Renderer)
		{
			return Drawing::GetDefaultRenderer();
		}
		return m_Renderer;
	}
}
