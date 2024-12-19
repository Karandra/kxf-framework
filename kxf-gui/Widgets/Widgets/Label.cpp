#include "kxf-pch.h"
#include "Label.h"
#include "WXUI/Label.h"
#include "kxf-gui/Drawing/ArtProvider.h"
#include "kxf-gui/Drawing/GraphicsRenderer.h"

namespace kxf::Widgets
{
	// Button
	Label::Label()
	{
		InitializeWxWidget();
	}
	Label::~Label() = default;

	// IWidget
	bool Label::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	// IButtonWidget
	String Label::GetLabel(FlagSet<WidgetTextFlag> flags) const
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
	void Label::SetLabel(const String& label, FlagSet<WidgetTextFlag> flags)
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

	BitmapImage Label::GetIcon() const
	{
		return Get()->GetBitmap();
	}
	void Label::SetIcon(const BitmapImage& icon, FlagSet<Direction> direction)
	{
		Get()->SetBitmap(icon);
	}
	void Label::SetStdIcon(FlagSet<StdIcon> stdIcon, FlagSet<Direction> direction)
	{
		if (stdIcon.Contains(StdIcon::Authentication))
		{
			Label::SetIcon(ArtProvider::GetMessageBoxResource(StdIcon::Authentication), direction);
		}
		else
		{
			Label::SetIcon({}, direction);
		}
	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> Label::GetActiveGraphicsRenderer() const
	{
		if (!m_Renderer)
		{
			return Drawing::GetDefaultRenderer();
		}
		return m_Renderer;
	}
}
