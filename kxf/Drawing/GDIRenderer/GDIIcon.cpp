#include "kxf-pch.h"
#include "GDIIcon.h"
#include "GDICursor.h"
#include "GDIBitmap.h"
#include "../BitmapImage.h"
#include "Private/GDI.h"

namespace kxf
{
	// Icon
	GDIIcon::GDIIcon(const GDICursor& other)
		:m_Icon(std::move(other.ToGDIIcon().m_Icon))
	{
	}
	GDIIcon::GDIIcon(const BitmapImage& other)
		:m_Icon(std::move(other.ToGDIIcon().m_Icon))
	{
	}
	GDIIcon::GDIIcon(const GDIBitmap& other)
		:m_Icon(std::move(other.ToGDIIcon().m_Icon))
	{
	}

	// IGDIObject
	void* GDIIcon::GetHandle() const
	{
		return m_Icon.GetHandle();
	}
	void* GDIIcon::DetachHandle()
	{
		return Drawing::Private::DetachGDIImageHandle(m_Icon);
	}
	void GDIIcon::AttachHandle(void* handle)
	{
		if (handle)
		{
			m_Icon.CreateFromHICON(reinterpret_cast<WXHICON>(handle));
			Drawing::Private::AttachIconHandle(m_Icon, handle, []()
			{
				return true;
			});
		}
		else
		{
			m_Icon = wxIcon();
		}
	}

	void GDIIcon::Create(const Size& size)
	{
		BitmapImage image(size);
		m_Icon = std::move(image.ToGDIIcon().m_Icon);
	}

	// IImage2D
	bool GDIIcon::Load(IInputStream& stream, const UniversallyUniqueID& format, size_t index)
	{
		BitmapImage image;
		if (image.Load(stream, format, index == IImage2D::npos ? -1 : static_cast<int>(index)))
		{
			m_Icon = std::move(image.ToGDIIcon().m_Icon);
			return m_Icon.IsOk();
		}
		return false;
	}
	bool GDIIcon::Save(IOutputStream& stream, const UniversallyUniqueID& format) const
	{
		if (m_Icon.IsOk() && format != ImageFormat::Any && format != ImageFormat::None)
		{
			return ToBitmapImage().Save(stream, format);
		}
		return false;
	}

	BitmapImage GDIIcon::ToBitmapImage(const Size& size, InterpolationQuality interpolationQuality) const
	{
		return GDIIcon::ToGDIBitmap(size, interpolationQuality).ToBitmapImage(Size::UnspecifiedSize(), InterpolationQuality::None);
	}

	// Icon
	GDICursor GDIIcon::ToGDICursor(const Point& hotSpot) const
	{
		GDICursor cursor(ToGDIBitmap());
		cursor.SetHotSpot(hotSpot);

		return cursor;
	}
	GDIBitmap GDIIcon::ToGDIBitmap(const Size& size, InterpolationQuality interpolationQuality) const
	{
		if (m_Icon.IsOk())
		{
			wxBitmap bitmap(m_Icon, wxBitmapTransparency::wxBitmapTransparency_Always);
			if (!size.IsFullySpecified() || m_Icon.GetSize() == size)
			{
				return bitmap;
			}

			BitmapImage image = GDIBitmap(bitmap);
			image.Rescale(size, interpolationQuality);
			return image.ToGDIBitmap();
		}
		return {};
	}

	GDIIcon GDIIcon::ConvertToDisabled(Angle brightness) const
	{
		return ToGDIBitmap().ConvertToDisabled(brightness);
	}
}
