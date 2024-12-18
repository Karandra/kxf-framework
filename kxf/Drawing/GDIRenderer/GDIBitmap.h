#pragma once
#include "Common.h"
#include "IGDIObject.h"
#include "../IImage2D.h"
#include <wx/bitmap.h>

namespace kxf
{
	class GDIContext;
}

namespace kxf
{
	class KXF_API GDIBitmap: public RTTI::DynamicImplementation<GDIBitmap, IGDIObject, IImage2D>
	{
		kxf_RTTI_DeclareIID(GDIBitmap, {0xd4c3e7be, 0xf0fd, 0x4c38, {0xa1, 0x94, 0x16, 0xb3, 0x9, 0xa, 0x34, 0xb5}});

		private:
			wxBitmap m_Bitmap;

		private:
			std::optional<String> GetOption(const String& name) const override
			{
				return {};
			}
			std::optional<int> GetOptionInt(const String& name) const override
			{
				return {};
			}

			void SetOption(const String& name, int value) override
			{
			}
			void SetOption(const String& name, const String& value) override
			{
			}

		private:
			void Initialize();

		public:
			GDIBitmap()
			{
				Initialize();
			}
			GDIBitmap(const wxBitmap& other)
				:m_Bitmap(other)
			{
				Initialize();
			}

			GDIBitmap(const GDIIcon& other);
			GDIBitmap(const BitmapImage& other);
			GDIBitmap(const BitmapImage& other, const GDIContext& dc);
			GDIBitmap(const GDICursor& other);
			GDIBitmap(const GDIBitmap& other)
				:m_Bitmap(other.m_Bitmap)
			{
				Initialize();
			}

			GDIBitmap(const char* xbm, const Size& size, ColorDepth depth = ColorDepthDB::BPP1)
				:m_Bitmap(xbm, size.GetWidth(), size.GetHeight(), depth ? depth.GetValue() : -1)
			{
				Initialize();
			}
			GDIBitmap(const Size& size, ColorDepth depth = {})
				:m_Bitmap(size.GetWidth(), size.GetHeight(), depth ? depth.GetValue() : -1)
			{
				Initialize();
			}
			GDIBitmap(const Size& size, const GDIContext& dc);

			virtual ~GDIBitmap() = default;

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_Bitmap.IsOk();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				return m_Bitmap.GetHandle() == other.GetHandle();
			}
			std::unique_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_unique<GDIBitmap>(m_Bitmap);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// IImage2D
			bool IsSameAs(const IImage2D& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto bitmap = other.QueryInterface<GDIBitmap>())
				{
					return m_Bitmap.IsSameAs(bitmap->m_Bitmap);
				}
				return false;
			}
			std::unique_ptr<IImage2D> CloneImage2D() const override
			{
				return std::make_unique<GDIBitmap>(m_Bitmap);
			}

			Size GetSize() const override
			{
				return m_Bitmap.IsOk() ? Size(m_Bitmap.GetSize()) : Size::UnspecifiedSize();
			}
			ColorDepth GetColorDepth() const override
			{
				return m_Bitmap.GetDepth();
			}
			UniversallyUniqueID GetFormat() const override
			{
				return ImageFormat::BMP;
			}

			void Create(const Size& size) override
			{
				m_Bitmap.Create(size, *ColorDepthDB::BPP32);
			}
			bool Load(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any, size_t index = npos);
			bool Save(IOutputStream& stream, const UniversallyUniqueID& format) const;

			BitmapImage ToBitmapImage(const Size& size = Size::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const override;

			// GDIBitmap
			const wxBitmap& ToWxBitmap() const noexcept
			{
				return m_Bitmap;
			}
			wxBitmap& ToWxBitmap() noexcept
			{
				return m_Bitmap;
			}

			GDICursor ToGDICursor(const Point& hotSpot = Point::UnspecifiedPosition()) const;
			GDIIcon ToGDIIcon() const;

			GDIBitmap GetSubBitmap(const Rect& rect) const
			{
				return m_Bitmap.GetSubBitmap(rect);
			}
			GDIBitmap ConvertToDisabled(Angle brightness = Angle::FromNormalized(1)) const
			{
				return m_Bitmap.ConvertToDisabled(static_cast<uint8_t>(brightness.ToNormalized() * 255));
			}
			GDIBitmap GetScaled(const Size& size = Size::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const;

			void UpdateAlpha()
			{
				m_Bitmap.MSWUpdateAlpha();
			}
			void ForceAlpha()
			{
				m_Bitmap.UseAlpha(true);
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

			GDIBitmap& operator=(const GDIBitmap& other)
			{
				m_Bitmap = other.m_Bitmap;
				Initialize();

				return *this;
			}
	};
}
