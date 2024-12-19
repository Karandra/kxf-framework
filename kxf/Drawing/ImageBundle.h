#pragma once
#include "Common.h"
#include "BitmapImage.h"
class wxIconBundle;

namespace kxf
{
	enum class ImageBundleFlag: uint32_t
	{
		None = 0,

		SystemSize = 1 << 1,
		SystemSizeSmall = 1 << 2,
		NearestLarger = 1 << 3
	};
	kxf_FlagSet_Declare(ImageBundleFlag);
}

namespace kxf
{
	class KXF_API ImageBundle: public RTTI::Interface<ImageBundle>
	{
		kxf_RTTI_DeclareIID(ImageBundle, {0x8e7461c, 0x1234, 0x49c0, {0xa0, 0x0, 0xe2, 0x16, 0x4a, 0x75, 0x7d, 0x85}});

		private:
			std::vector<BitmapImage> m_Items;

		public:
			ImageBundle() noexcept = default;
			ImageBundle(size_t initialCount)
			{
				m_Items.reserve(initialCount);
			}
			ImageBundle(const wxIconBundle& other);
			ImageBundle(const BitmapImage& other);

			ImageBundle(const ImageBundle&) = default;
			ImageBundle(ImageBundle&&) noexcept = default;
			~ImageBundle() = default;

		public:
			// ImageBundle
			bool IsNull() const;
			bool IsSameAs(const ImageBundle& other) const;

			bool Load(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any, size_t index = BitmapImage::npos);
			bool Save(IOutputStream& stream, const UniversallyUniqueID& format) const;

			void AddImage(const BitmapImage& image);
			BitmapImage GetImage(Size desiredSize, FlagSet<ImageBundleFlag> sizeFallback = ImageBundleFlag::None) const;

			size_t GetImageCount() const
			{
				return m_Items.size();
			}
			BitmapImage GetImageAt(size_t index) const
			{
				if (index < m_Items.size())
				{
					return m_Items[index];
				}
				return {};
			}

			wxIconBundle ToWxIconBundle() const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			ImageBundle& operator=(const ImageBundle&) = default;
			ImageBundle& operator=(ImageBundle&&) noexcept = default;
	};
}
