#include "kxf-pch.h"
#include "BasicWindowWidget.h"
#include "kxf/Drawing/GDIRenderer/GDIIcon.h"
#include <wx/window.h>
#include <wx/toplevel.h>

namespace kxf::Private
{
	// BasicWxWidgetBase
	wxTopLevelWindow* BasicWindowWidgetBase::GetWxWindow() const noexcept
	{
		return static_cast<wxTopLevelWindow*>(BasicWxWidgetBase::GetWxWindow());
	}

	// BasicTopLevelWindowWidgetBase
	BitmapImage BasicWindowWidgetBase::GetIcon() const
	{
		return GDIIcon(GetWxWindow()->GetIcon());
	}
	void BasicWindowWidgetBase::SetIcon(const BitmapImage& icon)
	{
		GetWxWindow()->SetIcon(icon.ToGDIIcon().ToWxIcon());
	}

	ImageBundle BasicWindowWidgetBase::GetIconPack() const
	{
		return GetWxWindow()->GetIcons();
	}
	void BasicWindowWidgetBase::SetIconPack(const ImageBundle& icon)
	{
		GetWxWindow()->SetIcons(icon.ToWxIconBundle());
	}

	String BasicWindowWidgetBase::GetTitle() const
	{
		return GetWxWindow()->GetTitle();
	}
	void BasicWindowWidgetBase::SetTitle(const String& title)
	{
		GetWxWindow()->SetTitle(title);
	}

	// State
	bool BasicWindowWidgetBase::IsMinimized() const
	{
		return GetWxWindow()->IsIconized();
	}
	void BasicWindowWidgetBase::Minimize()
	{
		GetWxWindow()->Iconize();
	}

	bool BasicWindowWidgetBase::IsMaximized() const
	{
		return GetWxWindow()->IsMaximized();
	}
	void BasicWindowWidgetBase::Maximize()
	{
		return GetWxWindow()->Maximize();
	}
	void BasicWindowWidgetBase::Restore()
	{
		GetWxWindow()->Restore();
	}

	bool BasicWindowWidgetBase::EnableCloseButton(bool enable)
	{
		return GetWxWindow()->EnableCloseButton(enable);
	}
	bool BasicWindowWidgetBase::EnableMinimizeButton(bool enable)
	{
		return GetWxWindow()->EnableMinimizeButton(enable);
	}
	bool BasicWindowWidgetBase::EnableMaximizeButton(bool enable)
	{
		return GetWxWindow()->EnableMaximizeButton(enable);
	}

	bool BasicWindowWidgetBase::IsFullScreen() const
	{
		return GetWxWindow()->IsFullScreen();
	}
	bool BasicWindowWidgetBase::ShowFullScreen(bool show)
	{
		return GetWxWindow()->ShowFullScreen(show);
	}
	void BasicWindowWidgetBase::ShowWithoutActivation()
	{
		GetWxWindow()->ShowWithoutActivating();
	}

	// Misc
	std::shared_ptr<IWidget> BasicWindowWidgetBase::GetDefaultWidget() const
	{
		if (auto window = GetWxWindow()->GetDefaultItem())
		{
			return Private::FindByWXObject(*window);
		}
		return nullptr;
	}
	void BasicWindowWidgetBase::SetDefaultWidget(const IWidget& widget)
	{
		GetWxWindow()->SetDefaultItem(widget.GetWxWindow());
	}

	std::shared_ptr<IMenuWidget> BasicWindowWidgetBase::GetSystemMenu() const
	{
		return nullptr;
	}
	void BasicWindowWidgetBase::RequestUserAttention(FlagSet<StdIcon> icon)
	{
		FlagSet<int> flags;
		flags.Add(wxUSER_ATTENTION_INFO, icon.Contains(StdIcon::Authentication));
		flags.Add(wxUSER_ATTENTION_INFO, icon.Contains(StdIcon::Information));
		flags.Add(wxUSER_ATTENTION_INFO, icon.Contains(StdIcon::Question));
		flags.Add(wxUSER_ATTENTION_INFO, icon.Contains(StdIcon::Warning));
		flags.Add(wxUSER_ATTENTION_ERROR, icon.Contains(StdIcon::Error));

		GetWxWindow()->RequestUserAttention(*flags);
	}

	bool BasicWindowWidgetBase::ShouldPreventApplicationExit() const
	{
		return m_PreventApplicationExit;
	}
	void BasicWindowWidgetBase::SetPreventApplicationExit(bool enable)
	{
		m_PreventApplicationExit = enable;
	}
}
