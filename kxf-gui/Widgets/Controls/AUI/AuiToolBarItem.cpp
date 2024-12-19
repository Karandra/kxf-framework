#include "kxf-pch.h"
#include "AuiToolBarItem.h"
#include "AuiToolBar.h"
#include "kxf/Utility/Common.h"

namespace kxf::UI
{
	Point AuiToolBarItem::DoGetDropdownMenuPosition(FlagSet<Alignment>* alignment, bool leftAlign) const
	{
		Rect rect = GetRect();
		if (!rect.IsEmpty())
		{
			if (leftAlign)
			{
				Utility::SetIfNotNull(alignment, Alignment::Left|Alignment::Top);
				return rect.GetLeftBottom() + Point(0, 2);
			}
			else
			{
				Utility::SetIfNotNull(alignment, Alignment::Right|Alignment::Top);
				return rect.GetRightBottom() + Point(0, 2);
			}
		}
		return Point::UnspecifiedPosition();
	}
	wxWindowID AuiToolBarItem::DoShowDropdownMenu(bool leftAlign)
	{
		FlagSet<Alignment> alignment;
		Point pos = DoGetDropdownMenuPosition(&alignment, leftAlign);
		//wxWindowID ret = GetDropdownMenu()->Show(m_ToolBar, pos, alignment);
		wxWindowID ret = wxID_NONE;

		// To make parent window respond to mouse events without clicking to window manually
		m_ToolBar->GenerateMouseLeave();
		m_ToolBar->GetParent()->CaptureMouse();
		m_ToolBar->GetParent()->ReleaseMouse();
		return ret;
	}

	void AuiToolBarItem::Refresh()
	{
		m_ToolBar->RefreshRect(GetRect());
	}

	wxWindowID AuiToolBarItem::GetID() const
	{
		return m_Item->GetId();
	}
	Rect AuiToolBarItem::GetRect() const
	{
		return Rect(m_ToolBar->wxAuiToolBar::GetToolRect(GetID()));
	}
	bool AuiToolBarItem::IsItemFits() const
	{
		return m_ToolBar->wxAuiToolBar::GetToolFits(GetID());
	}

	int AuiToolBarItem::GetIndex() const
	{
		return m_ToolBar->DoGetToolIndex(*this);
	}
	bool AuiToolBarItem::SetIndex(size_t index)
	{
		return m_ToolBar->DoSetToolIndex(*this, index);
	}

	bool AuiToolBarItem::IsToggled() const
	{
		return m_ToolBar->wxAuiToolBar::GetToolToggled(GetID());
	}
	void AuiToolBarItem::SetToggled(bool isPressed)
	{
		m_ToolBar->wxAuiToolBar::ToggleTool(GetID(), isPressed);
		Refresh();
	}

	bool AuiToolBarItem::IsEnabled() const
	{
		return m_ToolBar->wxAuiToolBar::GetToolEnabled(GetID());
	}
	void AuiToolBarItem::SetEnabled(bool isEnabled)
	{
		m_ToolBar->wxAuiToolBar::EnableTool(GetID(), isEnabled);
		Refresh();
	}

	bool AuiToolBarItem::HasDropDown() const
	{
		return m_Item->HasDropDown();
	}
	void AuiToolBarItem::SetDropDown(bool isDropDown)
	{
		m_Item->SetHasDropDown(isDropDown);
	}

	bool AuiToolBarItem::IsSticky() const
	{
		return m_Item->IsSticky();
	}
	void AuiToolBarItem::SetSticky(bool isSticky)
	{
		m_Item->SetSticky(isSticky);
	}

	void AuiToolBarItem::SetActive(bool isActive)
	{
		m_Item->SetActive(isActive);
	}
	bool AuiToolBarItem::IsActive() const
	{
		return m_Item->IsActive();
	}

	int AuiToolBarItem::GetProportion() const
	{
		return m_ToolBar->wxAuiToolBar::GetToolProportion(GetID());
	}
	void AuiToolBarItem::SetProportion(int proportion)
	{
		m_ToolBar->wxAuiToolBar::SetToolProportion(GetID(), proportion);
	}

	FlagSet<Alignment> AuiToolBarItem::GetAlignment() const
	{
		return FlagSet<Alignment>().FromInt(m_Item->GetAlignment());
	}
	void AuiToolBarItem::SetAlignment(FlagSet<Alignment> alignment)
	{
		m_Item->SetAlignment(alignment);
	}

	int AuiToolBarItem::GetSpacerPixels() const
	{
		return m_Item->GetSpacerPixels();
	}
	void AuiToolBarItem::SetSpacerPixels(int pixels)
	{
		m_Item->SetSpacerPixels(pixels);
	}

	void AuiToolBarItem::SetKind(wxItemKind kind)
	{
		m_Item->SetKind(kind);
	}
	wxItemKind AuiToolBarItem::GetKind() const
	{
		return static_cast<wxItemKind>(m_Item->GetKind());
	}

	void AuiToolBarItem::SetWindow(wxWindow* window)
	{
		return m_Item->SetWindow(window);
	}
	wxWindow* AuiToolBarItem::GetWindow()
	{
		return m_Item->GetWindow();
	}

	String AuiToolBarItem::GetLabel() const
	{
		return m_Item->GetLabel();
	}
	void AuiToolBarItem::SetLabel(const String& label)
	{
		m_Item->SetLabel(label);
	}

	String AuiToolBarItem::GetShortHelp() const
	{
		return m_Item->GetShortHelp();
	}
	void AuiToolBarItem::SetShortHelp(const String& helpString)
	{
		m_Item->SetShortHelp(helpString);
	}

	String AuiToolBarItem::GetLongHelp() const
	{
		return m_Item->GetLongHelp();
	}
	void AuiToolBarItem::SetLongHelp(const String& helpString)
	{
		m_Item->SetLongHelp(helpString);
	}

	GDIBitmap AuiToolBarItem::GetBitmap() const
	{
		return m_Item->GetBitmap();
	}
	void AuiToolBarItem::SetBitmap(const GDIBitmap& bitmap)
	{
		m_Item->SetBitmap(bitmap.AsWXBitmap());
		Refresh();
	}

	GDIBitmap AuiToolBarItem::GetDisabledBitmap() const
	{
		return m_Item->GetDisabledBitmap();
	}
	void AuiToolBarItem::SetDisabledBitmap(const GDIBitmap& bitmap)
	{
		m_Item->SetDisabledBitmap(bitmap.AsWXBitmap());
		Refresh();
	}
}
