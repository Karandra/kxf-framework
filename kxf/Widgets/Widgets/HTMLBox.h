#pragma once
#include "Common.h"
#include "../IHTMLWidget.h"
#include "../IScrollableWidget.h"
#include "../IGraphicsRendererAwareWidget.h"
#include "../Private/WxTextEntryWrapper.h"
#include "../Private/WxScrolledWrapper.h"

namespace kxf::WXUI
{
	class HTMLBox;
}

namespace kxf::Widgets
{
	class KXF_API HTMLBox: public RTTI::Implementation<HTMLBox, Private::BasicWxWidget<HTMLBox, WXUI::HTMLBox, IHTMLWidget>, IScrollableWidget, IGraphicsRendererAwareWidget>
	{
		private:
			std::shared_ptr<IGraphicsRenderer> m_Renderer;
			WXUI::Private::WxTextEntryWrapper m_TextEntryWrapper;
			WXUI::Private::WxScrolledWrapper m_ScrolledWrapper;

		public:
			HTMLBox();
			~HTMLBox();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& text = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IHTMLWidget
			String GetHTML() const override;
			void SetHTML(const String& html) override;

			bool IsSelectable() const override;
			void SetSelectable(bool isSelectable = true) override;

			void SetBackgroundImage(const IImage2D& image) override;

			// ITextEntry
			bool CanCut() const override
			{
				return m_TextEntryWrapper.CanCut();
			}
			bool CanCopy() const override
			{
				return m_TextEntryWrapper.CanCopy();
			}
			bool CanPaste() const override
			{
				return m_TextEntryWrapper.CanPaste();
			}
			bool CanUndo() const override
			{
				return m_TextEntryWrapper.CanUndo();
			}
			bool CanRedo() const override
			{
				return m_TextEntryWrapper.CanRedo();
			}

			void Cut() override
			{
				m_TextEntryWrapper.Cut();
			}
			void Copy() override
			{
				m_TextEntryWrapper.Undo();
			}
			void Paste() override
			{
				m_TextEntryWrapper.Undo();
			}
			void Undo() override
			{
				m_TextEntryWrapper.Undo();
			}
			void Redo() override
			{
				m_TextEntryWrapper.Redo();
			}

			void ClearText() override
			{
				m_TextEntryWrapper.ClearText();
			}
			bool IsTextEmpty() const override
			{
				return m_TextEntryWrapper.IsTextEmpty();
			}

			bool IsEditable() const override
			{
				return m_TextEntryWrapper.IsEditable();
			}
			void SetEditable(bool isEditable = true) override
			{
				m_TextEntryWrapper.SetEditable(isEditable);
			}

			bool IsMultiline() const override
			{
				return false;
			}
			void SetMultiline(bool isMultiline = true) override
			{
			}

			bool IsModified() const override
			{
				return m_TextEntryWrapper.IsModified();
			}
			void SetModified(bool isModified = true) override
			{
				m_TextEntryWrapper.SetModified(isModified);
			}

			size_t GetLengthLimit() const override
			{
				return m_TextEntryWrapper.GetLengthLimit();
			}
			void SetLengthLimit(size_t limit) override
			{
				m_TextEntryWrapper.SetLengthLimit(limit);
			}

			size_t GetTabWidth() const override
			{
				return m_TextEntryWrapper.GetTabWidth();
			}
			void SetTabWidth(size_t width) override
			{
				m_TextEntryWrapper.SetTabWidth(width);
			}

			size_t GetInsertionPoint() const override
			{
				return m_TextEntryWrapper.GetInsertionPoint();
			}
			void SetInsertionPoint(size_t pos) override
			{
				m_TextEntryWrapper.SetInsertionPoint(pos);
			}

			void SelectRange(size_t from, size_t to) override
			{
				m_TextEntryWrapper.SelectRange(from, to);
			}
			std::pair<size_t, size_t> GetSelectionRange() const override
			{
				return m_TextEntryWrapper.GetSelectionRange();
			}
			String GetRange(size_t from, size_t to) const override
			{
				return m_TextEntryWrapper.GetRange(from, to);
			}
			void RemoveRange(size_t from, size_t to) override
			{
				m_TextEntryWrapper.RemoveRange(from, to);
			}
			void ReplaceRange(size_t from, size_t to, const String& text) override
			{
				m_TextEntryWrapper.ReplaceRange(from, to, text);
			}

			size_t GetTextLength() const override
			{
				return m_TextEntryWrapper.GetTextLength();
			}
			String GetText() const override
			{
				return m_TextEntryWrapper.GetText();
			}
			void SetText(const String& text) override
			{
				m_TextEntryWrapper.SetText(text);
			}
			void AppendText(const String& text) override
			{
				m_TextEntryWrapper.AppendText(text);
			}

			String GetHint() const override
			{
				return m_TextEntryWrapper.GetHint();
			}
			void SetHint(const String& hint) override
			{
				m_TextEntryWrapper.SetHint(hint);
			}

			size_t GetRowCount() const override
			{
				return 1;
			}
			size_t GetColumnCount() const override
			{
				return m_TextEntryWrapper.GetTextLength();
			}
			size_t GetRowLength(size_t index) const override
			{
				return index == 0 ? m_TextEntryWrapper.GetTextLength() : 0;
			}

			UniChar GetPasswordMask() const
			{
				return {};
			}
			void SetPasswordMask(const UniChar& c) const
			{
			}
			void RemovePasswordMask() const
			{
			}
			using ITextEntry::SetPasswordMask;

			// IScrollableWidget
			bool IsKeyboardScrollingEnabled() const override
			{
				return m_ScrolledWrapper.IsKeyboardScrollingEnabled();
			}
			void SetKeyboardScrollingEnabled(bool enabled = true) override
			{
				m_ScrolledWrapper.SetKeyboardScrollingEnabled(enabled);
			}

			Point CalcScrolledPosition(const Point& point) const override
			{
				return m_ScrolledWrapper.CalcScrolledPosition(point);
			}
			Point CalcUnscrolledPosition(const Point& point) const override
			{
				return m_ScrolledWrapper.CalcUnscrolledPosition(point);
			}

			void SetupScrollbars(const Size& pixelPerUnit, const Size& unitCount) override
			{
				m_ScrolledWrapper.SetupScrollbars(pixelPerUnit, unitCount);
			}
			void ShowScrollbars(ScrollbarVisibility x, ScrollbarVisibility y) override
			{
				m_ScrolledWrapper.ShowScrollbars(x, y);
			}

			Size GetScrollRate() const override
			{
				return m_ScrolledWrapper.GetScrollRate();
			}
			void SetScrollRate(const Size& scrollRate) override
			{
				m_ScrolledWrapper.SetScrollRate(scrollRate);
			}

			Point GetViewStart() const override
			{
				return m_ScrolledWrapper.GetViewStart();
			}
			void ScrollTo(const Point& point) override
			{
				m_ScrolledWrapper.ScrollTo(point);
			}

			SizeF GetScrollScale() const override
			{
				return m_ScrolledWrapper.GetScrollScale();
			}
			void SetScrollScale(const SizeF& scale) override
			{
				m_ScrolledWrapper.SetScrollScale(scale);
			}

			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override
			{
				m_Renderer = std::move(renderer);
			}
	};
}
