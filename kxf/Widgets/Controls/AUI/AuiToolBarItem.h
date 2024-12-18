#pragma once
#include "kxf/Widgets/Common.h"
#include "kxf/Core/WithOptions.h"
#include <wx/aui/auibar.h>

namespace kxf::UI
{
	class AuiToolBar;
}

namespace kxf::UI
{
	enum class AuiToolBarItemOption: uint32_t
	{
		None = 0,
		MenuOnLeftClick = 1 << 0,
		MenuOnMiddleClick = 1 << 1,
		MenuOnRightClick = 1 << 2,
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::AuiToolBarItemOption);
}

namespace kxf::UI
{
	class KXF_API AuiToolBarItem: public wxEvtHandler, public WithOptions<AuiToolBarItemOption>//, public WithDropdownMenu
	{
		friend class AuiToolBar;

		public:
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		private:
			AuiToolBar* m_ToolBar = nullptr;
			wxAuiToolBarItem* m_Item = nullptr;

		private:
			Point DoGetDropdownMenuPosition(FlagSet<Alignment>* alignment, bool leftAlign) const;
			wxWindowID DoShowDropdownMenu(bool leftAlign);

		public:
			AuiToolBarItem() = default;
			AuiToolBarItem(AuiToolBar& control, wxAuiToolBarItem& item)
				:m_ToolBar(&control), m_Item(&item)
			{
			}

		public:
			bool IsOK() const noexcept
			{
				return m_ToolBar != nullptr && m_Item != nullptr;
			}
			void Refresh();
			AuiToolBar& GetToolBar() const
			{
				return *m_ToolBar;
			}

			Point GetDropdownMenuPosition(FlagSet<Alignment>* alignment = nullptr) const
			{
				return DoGetDropdownMenuPosition(alignment, !HasDropDown());
			}
			Point GetDropdownMenuPosLeftAlign(FlagSet<Alignment>* alignment = nullptr) const
			{
				return DoGetDropdownMenuPosition(alignment, true);
			}
			Point GetDropdownMenuPosRightAlign(FlagSet<Alignment>* alignment = nullptr) const
			{
				return DoGetDropdownMenuPosition(alignment, false);
			}
		
			wxWindowID ShowDropdownMenu()
			{
				return DoShowDropdownMenu(!HasDropDown());
			}
			wxWindowID ShowDropdownMenuLeftAlign()
			{
				return DoShowDropdownMenu(true);
			}
			wxWindowID ShowDropdownMenuRightAlign()
			{
				return DoShowDropdownMenu(false);
			}

			wxWindowID GetID() const;
			Rect GetRect() const;
			bool IsItemFits() const;

			int GetIndex() const;
			bool SetIndex(size_t index);

			bool IsToggled() const;
			void SetToggled(bool isPressed);

			bool IsEnabled() const;
			void SetEnabled(bool isEnabled);

			bool HasDropDown() const;
			void SetDropDown(bool isDropDown);

			bool IsSticky() const;
			void SetSticky(bool isSticky);

			void SetActive(bool isActive);
			bool IsActive() const;

			int GetProportion() const;
			void SetProportion(int proportion);

			FlagSet<Alignment> GetAlignment() const;
			void SetAlignment(FlagSet<Alignment> alignment);

			int GetSpacerPixels() const;
			void SetSpacerPixels(int pixels);

			void SetKind(wxItemKind kind);
			wxItemKind GetKind() const;

			void SetWindow(wxWindow* window);
			wxWindow* GetWindow();

			String GetLabel() const;
			void SetLabel(const String& label);

			String GetShortHelp() const;
			void SetShortHelp(const String& helpString);

			String GetLongHelp() const;
			void SetLongHelp(const String& helpString);

			GDIBitmap GetBitmap() const;
			void SetBitmap(const GDIBitmap& bitmap);

			GDIBitmap GetDisabledBitmap() const;
			void SetDisabledBitmap(const GDIBitmap& bitmap);

		public:
			explicit operator bool() const noexcept
			{
				return IsOK();
			}
			bool operator!() const noexcept
			{
				return !IsOK();
			}
	};
}
