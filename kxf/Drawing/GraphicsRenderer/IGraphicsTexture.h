#pragma once
#include "Common.h"
#include "IGraphicsObject.h"
#include "../ImageDefines.h"

namespace kxf
{
	class BitmapImage;
	class UniversallyUniqueID;

	class IInputStream;
	class IOutputStream;
}

namespace kxf
{
	class KXF_API IGraphicsTexture: public RTTI::ExtendInterface<IGraphicsTexture, IGraphicsObject>
	{
		kxf_RTTI_DeclareIID(IGraphicsTexture, {0xa7852135, 0x2bf0, 0x4737, {0xa4, 0xfb, 0xc9, 0x99, 0xc7, 0xa2, 0x57, 0xd3}});

		public:
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		public:
			virtual ~IGraphicsTexture() = default;

		public:
			virtual SizeF GetDPI() const = 0;
			virtual SizeF GetSize() const = 0;
			virtual float GetWidth() const = 0;
			virtual float GetHeight() const = 0;
			virtual ColorDepth GetColorDepth() const = 0;

			virtual bool Load(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any, size_t index = npos) = 0;
			virtual bool Save(IOutputStream& stream, const UniversallyUniqueID& format) const = 0;

			virtual std::shared_ptr<IGraphicsTexture> GetSubTexture(const RectF& rect) const = 0;
			virtual void Rescale(const SizeF& size, InterpolationQuality interpolationQuality) = 0;

			virtual BitmapImage ToBitmapImage(const SizeF& size = SizeF::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const = 0;
			virtual bool FromBitmapImage(const BitmapImage& image) = 0;
	};
}
