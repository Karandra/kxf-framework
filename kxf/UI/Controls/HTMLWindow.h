#pragma once
#include "kxf/UI/Common.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include <wx/html/htmlwin.h>
#include <wx/textentry.h>
#include <wx/graphics.h>

namespace kxf::UI
{
	class Menu;

	enum class HTMLWindowStyle: uint32_t
	{
		None = 0,

		ScrollbarAuto = wxHW_SCROLLBAR_AUTO,
		ScrollbarNever = wxHW_SCROLLBAR_NEVER,
		NoSelection = wxHW_NO_SELECTION,
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::HTMLWindowStyle);
}

namespace kxf::UI
{
	class KXF_API HTMLWindow: public WindowRefreshScheduler<wxHtmlWindow>, public wxTextEntry
	{
		public:
			static constexpr FlagSet<HTMLWindowStyle> DefaultStyle = HTMLWindowStyle::ScrollbarAuto;

		public:
			static String ProcessPlainText(const String& text);
			static bool SetupFontsUsing(const GDIFont& normalFont, String& normalFace, String& fixedFace, int& pointSize);

		private:
			GDIBitmap m_BackgroundBitmap;
			Color m_BackgroundColor;
			wxGraphicsRenderer* m_Renderer = nullptr;

			const bool m_IsEditable = false;

		private:
			void CopyTextToClipboard(const String& value) const;
			void CreateContextMenu(Menu& menu, const wxHtmlLinkInfo* link = nullptr);
			void ExecuteContextMenu(Menu& menu, const wxHtmlLinkInfo* link = nullptr);

			void OnContextMenu(wxContextMenuEvent& event);
			void OnKey(wxKeyEvent& event);

		protected:
			virtual String OnProcessPlainText(const String& text) const;
			void OnHTMLLinkClicked(const wxHtmlLinkInfo& link) override;
			wxHtmlOpeningStatus OnHTMLOpeningURL(wxHtmlURLType type, const wxString& url, wxString* redirect) const override;

			bool DoSetFont(const GDIFont& normalFont);
			bool DoSetValue(const String& value);
			bool DoAppendValue(const String& value);

			WXHWND GetEditHWND() const override
			{
				return nullptr;
			}
			wxWindow* GetEditableWindow() override
			{
				return this;
			}

			void OnEraseBackground(wxEraseEvent& event);
			void OnPaint(wxPaintEvent& event);

		public:
			HTMLWindow() = default;
			HTMLWindow(wxWindow* parent,
					   wxWindowID id,
					   const String& text = {},
					   FlagSet<HTMLWindowStyle> style = DefaultStyle
			)
			{
				Create(parent, id, text, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& text = {},
						FlagSet<HTMLWindowStyle> style = DefaultStyle
			);
			~HTMLWindow() = default;

		public:
			// HTMLWindow
			wxBorder GetDefaultBorder() const override
			{
				return wxBORDER_THEME;
			}

			wxGraphicsRenderer* GetRenderer() const
			{
				return m_Renderer;
			}
			void SetRenderer(wxGraphicsRenderer* renderer)
			{
				m_Renderer = renderer;
				ScheduleRefresh();
			}

			GDIBitmap GetHTMLBackgroundImage() const
			{
				return m_BackgroundBitmap;
			}
			void SetHTMLBackgroundImage(const wxBitmapBundle& bitmapBundle) override
			{
				m_BackgroundBitmap = bitmapBundle.GetBitmap(wxDefaultSize);
				wxHtmlWindow::SetHTMLBackgroundImage(bitmapBundle);
				ScheduleRefresh();
			}
			void SetHTMLBackgroundImage(const GDIBitmap& bitmap)
			{
				m_BackgroundBitmap = bitmap;

				wxBitmapBundle bitmapBundle(bitmap.ToWxBitmap());
				HTMLWindow::SetHTMLBackgroundImage(bitmapBundle);
				ScheduleRefresh();
			}

			wxColour GetHTMLBackgroundColour() const override
			{
				return m_BackgroundColor;
			}
			void SetHTMLBackgroundColour(const wxColour& color) override
			{
				m_BackgroundColor = color;
				wxHtmlWindow::SetHTMLBackgroundColour(color);
				ScheduleRefresh();
			}

			String GetPage() const
			{
				const wxString* source = wxHtmlWindow::GetParser()->GetSource();
				return source ? *source : "";
			}
			bool IsEmpty() const
			{
				const wxString* source = wxHtmlWindow::GetParser()->GetSource();
				return source == nullptr || source->IsEmpty();
			}

		public:
			// wxTextEntry
			wxString GetValue() const override
			{
				return const_cast<HTMLWindow*>(this)->ToText();
			}
			void SetValue(const wxString& value) override
			{
				DoSetValue(value);
			}
			void ChangeValue(const wxString& value) override
			{
				DoSetValue(value);
			}
			void AppendText(const wxString& text) override
			{
				DoAppendValue(text);
			}
			void WriteText(const wxString& text) override
			{
				DoAppendValue(text);
			}
			void Clear() override
			{
				wxHtmlWindow::SetPage({});
			}

			void Cut() override
			{
			}
			void Copy() override
			{
				CopyTextToClipboard(wxHtmlWindow::SelectionToText());
			}
			void Paste() override
			{
			}
			void Undo() override
			{
			}
			void Redo() override
			{
			}

			bool CanCopy() const override
			{
				return HasSelection();
			}
			bool CanCut() const override
			{
				return IsEditable() && HasSelection();
			}
			bool CanPaste() const override
			{
				return IsEditable();
			}
			bool CanUndo() const override
			{
				return IsEditable();
			}
			bool CanRedo() const override
			{
				return IsEditable();
			}

			wxTextPos GetInsertionPoint() const override
			{
				return 0;
			}
			wxTextPos GetLastPosition() const override
			{
				const String text = const_cast<HTMLWindow*>(this)->ToText();
				return !text.IsEmpty() ? text.length() - 1 : 0;
			}
			wxString GetRange(long from, long to) const override
			{
				const String text = const_cast<HTMLWindow*>(this)->ToText();
				return text.SubRange(from, to);
			}
			void SetInsertionPoint(long pos) override
			{
			}
			void SetInsertionPointEnd() override
			{
			}
			void SetMaxLength(unsigned long len) override
			{
			}

			wxString GetStringSelection() const override
			{
				return const_cast<HTMLWindow*>(this)->SelectionToText();
			}
			bool HasSelection() const
			{
				return m_selection && !m_selection->IsEmpty();
			}
			void GetSelection(long* from, long* to) const override
			{
				if (from)
				{
					*from = m_selection ? m_selection->GetFromCharacterPos() : 0;
				}
				if (to)
				{
					*to = m_selection ? m_selection->GetToCharacterPos() : 0;
				}
			}
			void SetSelection(long from, long to) override
			{
				// Initialize selection if none
				wxHtmlWindow::SelectAll();

				m_selection->SetFromCharacterPos(from);
				m_selection->SetToCharacterPos(to);
			}
			void SelectAll() override
			{
				wxHtmlWindow::SelectAll();
			}
			void SelectNone() override
			{
				SetSelection(0, 0);
			}

			wxString GetHint() const override
			{
				return {};
			}
			bool SetHint(const wxString& hint) override
			{
				return false;
			}

			bool IsEditable() const override
			{
				return m_IsEditable;
			}
			void SetEditable(bool isEditable) override
			{
				// Not implemented
				//m_IsEditable = isEditable;
			}

		public:
			HTMLWindow& operator<<(const String& value)
			{
				AppendText(value);
				return *this;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(HTMLWindow);
	};
}
