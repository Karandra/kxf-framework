#pragma once
#include "kxf/UI/Common.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include "kxf/EventSystem/Event.h"
#include "kxf/IO/IStream.h"
#include <wx/systhemectrl.h>
#include <wx/vscroll.h>

namespace kxf::UI
{
	class KXF_API ThumbViewItem final
	{
		private:
			GDIBitmap m_Bitmap;

		public:
			ThumbViewItem(const GDIBitmap& bitmap)
				:m_Bitmap(bitmap)
			{
			}

		public:
			const GDIBitmap& GetBitmap() const
			{
				return m_Bitmap;
			}
			GDIBitmap& GetBitmap()
			{
				return m_Bitmap;
			}
	};
}

namespace kxf::UI
{
	class KXF_API ThumbView: public wxSystemThemedControl<WindowRefreshScheduler<wxVScrolledWindow>>
	{
		public:
			static constexpr FlagSet<WindowStyle> DefaultStyle = WindowStyle::None;
			static inline const Size DefaultThumbSize = Size(256, 144);
			static constexpr double ThumbPaddingScale = 0.9;

			kxf_EVENT_MEMBER(wxCommandEvent, Selected);
			kxf_EVENT_MEMBER(wxCommandEvent, Activated);
			kxf_EVENT_MEMBER(wxContextMenuEvent, ContextMenu);

		private:
			static constexpr size_t InvalidItemIndex = std::numeric_limits<size_t>::max();

		private:
			Size m_ThumbSize = Size::UnspecifiedSize();
			Size m_Spacing = Size(1, 1);
			size_t m_Focused = InvalidItemIndex;
			size_t m_Selected = InvalidItemIndex;
			std::vector<ThumbViewItem> m_Items;

		private:
			void OnPaint(wxPaintEvent& event);
			void OnSize(wxSizeEvent& event);
			void OnMouse(wxMouseEvent& event);
			void OnKillFocus(wxFocusEvent& event);

			size_t GetIndexByRowColumn(size_t row, size_t columnIndex, size_t itemsInRow) const;
			Rect GetThumbRect(size_t row, size_t columnIndex, size_t beginRow);
			Rect GetFullThumbRect(size_t row, size_t columnIndex, size_t beginRow);
			Size GetFinalThumbSize() const;
			size_t CalcItemsPerRow() const;

			int OnGetRowHeight(size_t i) const override
			{
				return GetFinalThumbSize().GetHeight();
			}
			size_t CalcRowCount() const;
			void UpdateRowCount();
			ThumbViewItem& GetThumb(size_t i);
			GDIBitmap CreateThumb(const GDIBitmap& bitmap, const Size& size) const;

			void OnInternalIdle() override;

		public:
			ThumbView() = default;
			ThumbView(wxWindow* parent,
					  wxWindowID id,
					  FlagSet<WindowStyle> style = DefaultStyle
			)
			{
				Create(parent, id, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						FlagSet<WindowStyle> style = DefaultStyle
			);

		public:
			wxBorder GetDefaultBorder() const override
			{
				return wxBORDER_THEME;
			}
			bool ShouldInheritColours() const override
			{
				return true;
			}

			Size GetThumbSize() const;
			void SetThumbSize(const Size& size);
			Size GetSpacing() const;
			void SetSpacing(const Size& spacing);

			int GetSelectedThumb() const;
			void SetSelectedThumb(int index);

			size_t GetThumbsCount() const;
			size_t AddThumb(const GDIBitmap& bitmap);
			size_t AddThumb(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any, size_t index = IImage2D::npos);
			void RemoveThumb(size_t index);
			void ClearThumbs();

		public:
			wxDECLARE_DYNAMIC_CLASS(ThumbView);
	};
}
