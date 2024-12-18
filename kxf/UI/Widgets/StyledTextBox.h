#pragma once
#include "Common.h"
#include "../IStyledTextWidget.h"

namespace kxf::WXUI
{
	class StyledTextBox;
}

namespace kxf::Widgets
{
	class KXF_API StyledTextBox: public RTTI::Implementation<StyledTextBox, Private::BasicWxWidget<StyledTextBox, WXUI::StyledTextBox, IStyledTextWidget>>
	{
		public:
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		private:
			size_t m_LengthLimit = npos;

		public:
			StyledTextBox();
			~StyledTextBox();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// ITextEntry
			bool CanCut() const override;
			bool CanCopy() const override;
			bool CanPaste() const override;
			bool CanUndo() const override;
			bool CanRedo() const override;

			void Cut() override;
			void Copy() override;
			void Paste() override;
			void Undo() override;
			void Redo() override;

			void ClearText() override;
			bool IsTextEmpty() const override;

			bool IsEditable() const override;
			void SetEditable(bool isEditable = true) override;

			bool IsMultiline() const override;
			void SetMultiline(bool isMultiline = true) override;

			bool IsModified() const override;
			void SetModified(bool isModified = true) override;

			size_t GetLengthLimit() const override;
			void SetLengthLimit(size_t limit) override;

			size_t GetTabWidth() const override;
			void SetTabWidth(size_t width) override;

			size_t GetInsertionPoint() const override;
			void SetInsertionPoint(size_t pos) override;

			void SelectRange(size_t from, size_t to) override;
			std::pair<size_t, size_t> GetSelectionRange() const override;
			String GetRange(size_t from, size_t to) const override;
			void RemoveRange(size_t from, size_t to) override;
			void ReplaceRange(size_t from, size_t to, const String& text) override;

			size_t GetTextLength() const override;
			String GetText() const override;
			void SetText(const String& text) override;
			void AppendText(const String& text) override;

			String GetHint() const override;
			void SetHint(const String& hint) override;

			size_t GetRowCount() const override;
			size_t GetColumnCount() const override;
			size_t GetRowLength(size_t index) const override;

			UniChar GetPasswordMask() const;
			void SetPasswordMask(const UniChar& c) const;
			void RemovePasswordMask() const;
			using ITextEntry::SetPasswordMask;

			// ITextWidget
			void EnsurePositionVisible(size_t pos) override;
			Point MapPositionToCoordinates(size_t pos) const override;
			size_t MapCoordinatesToPosition(const Point& point) const override;

			Point MapPositionToRowColumn(size_t pos) const override;
			size_t MapRowColumnToPosition(const Point& rowColumn) const override;

			TextWrapMode GetWrapMode() const override;
			void SetWrapMode(TextWrapMode wrapMode) override;

			// IStyledTextWidget
			bool IsIndentationGuidesVisible() const override;
			void SetIndentationGuidesVisible(bool isVisible = true) override;
	};
}
