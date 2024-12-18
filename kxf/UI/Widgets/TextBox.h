#pragma once
#include "Common.h"
#include "../ITextWidget.h"
#include "../Private/WxTextCtrlWrapper.h"

namespace kxf::WXUI
{
	class TextBox;
}

namespace kxf::Widgets
{
	class KXF_API TextBox: public RTTI::Implementation<TextBox, Private::BasicWxWidget<TextBox, WXUI::TextBox, ITextWidget>>
	{
		public:
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		private:
			WXUI::Private::WxTextCtrlWrapper m_TextCtrlWrapper;

		public:
			TextBox();
			~TextBox();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// ITextEntry
			bool CanCut() const override
			{
				return m_TextCtrlWrapper.CanCut();
			}
			bool CanCopy() const override
			{
				return m_TextCtrlWrapper.CanCopy();
			}
			bool CanPaste() const override
			{
				return m_TextCtrlWrapper.CanPaste();
			}
			bool CanUndo() const override
			{
				return m_TextCtrlWrapper.CanUndo();
			}
			bool CanRedo() const override
			{
				return m_TextCtrlWrapper.CanRedo();
			}

			void Cut() override
			{
				m_TextCtrlWrapper.Cut();
			}
			void Copy() override
			{
				m_TextCtrlWrapper.Undo();
			}
			void Paste() override
			{
				m_TextCtrlWrapper.Undo();
			}
			void Undo() override
			{
				m_TextCtrlWrapper.Undo();
			}
			void Redo() override
			{
				m_TextCtrlWrapper.Redo();
			}

			void ClearText() override
			{
				m_TextCtrlWrapper.ClearText();
			}
			bool IsTextEmpty() const override
			{
				return m_TextCtrlWrapper.IsTextEmpty();
			}

			bool IsEditable() const override
			{
				return m_TextCtrlWrapper.IsEditable();
			}
			void SetEditable(bool isEditable = true) override
			{
				m_TextCtrlWrapper.SetEditable(isEditable);
			}

			bool IsMultiline() const override
			{
				return m_TextCtrlWrapper.IsMultiline();
			}
			void SetMultiline(bool isMultiline = true) override
			{
				m_TextCtrlWrapper.SetMultiline(isMultiline);
			}

			bool IsModified() const override
			{
				return m_TextCtrlWrapper.IsModified();
			}
			void SetModified(bool isModified = true) override
			{
				m_TextCtrlWrapper.SetModified(isModified);
			}

			size_t GetLengthLimit() const override
			{
				return m_TextCtrlWrapper.GetLengthLimit();
			}
			void SetLengthLimit(size_t limit) override
			{
				m_TextCtrlWrapper.SetLengthLimit(limit);
			}

			size_t GetTabWidth() const
			{
				return m_TextCtrlWrapper.GetTabWidth();
			}
			void SetTabWidth(size_t width)
			{
				m_TextCtrlWrapper.SetTabWidth(width);
			}

			size_t GetInsertionPoint() const override
			{
				return m_TextCtrlWrapper.GetInsertionPoint();
			}
			void SetInsertionPoint(size_t pos) override
			{
				m_TextCtrlWrapper.SetInsertionPoint(pos);
			}

			void SelectRange(size_t from, size_t to) override
			{
				m_TextCtrlWrapper.SelectRange(from, to);
			}
			std::pair<size_t, size_t> GetSelectionRange() const override
			{
				return m_TextCtrlWrapper.GetSelectionRange();
			}
			String GetRange(size_t from, size_t to) const override
			{
				return m_TextCtrlWrapper.GetRange(from, to);
			}
			void RemoveRange(size_t from, size_t to) override
			{
				m_TextCtrlWrapper.RemoveRange(from, to);
			}
			void ReplaceRange(size_t from, size_t to, const String& text) override
			{
				m_TextCtrlWrapper.ReplaceRange(from, to, text);
			}

			size_t GetTextLength() const override
			{
				return m_TextCtrlWrapper.GetTextLength();
			}
			String GetText() const override
			{
				return m_TextCtrlWrapper.GetText();
			}
			void SetText(const String& text) override
			{
				m_TextCtrlWrapper.SetText(text);
			}
			void AppendText(const String& text) override
			{
				m_TextCtrlWrapper.AppendText(text);
			}

			String GetHint() const override
			{
				return m_TextCtrlWrapper.GetHint();
			}
			void SetHint(const String& hint) override
			{
				m_TextCtrlWrapper.SetHint(hint);
			}

			size_t GetRowCount() const override
			{
				return m_TextCtrlWrapper.GetRowCount();
			}
			size_t GetColumnCount() const override
			{
				return m_TextCtrlWrapper.GetColumnCount();
			}
			size_t GetRowLength(size_t index) const override
			{
				return m_TextCtrlWrapper.GetRowLength(index);
			}

			UniChar GetPasswordMask() const
			{
				return m_TextCtrlWrapper.GetPasswordMask();
			}
			void SetPasswordMask(const UniChar& c) const
			{
				m_TextCtrlWrapper.SetPasswordMask(c);
			}
			void RemovePasswordMask() const
			{
				m_TextCtrlWrapper.RemovePasswordMask();
			}
			using ITextEntry::SetPasswordMask;

			// ITextWidget
			void EnsurePositionVisible(size_t pos) override
			{
				m_TextCtrlWrapper.EnsurePositionVisible(pos);
			}
			Point MapPositionToCoordinates(size_t pos) const override
			{
				return m_TextCtrlWrapper.MapPositionToCoordinates(pos);
			}
			size_t MapCoordinatesToPosition(const Point& point) const override
			{
				return m_TextCtrlWrapper.MapCoordinatesToPosition(point);
			}

			Point MapPositionToRowColumn(size_t pos) const override
			{
				return m_TextCtrlWrapper.MapPositionToRowColumn(pos);
			}
			size_t MapRowColumnToPosition(const Point& rowColumn) const override
			{
				return m_TextCtrlWrapper.MapRowColumnToPosition(rowColumn);
			}

			TextWrapMode GetWrapMode() const override
			{
				return m_TextCtrlWrapper.GetWrapMode();
			}
			void SetWrapMode(TextWrapMode wrapMode) override
			{
				m_TextCtrlWrapper.SetWrapMode(wrapMode);
			}

			Size GetTextExtent() const override;
			Size GetTextExtent(const String& text) const override;
	};
}
