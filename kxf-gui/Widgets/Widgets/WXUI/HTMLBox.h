#pragma once
#include "Common.h"
#include "../../IHTMLWidget.h"
#include "../../IGraphicsRendererAwareWidget.h"
#include "kxf-gui/Widgets/WindowRefreshScheduler.h"
#include <wx/html/htmlwin.h>
#include <wx/systhemectrl.h>
#include <wx/textentry.h>

namespace kxf
{
	class IMenuWidget;
}

namespace kxf::WXUI
{
	class KXF_API HTMLBox: public EvtHandlerWrapper<HTMLBox, UI::WindowRefreshScheduler<wxSystemThemedControl<wxHtmlWindow>>>, public wxTextEntry
	{
		public:
			static String ProcessPlainText(const String& text);
			static bool SetupFontsUsing(const GDIFont& normalFont, String& normalFace, String& fixedFace, int& pointSize);

		private:
			IHTMLWidget& m_Widget;
			std::shared_ptr<IGraphicsRendererAwareWidget> m_RendererAware;
			wxEvtHandler m_EvtHandler;

			std::unique_ptr<IImage2D> m_BackgroundImage;
			Color m_BackgroundColor;
			const bool m_IsEditable = false;

		private:
			void CreateContextMenu(IMenuWidget& menu, const wxHtmlLinkInfo* link = nullptr);
			void CopyTextToClipboard(const String& value) const;

			void OnPaint(wxPaintEvent& event);
			void OnEraseBackground(wxEraseEvent& event);
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

		public:
			HTMLBox(IHTMLWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}
			~HTMLBox()
			{
				if (m_EvtHandler.GetClientData() == this)
				{
					PopEventHandler();
				}
			}

		public:
			bool Create(wxWindow* parent,
						const String& text,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

		public:
			// wxWindow
			wxBorder GetDefaultBorder() const override
			{
				return wxBORDER_THEME;
			}
			bool SetBackgroundColour(const wxColour& color) override
			{
				HTMLBox::SetHTMLBackgroundColour(color);
				return wxHtmlWindow::SetBackgroundColour(color);
			}

			// wxHtmlWindow
			wxColour GetHTMLBackgroundColour() const override
			{
				return m_BackgroundColor;
			}
			void SetHTMLBackgroundColour(const wxColour& color) override
			{
				m_BackgroundColor = color;
				wxHtmlWindow::SetBackgroundColour(color);
				wxHtmlWindow::SetHTMLBackgroundColour(color);

				ScheduleRefresh();
			}

			// HTMLBox
			bool IsEmpty() const
			{
				const wxString* source = wxHtmlWindow::GetParser()->GetSource();
				return source == nullptr || source->IsEmpty();
			}
			String GetHTML() const
			{
				const wxString* source = wxHtmlWindow::GetParser()->GetSource();
				return source ? *source : "";
			}
			void SetHTML(const String& html)
			{
				SetPage(html);
			}

			void SetBackgroundImage(const IImage2D& image)
			{
				m_BackgroundImage = image ? image.CloneImage2D() : nullptr;
				wxHtmlWindow::SetHTMLBackgroundImage(wxNullBitmap);

				ScheduleRefresh();
			}

		public:
			// wxTextEntry
			wxString GetValue() const override
			{
				return const_cast<HTMLBox*>(this)->ToText();
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
				const String text = const_cast<HTMLBox*>(this)->ToText();
				return !text.IsEmpty() ? text.length() - 1 : 0;
			}
			wxString GetRange(long from, long to) const override
			{
				const String text = const_cast<HTMLBox*>(this)->ToText();
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
				return const_cast<HTMLBox*>(this)->SelectionToText();
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
			}
	};
}
