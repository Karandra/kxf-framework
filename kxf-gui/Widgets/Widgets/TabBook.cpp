#include "kxf-pch.h"
#include "TabBook.h"
#include "WXUI/TabBook.h"
#include "kxf-gui/Drawing/GraphicsRenderer.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIBitmap.h"

namespace kxf::Widgets
{
	// Simplebook
	TabBook::TabBook()
	{
		InitializeWxWidget();
	}
	TabBook::~TabBook() = default;

	// IWidget
	bool TabBook::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent && Get()->Create(parent->GetWxWindow(), label, pos, size))
		{
			m_BookCtrlWrapper.Initialize(*Get());
			return true;
		}
		return false;
	}

	// IBookWidget
	BitmapImage TabBook::GetPageImage(size_t index) const
	{
		return Get()->GetPageBitmap(index);
	}
	void TabBook::SetPageImage(size_t index, const BitmapImage& image)
	{
		Get()->SetPageBitmap(index, image.ToWXBitmap());
	}

	// ITabBookWidget
	Direction TabBook::GetTabPosition() const
	{
		FlagSet style = Get()->GetWindowStyle();

		if (style.Contains(wxAUI_NB_BOTTOM))
		{
			return Direction::Down;
		}
		else if (style.Contains(wxAUI_NB_LEFT))
		{
			return Direction::Left;
		}
		else if (style.Contains(wxAUI_NB_RIGHT))
		{
			return Direction::Right;
		}
		return Direction::Up;
	}
	void TabBook::SetTabPosition(Direction position)
	{
		FlagSet style = Get()->GetWindowStyle();
		style.Mod(wxAUI_NB_TOP, position == Direction::Up);
		style.Mod(wxAUI_NB_BOTTOM, position == Direction::Down);
		style.Mod(wxAUI_NB_LEFT, position == Direction::Left);
		style.Mod(wxAUI_NB_RIGHT, position == Direction::Right);

		Get()->SetWindowStyle(*style);
	}

	String TabBook::GetTabTooltip(size_t index) const
	{
		return Get()->GetPageToolTip(index);
	}
	void TabBook::SetTabTooltip(size_t index, const String& tooltip)
	{
		Get()->SetPageToolTip(index, tooltip);
	}

	bool TabBook::IsCloseButtonVisible() const
	{
		FlagSet style = Get()->GetWindowStyle();

		return style.Contains(wxAUI_NB_CLOSE_BUTTON|wxAUI_NB_CLOSE_ON_ACTIVE_TAB|wxAUI_NB_CLOSE_ON_ALL_TABS|wxAUI_NB_MIDDLE_CLICK_CLOSE);
	}
	void TabBook::SetCloseButtonVisible(bool isVisible)
	{
		FlagSet style = Get()->GetWindowStyle();
		style.Mod(wxAUI_NB_CLOSE_BUTTON|wxAUI_NB_CLOSE_ON_ACTIVE_TAB|wxAUI_NB_CLOSE_ON_ALL_TABS|wxAUI_NB_MIDDLE_CLICK_CLOSE, isVisible);

		Get()->SetWindowStyle(*style);
	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> TabBook::GetActiveGraphicsRenderer() const
	{
		if (!m_Renderer)
		{
			return Drawing::GetDefaultRenderer();
		}
		return m_Renderer;
	}
}
