#pragma once
#include "Common.h"
#include "IWidget.h"

namespace kxf
{
	enum class TextWrapMode
	{
		None = 0,
		Character,
		Whitespace,
		Word,
	};
}

namespace kxf
{
	class KXF_API ITextEntry: public RTTI::Interface<ITextEntry>
	{
		kxf_RTTI_DeclareIID(ITextEntry, {0x42e05214, 0x6cf3, 0x4fd5, {0x95, 0x53, 0x20, 0xc7, 0xdc, 0x95, 0xa, 0x9}});

		public:
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		public:
			virtual ~ITextEntry() = default;

		public:
			virtual bool CanCut() const = 0;
			virtual bool CanCopy() const = 0;
			virtual bool CanPaste() const = 0;
			virtual bool CanUndo() const = 0;
			virtual bool CanRedo() const = 0;

			virtual void Cut() = 0;
			virtual void Copy() = 0;
			virtual void Paste() = 0;
			virtual void Undo() = 0;
			virtual void Redo() = 0;

			virtual void ClearText() = 0;
			virtual bool IsTextEmpty() const = 0;

			virtual bool IsEditable() const = 0;
			virtual void SetEditable(bool isEditable = true) = 0;

			virtual bool IsMultiline() const = 0;
			virtual void SetMultiline(bool isMultiline = true) = 0;

			virtual bool IsModified() const = 0;
			virtual void SetModified(bool isModified = true) = 0;

			virtual size_t GetLengthLimit() const = 0;
			virtual void SetLengthLimit(size_t limit) = 0;

			virtual size_t GetTabWidth() const = 0;
			virtual void SetTabWidth(size_t width) = 0;

			virtual size_t GetInsertionPoint() const = 0;
			virtual void SetInsertionPoint(size_t pos) = 0;

			virtual void SelectRange(size_t from, size_t to) = 0;
			void SelectNone()
			{
				SelectRange(0, 0);
			}
			void SelectAll()
			{
				SelectRange(0, npos);
			}

			virtual std::pair<size_t, size_t> GetSelectionRange() const = 0;
			virtual String GetRange(size_t from, size_t to) const = 0;
			virtual void RemoveRange(size_t from, size_t to) = 0;
			virtual void ReplaceRange(size_t from, size_t to, const String& text) = 0;

			bool IsSelectionRangeEmpty() const
			{
				auto [from, to] = GetSelectionRange();
				return from == to || from == npos;
			}
			void RemoveSelectedRange()
			{
				auto [from, to] = GetSelectionRange();
				RemoveRange(from, to);
			}
			void ReplaceSelectedRange(const String& text)
			{
				auto [from, to] = GetSelectionRange();
				ReplaceRange(from, to, text);
			}
			String GetSelectedRange() const
			{
				auto [from, to] = GetSelectionRange();
				return GetRange(from, to);
			}

			virtual size_t GetTextLength() const = 0;
			virtual String GetText() const = 0;
			virtual void SetText(const String& text) = 0;
			virtual void AppendText(const String& text) = 0;

			virtual String GetHint() const = 0;
			virtual void SetHint(const String& hint) = 0;

			virtual size_t GetRowCount() const = 0;
			virtual size_t GetColumnCount() const = 0;
			virtual size_t GetRowLength(size_t index) const = 0;

			virtual UniChar GetPasswordMask() const = 0;
			virtual void SetPasswordMask(const UniChar& c) const = 0;
			void SetPasswordMask() const
			{
				SetPasswordMask(0x25CF);
			}
			virtual void RemovePasswordMask() const = 0;

	};
}

namespace kxf
{
	class KXF_API ITextWidget: public RTTI::ExtendInterface<ITextWidget, IWidget, ITextEntry>
	{
		kxf_RTTI_DeclareIID(ITextWidget, {0x711b7c3, 0x54cd, 0x4147, {0x9b, 0xdd, 0x2, 0x8, 0x65, 0x6f, 0x61, 0x1}});

		public:
			virtual ~ITextWidget() = default;

		public:
			virtual void EnsurePositionVisible(size_t pos) = 0;
			virtual Point MapPositionToCoordinates(size_t pos) const = 0;
			virtual size_t MapCoordinatesToPosition(const Point& point) const = 0;

			virtual Point MapPositionToRowColumn(size_t pos) const = 0;
			virtual size_t MapRowColumnToPosition(const Point& rowColumn) const = 0;

			virtual TextWrapMode GetWrapMode() const = 0;
			virtual void SetWrapMode(TextWrapMode wrapMode) = 0;

			virtual Size GetTextExtent() const = 0;
			virtual Size GetTextExtent(const String& text) const = 0;
	};
}
