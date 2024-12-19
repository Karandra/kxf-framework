#include "kxf-pch.h"
#include "HTMLBox.h"
#include "WXUI/HTMLBox.h"
#include "kxf-gui/Drawing/GraphicsRenderer.h"

namespace kxf::Widgets
{
	// ComboBox
	HTMLBox::HTMLBox()
	{
		InitializeWxWidget();
	}
	HTMLBox::~HTMLBox() = default;

	// IWidget
	bool HTMLBox::CreateWidget(std::shared_ptr<IWidget> parent, const String& text, Point pos, Size size)
	{
		if (parent && Get()->Create(parent->GetWxWindow(), text, pos, size))
		{
			m_TextEntryWrapper.Initialize(*Get());
			m_ScrolledWrapper.Initialize(*Get());
			return true;
		}
		return false;
	}

	// IHTMLWidget
	String HTMLBox::GetHTML() const
	{
		return Get()->GetHTML();
	}
	void HTMLBox::SetHTML(const String& html)
	{
		Get()->SetHTML(html);
	}

	bool HTMLBox::IsSelectable() const
	{
		FlagSet style = Get()->GetWindowStyle();
		return !style.Contains(wxHW_NO_SELECTION);
	}
	void HTMLBox::SetSelectable(bool isSelectable)
	{
		FlagSet style = Get()->GetWindowStyle();
		style.Mod(wxHW_NO_SELECTION, !isSelectable);

		Get()->SetWindowStyle(*style);
	}

	void HTMLBox::SetBackgroundImage(const IImage2D& image)
	{
		Get()->SetBackgroundImage(image);
	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> HTMLBox::GetActiveGraphicsRenderer() const
	{
		if (!m_Renderer)
		{
			return Drawing::GetDefaultRenderer();
		}
		return m_Renderer;
	}
}
