#pragma once
#include "kxf/UI/Common.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include "AuiToolBarItem.h"
#include "AuiToolBarEvent.h"

namespace kxf::UI
{
	enum class AuiToolBarStyle: uint32_t
	{
		None = 0,

		Text = wxAUI_TB_TEXT,
		NoToolTips = wxAUI_TB_NO_TOOLTIPS,
		NoAutoResize = wxAUI_TB_NO_AUTORESIZE,
		Gripper = wxAUI_TB_GRIPPER,
		Overflow = wxAUI_TB_OVERFLOW,
		PlainBackground = wxAUI_TB_PLAIN_BACKGROUND,
		Vertical = wxAUI_TB_VERTICAL,
		Horizontal = wxAUI_TB_HORZ_LAYOUT,
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::AuiToolBarStyle);
}

namespace kxf::UI
{
	class KXF_API AuiToolBar: public WindowRefreshScheduler<wxAuiToolBar>
	{
		friend class AuiToolBarItem;

		public:
			static constexpr FlagSet<AuiToolBarStyle> DefaultStyle = AuiToolBarStyle::Horizontal|AuiToolBarStyle::Text;
			static constexpr int DefaultSeparatorSize = 2;

		private:
			std::unordered_map<wxWindowID, std::unique_ptr<AuiToolBarItem>> m_Items;
			Color m_ColorBorder;

		private:
			void EventHandler(wxAuiToolBarEvent& event);
			void OnLeftClick(wxCommandEvent& event);

			AuiToolBarItem* DoCreateTool(wxAuiToolBarItem* item);
			AuiToolBarItem* DoGetTool(const wxAuiToolBarItem& item);
			bool DoRemoveTool(wxAuiToolBarItem& item);
			bool DoRemoveTool(AuiToolBarItem& item)
			{
				return DoRemoveTool(*item.m_Item);
			}

			size_t DoGetToolIndex(const AuiToolBarItem& item) const;
			bool DoSetToolIndex(AuiToolBarItem& item, size_t newIndex);

		public:
			AuiToolBar() = default;
			AuiToolBar(wxWindow* parent,
					   wxWindowID id,
					   FlagSet<AuiToolBarStyle> style = DefaultStyle
			)
			{
				Create(parent, id, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						FlagSet<AuiToolBarStyle> style = DefaultStyle
			);
			~AuiToolBar()
			{
				Clear();
			}

		public:
			Color GetBorderColor() const
			{
				return m_ColorBorder;
			}
			void SetBorderColor(const Color& color)
			{
				m_ColorBorder = color;
				ScheduleRefresh();
			}

			void ClearTools();
			bool IsOverflowVisible() const
			{
				return wxAuiToolBar::GetOverflowVisible();
			}
			bool IsGripperVisible() const
			{
				return wxAuiToolBar::GetGripperVisible();
			}
			bool IsItemsFits() const
			{
				return wxAuiToolBar::GetToolBarFits();
			}

			AuiToolBarItem* AddTool(const String& label, const GDIBitmap& bitmap, wxItemKind kind = wxITEM_NORMAL, const String& shortHelp = {});
			AuiToolBarItem* AddTool(const String& label, const GDIBitmap& bitmap, const GDIBitmap& disabledBitmap, wxItemKind kind = wxITEM_NORMAL, const String& shortHelp = {});
			AuiToolBarItem* AddTool(const GDIBitmap& bitmap, const GDIBitmap& disabledBitmap, bool isToggle = false, const String& shortHelp = {}, const String& longHelp = {});

			AuiToolBarItem* AddLabel(const String& label, const int width = -1);
			AuiToolBarItem* AddControl(wxControl* control, const String& label = {});
			AuiToolBarItem* AddSeparator();
			AuiToolBarItem* AddSpacer(int pixels);
			AuiToolBarItem* AddStretchSpacer(int proportion = 1);

			AuiToolBarItem* FindToolByPosition(const Point& pos) const;
			AuiToolBarItem* FindToolByPosition(wxCoord x, wxCoord y) const
			{
				return FindToolByPosition(Point(x, y));
			}
			AuiToolBarItem* FindToolByIndex(int index) const;
			AuiToolBarItem* FindToolByID(wxWindowID id) const;

			bool RemoveTool(AuiToolBarItem& tool);
			bool RemoveTool(int index);

			void UpdateUI();

		private:
			wxAuiToolBarItem* AddTool(int tool_id, const wxString &label, const wxBitmap &bitmap, const wxString &short_help_string = {}, wxItemKind kind = wxITEM_NORMAL) = delete;
			wxAuiToolBarItem* AddTool(int tool_id, const wxString &label, const wxBitmap &bitmap, const wxBitmap &disabled_bitmap, wxItemKind kind, const wxString &short_help_string, const wxString &long_help_string, wxObject* client_data) = delete;
			wxAuiToolBarItem* AddTool(int tool_id, const wxBitmap &bitmap, const wxBitmap &disabled_bitmap, bool toggle = false, wxObject* client_data = nullptr, const wxString &short_help_string = {}, const wxString &long_help_string = {}) = delete;
			bool GetOverflowVisible() const = delete;
			bool GetToolBarFits() const = delete;
			bool GetGripperVisible() const = delete;
			wxAuiToolBarItem* FindTool(int id) const = delete;
			bool DeleteTool(int tool_id) = delete;
			bool DeleteByIndex(int tool_id) = delete;
			void ToggleTool(int tool_id, bool state) = delete;
			bool GetToolToggled(int tool_id) const = delete;
			void EnableTool(int tool_id, bool state) = delete;
			bool GetToolEnabled(int tool_id) const = delete;
			void SetToolDropDown(int tool_id, bool dropdown) = delete;
			bool GetToolDropDown(int tool_id) const = delete;
			void SetToolProportion(int tool_id, int proportion) = delete;
			int GetToolProportion(int tool_id) const = delete;
			Rect GetToolRect(int tool_id) const = delete;
			void SetToolSticky(int tool_id, bool sticky) = delete;
			bool GetToolSticky(int tool_id) const = delete;
			wxString GetToolLabel(int tool_id) const = delete;
			void SetToolLabel(int tool_id, const wxString &label) = delete;
			wxBitmap GetToolBitmap(int tool_id) const = delete;
			void SetToolBitmap(int tool_id, const wxBitmap &bitmap) = delete;
			wxString GetToolShortHelp(int tool_id) const = delete;
			void SetToolShortHelp(int tool_id, const wxString &help_string) = delete;
			wxString GetToolLongHelp(int tool_id) const = delete;
			void SetToolLongHelp(int tool_id, const wxString &help_string) = delete;
			void SetCustomOverflowItems(const wxAuiToolBarItemArray &prepend, const wxAuiToolBarItemArray &append) = delete;
			bool GetToolFitsByIndex(int tool_id) const = delete;
			int GetToolPos(int tool_id) const = delete;
			int GetToolIndex(int tool_id) const = delete;
			bool GetToolFits(int tool_id) const = delete;

		public:
			wxDECLARE_DYNAMIC_CLASS(AuiToolBar);
	};
}

namespace kxf::UI
{
	class KXF_API AuiToolBarArt: public wxAuiDefaultToolBarArt
	{
		private:
			AuiToolBar* m_Instance = nullptr;

		public:
			AuiToolBarArt(AuiToolBar& object)
				:m_Instance(&object)
			{
			}

		public:
			void DrawPlainBackground(wxDC& dc, wxWindow* window, const wxRect& rect) override;
	};
}
