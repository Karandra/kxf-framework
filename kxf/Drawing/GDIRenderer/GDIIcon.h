#pragma once
#include "Common.h"
#include "IGDIObject.h"
#include "../IImage2D.h"
#include <wx/icon.h>

namespace kxf
{
	class KXF_API GDIIcon: public RTTI::DynamicImplementation<GDIIcon, IGDIObject, IImage2D>
	{
		kxf_RTTI_DeclareIID(GDIIcon, {0x5da6784, 0xf20c, 0x4ad6, {0xb1, 0x3b, 0x3a, 0xa1, 0x2d, 0xd9, 0x66, 0x4a}});

		private:
			wxIcon m_Icon;

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

		public:
			GDIIcon() = default;
			GDIIcon(const wxIcon& other)
				:m_Icon(other)
			{
			}

			GDIIcon(const BitmapImage& other);
			GDIIcon(const GDIBitmap& other);
			GDIIcon(const GDICursor& other);
			GDIIcon(const GDIIcon& other)
				:m_Icon(other.m_Icon)
			{
			}

			GDIIcon(const char* xbm, const Size& size)
				:m_Icon(xbm, size.GetWidth(), size.GetHeight())
			{
			}

			virtual ~GDIIcon() = default;

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_Icon.IsOk();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				return m_Icon.GetHandle() == other.GetHandle();
			}
			std::unique_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_unique<GDIIcon>(m_Icon);
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
				else if (auto icon = other.QueryInterface<GDIIcon>())
				{
					return m_Icon.IsSameAs(icon->m_Icon);
				}
				return false;
			}
			std::unique_ptr<IImage2D> CloneImage2D() const override
			{
				return std::make_unique<GDIIcon>(m_Icon);
			}

			Size GetSize() const override
			{
				return m_Icon.IsOk() ? Size(m_Icon.GetSize()) : Size::UnspecifiedSize();
			}
			ColorDepth GetColorDepth() const override
			{
				return m_Icon.GetDepth();
			}
			UniversallyUniqueID GetFormat() const override
			{
				return ImageFormat::ICO;
			}

			void Create(const Size& size) override;
			bool Load(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any, size_t index = npos);
			bool Save(IOutputStream& stream, const UniversallyUniqueID& format) const;

			BitmapImage ToBitmapImage(const Size& size = Size::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const override;

			// Icon
			const wxIcon& ToWxIcon() const noexcept
			{
				return m_Icon;
			}
			wxIcon& ToWxIcon() noexcept
			{
				return m_Icon;
			}
			GDICursor ToGDICursor(const Point& hotSpot = Point::UnspecifiedPosition()) const;
			GDIBitmap ToGDIBitmap(const Size& size = Size::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const;

			GDIIcon ConvertToDisabled(Angle brightness = Angle::FromNormalized(1)) const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			GDIIcon& operator=(const GDIIcon& other)
			{
				m_Icon = other.m_Icon;

				return *this;
			}
	};
}
