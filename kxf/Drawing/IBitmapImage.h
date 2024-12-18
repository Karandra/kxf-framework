#pragma once
#include "Common.h"
#include "IImage2D.h"

namespace kxf
{
	class KXF_API IBitmapImage: public RTTI::ExtendInterface<IBitmapImage, IImage2D>
	{
		kxf_RTTI_DeclareIID(IImage2D, {0x2c48e1ad, 0xb93e, 0x4047, {0xb4, 0x52, 0x9b, 0x4c, 0x3a, 0x43, 0xeb, 0x77}});

		public:
			virtual ~IBitmapImage() = default;

		public:
			// Pixel data
			virtual std::vector<PackedRGBA<uint8_t>> GetPixelDataRGBA() const = 0;
			virtual std::vector<PackedRGB<uint8_t>> GetPixelDataRGB() const = 0;
			virtual std::vector<uint8_t> GetPixelDataAlpha() const = 0;

			virtual void SetPixelDataRGBA(const PackedRGBA<uint8_t>* pixelData) = 0;
			virtual void SetPixelDataRGB(const PackedRGB<uint8_t>* pixelDataRGB) = 0;
			virtual void SetPixelDataAlpha(const uint8_t* pixelDataAlpha) = 0;

			virtual void ClearPixelDataRGBA(uint8_t value = 0) = 0;
			virtual void ClearPixelDataRGB(uint8_t value = 0) = 0;
			virtual void ClearPixelDataAlpha(uint8_t value = 0) = 0;

			virtual PackedRGBA<uint8_t> GetPixelRGBA(const Point& pos) const = 0;
			virtual PackedRGB<uint8_t> GetPixelRGB(const Point& pos) const = 0;

			virtual void SetPixelRGBA(const Point& pos, const PackedRGBA<uint8_t>& color) = 0;
			virtual void SetPixelRGB(const Point& pos, const PackedRGB<uint8_t>& color) = 0;
			virtual void SetAreaRGBA(const Rect& rect, const PackedRGBA<uint8_t>& color) = 0;
			virtual void SetAreaRGB(const Rect& rect, const PackedRGB<uint8_t>& color) = 0;

			virtual void ReplaceRGB(const PackedRGB<uint8_t>& source, const PackedRGB<uint8_t>& target) = 0;
			virtual void ReplaceRGBA(const PackedRGBA<uint8_t>& source, const PackedRGBA<uint8_t>& target) = 0;

			// Transparency
			virtual bool IsPartiallyTransparent() const = 0;
			virtual bool IsPixelTransparent(const Point& pos, uint8_t threshold = 128) const = 0;
	};
}
