#pragma once
#include "Common.h"
#include "IGDIObject.h"
#include "GDIBitmap.h"
#include "GDIIcon.h"
#include "../BitmapImage.h"
#include "kxf/System/COM.h"
#include <wx/imaglist.h>
struct IImageList2;

namespace kxf
{
	class GDIContext;

	enum class ImageListFlag: uint32_t
	{
		None = 0,

		Transparent = 1 << 0,
		Selected = 1 << 1,
		Focused = 1 << 2,
	};
	kxf_FlagSet_Declare(ImageListFlag);
}

namespace kxf
{
	class KXF_API GDIImageList: public wxImageList, public RTTI::ExtendInterface<GDIImageList, IGDIObject>
	{
		kxf_RTTI_DeclareIID(GDIImageList, {0x50e3b888, 0xefda, 0x4d73, {0x88, 0x58, 0x1, 0xde, 0xf, 0xf8, 0xc8, 0xaf}});

		protected:
			uint32_t m_Flags = 0;

		private:
			void OnCreate(int width, int height, bool mask, int initialCount) noexcept;
			bool DoDraw(GDIContext& canvas, int index, const Rect& rect, FlagSet<ImageListFlag> flags = {}, int overlayIndex = Drawing::InvalidImageIndex) noexcept;

		public:
			GDIImageList() noexcept;
			GDIImageList(int width, int height, int initialCount = 1) noexcept;
			GDIImageList(const Size& size, int initialCount = 1) noexcept
				:GDIImageList(size.GetWidth(), size.GetWidth(), initialCount)
			{
			}
			GDIImageList(const GDIImageList&) = delete;
			GDIImageList(GDIImageList&& other) noexcept
			{
				*this = std::move(other);
			}

		public:
			// IGDIObject
			bool IsNull() const override;
			bool IsSameAs(const IGDIObject& other) const override;
			std::shared_ptr<IGDIObject> CloneGDIObject() const override;

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// GDIImageList
			bool HasMask() const noexcept;
			COMPtr<IImageList2> QueryNativeInterface() const noexcept;

			size_t GetImageCount() const noexcept
			{
				return static_cast<size_t>(wxImageList::GetImageCount());
			}

			bool Create(int width, int height, int initialCount = 1) noexcept;
			bool Create(const Size& size, int initialCount = 1) noexcept;

			bool Clear() noexcept;
			bool RemoveAll() noexcept;
			bool Remove(int index) noexcept;

			int Add(const GDIBitmap& bitmap);
			int Add(const GDIIcon& icon);
			int Add(const BitmapImage& image);

			bool Replace(int index, const BitmapImage& image);
			bool Replace(int index, const GDIIcon& icon);
			bool Replace(int index, const GDIBitmap& bitmap);

			BitmapImage GetImage(int index) const;
			GDIIcon GetIcon(int index) const;
			GDIBitmap GetBitmap(int index) const;

			Color GetBackgroundColor() const noexcept;
			void SetBackgroundColor(const Color& color) noexcept;
			bool SetOverlayImage(int index, int overlayIndex) noexcept;

			bool Draw(GDIContext& dc, int index, const Point& point, FlagSet<ImageListFlag> flags = {}) noexcept
			{
				return DoDraw(dc, index, Rect(point, Size::UnspecifiedSize()), flags);
			}
			bool Draw(GDIContext& dc, int index, const Rect& rect, FlagSet<ImageListFlag> flags = {}) noexcept
			{
				return DoDraw(dc, index, rect, flags);
			}

			bool DrawOverlay(GDIContext& dc, int index, int overlayIndex, const Point& point, FlagSet<ImageListFlag> flags = {}) noexcept
			{
				return DoDraw(dc, index, Rect(point, Size::UnspecifiedSize()), flags, overlayIndex);
			}
			bool DrawOverlay(GDIContext& dc, int index, int overlayIndex, const Rect& rect, FlagSet<ImageListFlag> flags = {}) noexcept
			{
				return DoDraw(dc, index, rect, flags, overlayIndex);
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			GDIImageList& operator=(const GDIImageList&) = delete;
			GDIImageList& operator=(GDIImageList&& other) noexcept;

		public:
			wxDECLARE_DYNAMIC_CLASS(GDIImageList);
	};
}
