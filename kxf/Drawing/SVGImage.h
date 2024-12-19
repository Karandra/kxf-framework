#pragma once
#include "Common.h"
#include "IVectorImage.h"
#include "kxf/Serialization/BinarySerializer.h"

namespace lunasvg
{
	class SVGDocument;
}

namespace kxf
{
	class KXF_API SVGImage final: public RTTI::DynamicImplementation<SVGImage, IVectorImage>
	{
		kxf_RTTI_DeclareIID_Using(SVGImage, ImageFormat::SVG.ToNativeUUID());
		
		friend struct BinarySerializer<SVGImage>;

		private:
			std::shared_ptr<lunasvg::SVGDocument> m_Document;
			int m_DPI = -1;

		private:
			void AllocExclusive();

		public:
			SVGImage() = default;
			SVGImage(const SVGImage& other)
			{
				*this = other;
			}
			SVGImage(SVGImage&& other)
			{
				*this = std::move(other);
			}
			~SVGImage() = default;

		public:
			// IImage2D
			bool IsNull() const
			{
				return m_Document == nullptr;
			}
			bool IsSameAs(const IImage2D& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto svg = other.QueryInterface<SVGImage>())
				{
					return m_Document == svg->m_Document;
				}
				return false;
			}
			std::shared_ptr<IImage2D> CloneImage2D() const override;

			// IImage2D: Create, save and load
			void Create(const Size& size);
			bool Load(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any, size_t index = npos);
			bool Save(IOutputStream& stream, const UniversallyUniqueID& format) const;

			// IImage2D: Properties
			Size GetSize() const override;
			ColorDepth GetColorDepth() const override
			{
				return ColorDepthDB::BPP32;
			}
			UniversallyUniqueID GetFormat() const override
			{
				return ImageFormat::SVG;
			}

			// IImage2D: Options
			std::optional<String> GetOption(const String& name) const override
			{
				return {};
			}
			std::optional<int> GetOptionInt(const String& name) const override;

			void SetOption(const String& name, int value) override;
			void SetOption(const String& name, const String& value) override
			{
			}

			// IImage2D: Conversion
			BitmapImage ToBitmapImage(const Size& size = Size::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const override;

		public:
			// IVectorImage
			Rect GetBoundingBox() const override;

		public:
			SVGImage& operator=(const SVGImage& other);
			SVGImage& operator=(SVGImage&& other) noexcept;
	};
}

namespace kxf
{
	template<>
	struct KXF_API BinarySerializer<SVGImage> final
	{
		uint64_t Serialize(IOutputStream& stream, const SVGImage& value) const;
		uint64_t Deserialize(IInputStream& stream, SVGImage& value) const;
	};
}
