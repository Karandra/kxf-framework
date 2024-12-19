#pragma once
#include "../Common.h"
#include "../IGraphicsTexture.h"
#include "kxf/Drawing/BitmapImage.h"

namespace kxf::Drawing
{
	class KXF_API NullGraphicsTexture final: public IGraphicsTexture
	{
		public:
			~NullGraphicsTexture() = default;

		public:
			// IGraphicsObject
			bool IsNull() const override
			{
				return true;
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				return other.IsNull();
			}
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return nullptr;
			}

			IGraphicsRenderer& GetRenderer() override
			{
				return Drawing::Private::GetNullGraphicsRenderer();
			}
			void* GetNativeHandle() const
			{
				return nullptr;
			}

			// IGraphicsTexture
			SizeF GetDPI() const override
			{
				return SizeF::UnspecifiedSize();
			}
			SizeF GetSize() const override
			{
				return SizeF::UnspecifiedSize();
			}
			float GetWidth() const override
			{
				return Geometry::DefaultCoord;
			}
			float GetHeight() const override
			{
				return Geometry::DefaultCoord;
			}
			ColorDepth GetColorDepth() const override
			{
				return {};
			}

			bool Load(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any, size_t index = npos) override
			{
				return false;
			}
			bool Save(IOutputStream& stream, const UniversallyUniqueID& format) const override
			{
				return false;
			}

			std::shared_ptr<IGraphicsTexture> GetSubTexture(const RectF& rect) const override
			{
				return nullptr;
			}
			void Rescale(const SizeF& size, InterpolationQuality interpolationQuality) override
			{
			}

			BitmapImage ToBitmapImage(const SizeF& size = SizeF::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const override
			{
				return {};
			}
			bool FromBitmapImage(const BitmapImage& image) override
			{
				return false;
			}
	};
}
namespace kxf
{
	inline const Drawing::NullGraphicsTexture NullGraphicsTexture;
}
