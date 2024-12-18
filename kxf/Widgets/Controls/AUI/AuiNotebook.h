#pragma once
#include "kxf/Widgets/Common.h"
#include "kxf/Widgets/WindowRefreshScheduler.h"
#include "kxf/wxWidgets/WithImageList.h"
#include <wx/aui/auibook.h>
#include <wx/aui/dockart.h>

namespace kxf::UI
{
	using AuiDefaultTabArtBase = wxAuiDefaultTabArt;
	using AuiDefaultDockArtBase = wxAuiDefaultDockArt;

	enum class AuiNotebookStyle: uint32_t
	{
		None = 0,

		Top = wxAUI_NB_TOP,
		Left = wxAUI_NB_LEFT,
		Right = wxAUI_NB_RIGHT,
		Bottom = wxAUI_NB_BOTTOM,
		
		ScrollButtons = wxAUI_NB_SCROLL_BUTTONS,
		TabListControls = wxAUI_NB_WINDOWLIST_BUTTON,
		CloseButton = wxAUI_NB_CLOSE_BUTTON,
		CloseOnActiveTab = wxAUI_NB_CLOSE_ON_ACTIVE_TAB,
		CloseOnAllTabs = wxAUI_NB_CLOSE_ON_ALL_TABS,
		MiddleClickClose = wxAUI_NB_MIDDLE_CLICK_CLOSE,

		TabSplit = wxAUI_NB_TAB_SPLIT,
		TabMove = wxAUI_NB_TAB_MOVE,
		TabExternalMove = wxAUI_NB_TAB_EXTERNAL_MOVE,
		TabFixedWidth = wxAUI_NB_TAB_FIXED_WIDTH,
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::AuiNotebookStyle);
}

namespace kxf::UI
{
	class KXF_API AuiNotebook: public wxWidgets::WithImageListWrapper<wxAuiNotebook>
	{
		public:
			enum class RendererType: uint32_t
			{
				Default,
				Generic,
				Simple
			};
			static constexpr FlagSet<AuiNotebookStyle> DefaultStyle = AuiNotebookStyle::Top|AuiNotebookStyle::ScrollButtons;

		private:
			int m_TabIndent = 3;
			RendererType m_CurrentRendererType = RendererType::Default;

		public:
			AuiNotebook() = default;
			AuiNotebook(wxWindow* parent,
						wxWindowID id,
						FlagSet<AuiNotebookStyle> style = DefaultStyle
			)
			{
				Create(parent, id, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						FlagSet<AuiNotebookStyle> style = DefaultStyle
			);

		public:
			bool ShouldInheritColours() const override
			{
				return true;
			}

			int GetTabIndent() const
			{
				return m_TabIndent;
			}
			void SetTabIndent(int value)
			{
				m_TabIndent = value;
			}

			RendererType GetRenderer() const
			{
				return m_CurrentRendererType;
			}
			void SetRenderer(RendererType value);

		public:
			wxDECLARE_DYNAMIC_CLASS(AuiNotebook);
	};
}

namespace kxf::UI
{
	class KXF_API AuiDefaultTabArt: public AuiDefaultTabArtBase
	{
		private:
			AuiNotebook* m_Notebook = nullptr;

		public:
			AuiDefaultTabArt(AuiNotebook& object)
				:m_Notebook(&object)
			{
			}

		public:
			AuiDefaultTabArt* Clone() override
			{
				return new AuiDefaultTabArt(*this);
			}

			void DrawBackground(wxDC& dc, wxWindow* window, const wxRect& rect) override;
			void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& pane, const wxRect& inRect, int closeButtonState, wxRect* outTabRect, wxRect* outButtonRect, int* xExtent) override;
			void DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& inRect, int bitmapId, int buttonState, int orientation, wxRect* outRect) override;

			int GetIndentSize() override
			{
				if (m_Notebook)
				{
					return m_Notebook->GetTabIndent();
				}
				return 3; // 3 is the default in wxAuiDefaultTabArt
			}
	};
}

namespace kxf::UI
{
	class KXF_API AuiNotebookDockArtProvider: public AuiDefaultDockArtBase
	{
		private:
			AuiNotebook* m_Notebook = nullptr;

		public:
			AuiNotebookDockArtProvider(AuiNotebook& object)
				:m_Notebook(&object)
			{
			}

		public:
			void DrawBackground(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect) override;
			void DrawBorder(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane) override;
	};
}
