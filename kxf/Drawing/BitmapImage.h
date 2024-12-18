#pragma once
#include "Common.h"
#include "IBitmapImage.h"
#include "kxf/Serialization/BinarySerializer.h"
class wxImage;
class wxMemoryBuffer;

namespace kxf
{
	class GDICursor;
	class GDIBitmap;
	class GDIIcon;
}

namespace kxf
{
	class KXF_API BitmapImage final: public RTTI::DynamicImplementation<BitmapImage, IBitmapImage>
	{
		kxf_RTTI_DeclareIID(BitmapImage, {0x84c3ee9b, 0x62dc, 0x4d8c, {0x8c, 0x94, 0xd1, 0xbc, 0xaf, 0x68, 0xfb, 0xc1}});

		public:
			static size_t GetImageCount(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any);

		private:
			std::unique_ptr<wxImage> m_Image;

		public:
			BitmapImage() = default;
			BitmapImage(const wxImage& other);
			BitmapImage(const BitmapImage& other);
			BitmapImage(BitmapImage&&) noexcept = default;

			BitmapImage(const GDIIcon& other);
			BitmapImage(const GDIBitmap& other);
			BitmapImage(const GDICursor& other);

			BitmapImage(const Size& size);
			BitmapImage(const Size& size, uint8_t* rgb);
			BitmapImage(const Size& size, uint8_t* rgb, uint8_t* alpha);
			BitmapImage(const Size& size, wxMemoryBuffer& rgb);
			BitmapImage(const Size& size, wxMemoryBuffer& rgb, wxMemoryBuffer& alpha);

			~BitmapImage();

		public:
			// IImage2D
			bool IsNull() const;
			bool IsSameAs(const IImage2D& other) const override;
			std::unique_ptr<IImage2D> CloneImage2D() const override;

			// IImage2D: Create, save and load
			void Create(const Size& size);
			bool Load(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any, size_t index = npos);
			bool Save(IOutputStream& stream, const UniversallyUniqueID& format) const;

			// IImage2D: Properties
			Size GetSize() const override;
			int GetWidth() const override;
			int GetHeight() const override;
			ColorDepth GetColorDepth() const override;
			UniversallyUniqueID GetFormat() const override;

			// IImage2D: Options
			std::optional<String> GetOption(const String& name) const override;
			std::optional<int> GetOptionInt(const String& name) const override;

			void SetOption(const String& name, int value) override;
			void SetOption(const String& name, const String& value) override;

			// IImage2D: Conversion
			BitmapImage ToBitmapImage(const Size& size = Size::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const override;

		public:
			// IBitmapImage: Pixel data
			std::vector<PackedRGBA<uint8_t>> GetPixelDataRGBA() const override;
			std::vector<PackedRGB<uint8_t>> GetPixelDataRGB() const override;
			std::vector<uint8_t> GetPixelDataAlpha() const override;

			void SetPixelDataRGBA(const PackedRGBA<uint8_t>* pixelData) override;
			void SetPixelDataRGB(const PackedRGB<uint8_t>* pixelDataRGB) override;
			void SetPixelDataAlpha(const uint8_t* pixelDataAlpha) override;

			void ClearPixelDataRGBA(uint8_t value = 0) override;
			void ClearPixelDataRGB(uint8_t value = 0) override;
			void ClearPixelDataAlpha(uint8_t value = 0)override;

			PackedRGBA<uint8_t> GetPixelRGBA(const Point& pos) const override;
			PackedRGB<uint8_t> GetPixelRGB(const Point& pos) const override;

			void SetPixelRGBA(const Point& pos, const PackedRGBA<uint8_t>& color) override;
			void SetPixelRGB(const Point& pos, const PackedRGB<uint8_t>& color) override;
			void SetAreaRGBA(const Rect& rect, const PackedRGBA<uint8_t>& color) override;
			void SetAreaRGB(const Rect& rect, const PackedRGB<uint8_t>& color) override;

			void ReplaceRGB(const PackedRGB<uint8_t>& source, const PackedRGB<uint8_t>& target) override;
			void ReplaceRGBA(const PackedRGBA<uint8_t>& source, const PackedRGBA<uint8_t>& target) override;

			// IBitmapImage: Transparency
			bool IsPartiallyTransparent() const override;
			bool IsPixelTransparent(const Point& pos, uint8_t threshold = 128) const override;

		public:
			// BitmapImage
			void SetFormat(const UniversallyUniqueID& format);
			bool IsSameAs(const BitmapImage& other) const;
			BitmapImage Clone() const;

			// BitmapImage: Conversion
			const wxImage& ToWxImage() const noexcept;
			wxImage& ToWxImage() noexcept;

			GDICursor ToGDICursor(const Point& hotSpot = Point::UnspecifiedPosition()) const;
			GDIIcon ToGDIIcon() const;

			// BitmapImage: Pixel data
			const uint8_t* GetRawData() const;
			uint8_t* GetRawData();
			void ClearRawData(uint8_t value = 0);

			void SetRawData(wxMemoryBuffer& alpha);
			void SetRawData(uint8_t* alpha);
			void SetRawData(const Size& size, wxMemoryBuffer& alpha);
			void SetRawData(const Size& size, uint8_t* alpha);

			bool HasAlpha() const;
			bool InitAlpha();
			bool ClearAlpha();

			const uint8_t* GetRawAlpha() const;
			uint8_t* GetRawAlpha();

			uint8_t GetRed(const Point& pos) const;
			uint8_t GetGreen(const Point& pos) const;
			uint8_t GetBlue(const Point& pos) const;
			uint8_t GetAlpha(const Point& pos) const;

			// BitmapImage: Transformation
			BitmapImage GetSubImage(const Rect& rect) const;

			BitmapImage Blur(int radius, Orientation orientation = Orientation::Both) const;
			BitmapImage Paste(const BitmapImage& image, const Point& pos, CompositionMode compositionMode = CompositionMode::Dest);
			BitmapImage Mirror(Orientation orientation) const;
			BitmapImage Rotate(Angle angle, const Point& rotationCenter, InterpolationQuality interpolationQuality) const;

			BitmapImage& Resize(const Size& size, const Point& pos, const PackedRGB<uint8_t>& backgroundColor);
			BitmapImage& Rescale(const Size& size, InterpolationQuality interpolationQuality);

			// BitmapImage: Conversion
			GDIBitmap ToGDIBitmap(const Size& size = Size::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const;
			BitmapImage ConvertToDisabled(Angle brightness = Angle::FromNormalized(1)) const;
			BitmapImage ConvertToMonochrome(const PackedRGB<uint8_t>& makeWhite) const;
			BitmapImage ConvertToGrayscale(const PackedRGB<float>& weight = ColorWeight::CCIR_601) const;

			// BitmapImage: Misc
			std::optional<PackedRGB<uint8_t>> FindFirstUnusedColour(const PackedRGB<uint8_t>& startAt = {1, 0, 0}) const;
			BitmapImage& RotateHue(Angle angle);

			// BitmapImage: Mask
			bool IsMaskEnabled() const;
			void EnableMask(bool enable = true);

			PackedRGB<uint8_t> GetMask() const;
			void SetMask(const PackedRGB<uint8_t>& color);
			void SetMask(const Color& color);
			bool SetMask(const BitmapImage& shape, const PackedRGB<uint8_t>& color);
			bool SetMask(const BitmapImage& shape, const Color& color);

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			BitmapImage& operator=(const BitmapImage& other);
			BitmapImage& operator=(BitmapImage&& other) noexcept = default;
	};
}

namespace kxf
{
	template<>
	struct KXF_API BinarySerializer<BitmapImage> final
	{
		uint64_t Serialize(IOutputStream& stream, const BitmapImage& value) const;
		uint64_t Deserialize(IInputStream& stream, BitmapImage& value) const;
	};
}
