#include "kxf-pch.h"
#include "BitmapImage.h"
#include "kxf/IO/IStream.h"
#include "kxf/wxWidgets/MapDrawing.h"
#include "kxf/wxWidgets/StreamWrapper.h"
#include <wx/bitmap.h>
#include <wx/image.h>

namespace
{
	constexpr uint8_t g_OpaqueAlpha = kxf::ColorTraits<uint8_t>::max();
	constexpr size_t g_ChannelsRGB = 3;
	constexpr size_t g_ChannelsRGBA = g_ChannelsRGB + 1;

	size_t GetImageBufferSize(const wxImage& image, size_t dimensions = 1) noexcept
	{
		return static_cast<size_t>(image.GetWidth()) * static_cast<size_t>(image.GetHeight()) * dimensions;
	}
	size_t GetImageBufferSizeAuto(const wxImage& image) noexcept
	{
		return GetImageBufferSize(image, image.HasAlpha() ? g_ChannelsRGBA : g_ChannelsRGB);
	}

	constexpr void AssertPackedColorLayout()
	{
		using namespace kxf;

		static_assert(sizeof(PackedRGB<uint8_t>) == 3 && alignof(PackedRGB<uint8_t>) == alignof(uint8_t), "Invalid layout of PackedRGB<uint8_t> structure");
		static_assert(sizeof(PackedRGBA<uint8_t>) == 4 && alignof(PackedRGBA<uint8_t>) == alignof(uint8_t), "Invalid layout of PackedRGBA<uint8_t> structure");
	}
	constexpr wxImageResizeQuality MapInterpolarionQuality(kxf::InterpolationQuality interpolationQuality)
	{
		using namespace kxf;

		switch (interpolationQuality)
		{
			case InterpolationQuality::Default:
			case InterpolationQuality::FastestAvailable:
			{
				return wxIMAGE_QUALITY_NORMAL;
			}
			case InterpolationQuality::BestAvailable:
			{
				return wxIMAGE_QUALITY_HIGH;
			}
			case InterpolationQuality::NearestNeighbor:
			{
				return wxIMAGE_QUALITY_NEAREST;
			}
			case InterpolationQuality::Bilinear:
			{
				return wxIMAGE_QUALITY_BILINEAR;
			}
			case InterpolationQuality::Bicubic:
			{
				return wxIMAGE_QUALITY_BICUBIC;
			}
			case InterpolationQuality::BoxAverage:
			{
				return wxIMAGE_QUALITY_BOX_AVERAGE;
			}
		};
		return wxIMAGE_QUALITY_NORMAL;
	}
}

namespace kxf
{
	// BitmapImage
	size_t BitmapImage::GetImageCount(IInputStream& stream, const UniversallyUniqueID& format)
	{
		wxWidgets::InputStreamWrapperWx warpper(stream);
		return wxImage::GetImageCount(warpper, wxWidgets::MapImageFormat(format));
	}

	BitmapImage::BitmapImage(const wxIcon& other)
	{
		if (other.IsOk())
		{
			wxBitmap bitmap(other, wxBitmapTransparency::wxBitmapTransparency_Always);
			m_Image = std::make_unique<wxImage>(bitmap.ConvertToImage());
		}
	}
	BitmapImage::BitmapImage(const wxImage& other)
		:m_Image(std::make_unique<wxImage>(other))
	{
	}
	BitmapImage::BitmapImage(const wxBitmap& other)
	{
		if (other.IsOk())
		{
			m_Image = std::make_unique<wxImage>(other.ConvertToImage());
		}
	}
	BitmapImage::BitmapImage(const wxCursor& other)
	{
		if (other.IsOk())
		{
			wxBitmap bitmap(other);
			m_Image = std::make_unique<wxImage>(bitmap.ConvertToImage());
		}
	}
	BitmapImage::BitmapImage(const BitmapImage& other)
	{
		if (other)
		{
			m_Image = std::make_unique<wxImage>(*other.m_Image);
		}
	}

	BitmapImage::BitmapImage(const Size& size)
		:m_Image(std::make_unique<wxImage>(size.GetWidth(), size.GetHeight(), false))
	{
	}
	BitmapImage::BitmapImage(const Size& size, uint8_t* rgb)
		:m_Image(std::make_unique<wxImage>(size.GetWidth(), size.GetHeight(), rgb, true))
	{
	}
	BitmapImage::BitmapImage(const Size& size, uint8_t* rgb, uint8_t* alpha)
		:m_Image(std::make_unique<wxImage>(size.GetWidth(), size.GetHeight(), rgb, alpha, true))
	{
	}
	BitmapImage::BitmapImage(const Size& size, wxMemoryBuffer& rgb)
		:m_Image(std::make_unique<wxImage>(size.GetWidth(), size.GetHeight(), static_cast<unsigned char*>(rgb.release()), false))
	{
	}
	BitmapImage::BitmapImage(const Size& size, wxMemoryBuffer& rgb, wxMemoryBuffer& alpha)
		:m_Image(std::make_unique<wxImage>(size.GetWidth(), size.GetHeight(), static_cast<unsigned char*>(rgb.release()), static_cast<unsigned char*>(alpha.release()), false))
	{
	}

	BitmapImage::~BitmapImage() = default;

	// IImage2D
	bool BitmapImage::IsNull() const
	{
		return !m_Image || !m_Image->IsOk();
	}
	bool BitmapImage::IsSameAs(const IImage2D& other) const
	{
		if (this == &other)
		{
			return true;
		}
		else if (auto image = other.QueryInterface<BitmapImage>())
		{
			return m_Image == image->m_Image || m_Image->IsSameAs(*image->m_Image);
		}
		return false;
	}
	std::shared_ptr<IImage2D> BitmapImage::CloneImage2D() const
	{
		if (m_Image)
		{
			return std::make_shared<BitmapImage>(m_Image->Copy());
		}
		return nullptr;
	}

	// IImage2D: Create, save and load
	void BitmapImage::Create(const Size& size)
	{
		m_Image = std::make_unique<wxImage>(size, false);
	}
	bool BitmapImage::Load(IInputStream& stream, const UniversallyUniqueID& format, size_t index)
	{
		if (!m_Image)
		{
			m_Image = std::make_unique<wxImage>();
		}

		wxWidgets::InputStreamWrapperWx warpper(stream);
		return m_Image->LoadFile(warpper, wxWidgets::MapImageFormat(format), index == IImage2D::npos ? -1 : static_cast<int>(index));
	}
	bool BitmapImage::Save(IOutputStream& stream, const UniversallyUniqueID& format) const
	{
		if (m_Image && m_Image->IsOk() && format != ImageFormat::None)
		{
			wxWidgets::OutputStreamWrapperWx warpper(stream);
			if (format == ImageFormat::Any)
			{
				// Save to either original format or as a PNG
				if (auto type = m_Image->GetType(); type != wxBitmapType::wxBITMAP_TYPE_INVALID && wxBitmapType::wxBITMAP_TYPE_ANY)
				{
					return m_Image->SaveFile(warpper, type);
				}
				else
				{
					return m_Image->SaveFile(warpper, wxBitmapType::wxBITMAP_TYPE_PNG);
				}
			}
			else
			{
				return m_Image->SaveFile(warpper, wxWidgets::MapImageFormat(format));
			}
		}
		return false;
	}

	// IImage2D: Properties
	Size BitmapImage::GetSize() const
	{
		return m_Image && m_Image->IsOk() ? Size(m_Image->GetSize()) : Size::UnspecifiedSize();
	}
	int BitmapImage::GetWidth() const
	{
		return m_Image && m_Image->IsOk() ? m_Image->GetWidth() : Size::UnspecifiedSize().GetWidth();
	}
	int BitmapImage::GetHeight() const
	{
		return m_Image && m_Image->IsOk() ? m_Image->GetWidth() : Size::UnspecifiedSize().GetHeight();
	}
	ColorDepth BitmapImage::GetColorDepth() const
	{
		if (m_Image && m_Image->IsOk())
		{
			return m_Image->HasAlpha() ? ColorDepthDB::BPP32 : ColorDepthDB::BPP24;
		}
		return {};
	}
	UniversallyUniqueID BitmapImage::GetFormat() const
	{
		if (m_Image)
		{
			return wxWidgets::MapImageFormat(m_Image->GetType());
		}
		return {};
	}

	// IImage2D: Options
	std::optional<String> BitmapImage::GetOption(const String& name) const
	{
		if (m_Image && m_Image->HasOption(name))
		{
			return m_Image->GetOption(name);
		}
		return {};
	}
	std::optional<int> BitmapImage::GetOptionInt(const String& name) const
	{
		if (m_Image)
		{
			if (m_Image->HasOption(name))
			{
				return m_Image->GetOptionInt(name);
			}
			else if (name == ImageOption::DPI)
			{
				if (m_Image->GetOptionInt(ImageOption::ResolutionUnit) == wxIMAGE_RESOLUTION_NONE)
				{
					if (m_Image->HasOption(ImageOption::Resolution))
					{
						return m_Image->GetOptionInt(ImageOption::Resolution);
					}
					else if (m_Image->HasOption(ImageOption::ResolutionX) || m_Image->GetOptionInt(ImageOption::ResolutionY))
					{
						return std::min(m_Image->GetOptionInt(ImageOption::ResolutionX), m_Image->GetOptionInt(ImageOption::ResolutionY));
					}
				}
			}
		}
		return {};
	}

	void BitmapImage::SetOption(const String& name, int value)
	{
		if (m_Image)
		{
			if (name == ImageOption::DPI)
			{
				m_Image->SetOption(ImageOption::ResolutionUnit, wxIMAGE_RESOLUTION_NONE);
				m_Image->SetOption(ImageOption::Resolution, value);
				m_Image->SetOption(ImageOption::ResolutionX, value);
				m_Image->SetOption(ImageOption::ResolutionY, value);

				return;
			}
			m_Image->SetOption(name, value);
		}
	}
	void BitmapImage::SetOption(const String& name, const String& value)
	{
		if (m_Image)
		{
			m_Image->SetOption(name, value);
		}
	}

	// IImage2D: Conversion
	BitmapImage BitmapImage::ToBitmapImage(const Size& size, InterpolationQuality interpolationQuality) const
	{
		if (m_Image && m_Image->IsOk())
		{
			if (!size.IsFullySpecified() || m_Image->GetSize() == size)
			{
				return *m_Image;
			}
			else
			{
				return m_Image->Scale(size.GetWidth(), size.GetHeight(), MapInterpolarionQuality(interpolationQuality));
			}
		}
		return {};
	}

	// IBitmapImage: Pixel data
	std::vector<PackedRGBA<uint8_t>> BitmapImage::GetPixelDataRGBA() const
	{
		if (m_Image && m_Image->IsOk())
		{
			std::vector<PackedRGBA<uint8_t>> bufferRGBA;
			bufferRGBA.resize(GetImageBufferSize(*m_Image));

			const auto rgb = m_Image->GetData();
			const auto alpha = m_Image->GetAlpha();
			for (size_t i = 0; i < bufferRGBA.size(); i++)
			{
				const auto pixel = rgb + i;
				bufferRGBA[i] = {*pixel, *(pixel + 1), *(pixel + 2), alpha ? *(alpha + i) : g_OpaqueAlpha};
			}

			return bufferRGBA;
		}
		return {};
	}
	std::vector<PackedRGB<uint8_t>> BitmapImage::GetPixelDataRGB() const
	{
		AssertPackedColorLayout();

		if (m_Image && m_Image->IsOk())
		{
			std::vector<PackedRGB<uint8_t>> bufferRGB;
			bufferRGB.resize(GetImageBufferSize(*m_Image));
			std::memcpy(bufferRGB.data(), m_Image->GetData(), GetImageBufferSize(*m_Image, 3));

			return bufferRGB;
		}
		return {};
	}
	std::vector<uint8_t> BitmapImage::GetPixelDataAlpha() const
	{
		if (m_Image && m_Image->IsOk() && m_Image->HasAlpha())
		{
			const auto alpha = m_Image->GetAlpha();
			return {alpha, alpha + GetImageBufferSize(*m_Image)};
		}
		return {};
	}

	void BitmapImage::SetPixelDataRGBA(const PackedRGBA<uint8_t>* pixelData)
	{
		if (m_Image && m_Image->IsOk())
		{
			if (!m_Image->HasAlpha())
			{
				m_Image->InitAlpha();
			}

			auto targetRGB = m_Image->GetData();
			auto targetAlpha = m_Image->GetAlpha();

			const size_t totalSize = GetImageBufferSize(*m_Image);
			for (size_t i = 0; i < totalSize; i++)
			{
				// Copy first 3 components for RGB data
				auto rgb = targetRGB + (i * 3);
				rgb[0] = pixelData[i].Red;
				rgb[1] = pixelData[i].Green;
				rgb[2] = pixelData[i].Blue;

				// Copy single component for alpha data
				targetAlpha[i] = pixelData[i].Alpha;
			}
		}
	}
	void BitmapImage::SetPixelDataRGB(const PackedRGB<uint8_t>* pixelDataRGB)
	{
		if (m_Image && m_Image->IsOk())
		{
			AssertPackedColorLayout();

			std::memcpy(m_Image->GetData(), pixelDataRGB, GetImageBufferSize(*m_Image, 3));
		}
	}
	void BitmapImage::SetPixelDataAlpha(const uint8_t* pixelDataAlpha)
	{
		if (m_Image && m_Image->IsOk())
		{
			if (!m_Image->HasAlpha())
			{
				m_Image->InitAlpha();
			}
			std::memcpy(m_Image->GetAlpha(), pixelDataAlpha, GetImageBufferSize(*m_Image));
		}
	}

	void BitmapImage::ClearPixelDataRGBA(uint8_t value)
	{
		if (m_Image)
		{
			m_Image->Clear(value);
			if (!m_Image->HasAlpha())
			{
				m_Image->InitAlpha();
			}
			std::memset(m_Image->GetAlpha(), value, GetImageBufferSize(*m_Image));
		}
	}
	void BitmapImage::ClearPixelDataRGB(uint8_t value)
	{
		if (m_Image)
		{
			m_Image->Clear(value);
		}
	}
	void BitmapImage::ClearPixelDataAlpha(uint8_t value)
	{
		if (m_Image)
		{
			if (!m_Image->HasAlpha())
			{
				m_Image->InitAlpha();
			}
			std::memset(m_Image->GetAlpha(), value, GetImageBufferSize(*m_Image));
		}
	}

	PackedRGBA<uint8_t> BitmapImage::GetPixelRGBA(const Point& pos) const
	{
		if (m_Image && m_Image->IsOk())
		{
			return {GetRed(pos), GetGreen(pos), GetBlue(pos), GetAlpha(pos)};
		}
		return {};
	}
	PackedRGB<uint8_t> BitmapImage::GetPixelRGB(const Point& pos) const
	{
		if (m_Image && m_Image->IsOk())
		{
			return {GetRed(pos), GetGreen(pos), GetBlue(pos)};
		}
		return {};
	}

	void BitmapImage::SetPixelRGBA(const Point& pos, const PackedRGBA<uint8_t>& color)
	{
		if (m_Image && m_Image->IsOk())
		{
			InitAlpha();

			m_Image->SetRGB(pos.GetX(), pos.GetY(), color.Red, color.Green, color.Blue);
			m_Image->SetAlpha(pos.GetX(), pos.GetY(), color.Alpha);
		}
	}
	void BitmapImage::SetPixelRGB(const Point& pos, const PackedRGB<uint8_t>& color)
	{
		if (m_Image && m_Image->IsOk())
		{
			m_Image->SetRGB(pos.GetX(), pos.GetY(), color.Red, color.Green, color.Blue);
		}
	}
	void BitmapImage::SetAreaRGBA(const Rect& rect, const PackedRGBA<uint8_t>& color)
	{
		if (m_Image && m_Image->IsOk())
		{
			m_Image->SetRGB(rect, color.Red, color.Green, color.Blue);

			InitAlpha();
			for (int y = rect.GetY(); y < m_Image->GetHeight(); y++)
			{
				for (int x = rect.GetX(); x < m_Image->GetWidth(); x++)
				{
					m_Image->SetAlpha(x, y, color.Alpha);
				}
			}
		}
	}
	void BitmapImage::SetAreaRGB(const Rect& rect, const PackedRGB<uint8_t>& color)
	{
		if (m_Image && m_Image->IsOk())
		{
			m_Image->SetRGB(rect, color.Red, color.Green, color.Blue);
		}
	}

	void BitmapImage::ReplaceRGB(const PackedRGB<uint8_t>& source, const PackedRGB<uint8_t>& target)
	{
		if (m_Image)
		{
			m_Image->Replace(source.Red, source.Green, source.Blue, target.Red, target.Green, target.Blue);
		}
	}
	void BitmapImage::ReplaceRGBA(const PackedRGBA<uint8_t>& source, const PackedRGBA<uint8_t>& target)
	{
		if (m_Image && m_Image->IsOk())
		{
			if (!m_Image->HasAlpha())
			{
				m_Image->InitAlpha();
			}

			auto rgb = m_Image->GetData();
			auto alpha = m_Image->GetAlpha();
			const size_t totalSize = GetImageBufferSize(*m_Image);
			for (size_t i = 0; i < totalSize; i++)
			{
				auto pixel = rgb + (i * 3);
				if (source.Red == pixel[0] && source.Green == pixel[1] && source.Blue == pixel[2] && source.Alpha == alpha[i])
				{
					pixel[0] = target.Red;
					pixel[1] = target.Green;
					pixel[2] = target.Blue;
					alpha[i] = target.Alpha;
				}
			}
		}
	}

	// IBitmapImage: Transparency
	bool BitmapImage::IsPartiallyTransparent() const
	{
		if (m_Image && m_Image->HasAlpha())
		{
			const auto alpha = m_Image->GetAlpha();
			const size_t count = GetImageBufferSize(*m_Image);
			for (size_t i = 0; i < count; i++)
			{
				if (alpha[i] != g_OpaqueAlpha)
				{
					return true;
				}
			}
		}
		return false;
	}
	bool BitmapImage::IsPixelTransparent(const Point& pos, uint8_t threshold) const
	{
		if (m_Image)
		{
			return m_Image->IsTransparent(pos.GetX(), pos.GetY(), threshold);
		}
		return {};
	}

	// BitmapImage
	void BitmapImage::SetFormat(const UniversallyUniqueID& format)
	{
		if (m_Image)
		{
			m_Image->SetType(wxWidgets::MapImageFormat(format));
		}
	}
	bool BitmapImage::IsSameAs(const BitmapImage& other) const
	{
		return this == &other || m_Image == other.m_Image || (m_Image && other.m_Image && m_Image->IsSameAs(*other.m_Image));
	}
	BitmapImage BitmapImage::Clone() const
	{
		if (m_Image && m_Image->IsOk())
		{
			return m_Image->Copy();
		}
		return {};
	}

	// BitmapImage: Pixel data
	const uint8_t* BitmapImage::GetRawData() const
	{
		return m_Image ? m_Image->GetData() : nullptr;
	}
	uint8_t* BitmapImage::GetRawData()
	{
		return m_Image ? m_Image->GetData() : nullptr;
	}
	void BitmapImage::ClearRawData(uint8_t value)
	{
		m_Image->Clear(value);
	}

	void BitmapImage::SetRawData(wxMemoryBuffer& alpha)
	{
		if (m_Image)
		{
			m_Image->SetData(static_cast<unsigned char*>(alpha.release()), false);
		}
	}
	void BitmapImage::SetRawData(uint8_t* alpha)
	{
		if (m_Image)
		{
			m_Image->SetData(alpha, true);
		}
	}
	void BitmapImage::SetRawData(const Size& size, wxMemoryBuffer& alpha)
	{
		if (m_Image)
		{
			m_Image->SetData(static_cast<unsigned char*>(alpha.release()), size.GetWidth(), size.GetHeight(), false);
		}
	}
	void BitmapImage::SetRawData(const Size& size, uint8_t* alpha)
	{
		if (m_Image)
		{
			m_Image->SetData(alpha, size.GetWidth(), size.GetHeight(), true);
		}
	}

	bool BitmapImage::HasAlpha() const
	{
		return m_Image ? m_Image->HasAlpha() : false;
	}
	bool BitmapImage::InitAlpha()
	{
		if (m_Image && !m_Image->HasAlpha())
		{
			m_Image->InitAlpha();
			return true;
		}
		return false;
	}
	bool BitmapImage::ClearAlpha()
	{
		if (m_Image && m_Image->HasAlpha())
		{
			m_Image->ClearAlpha();
			return true;
		}
		return false;
	}

	const uint8_t* BitmapImage::GetRawAlpha() const
	{
		return m_Image ? m_Image->GetAlpha() : nullptr;
	}
	uint8_t* BitmapImage::GetRawAlpha()
	{
		return m_Image ? m_Image->GetAlpha() : nullptr;
	}

	uint8_t BitmapImage::GetRed(const Point& pos) const
	{
		if (m_Image)
		{
			return m_Image->GetRed(pos.GetX(), pos.GetY());
		}
		return 0;
	}
	uint8_t BitmapImage::GetGreen(const Point& pos) const
	{
		if (m_Image)
		{
			return m_Image->GetGreen(pos.GetX(), pos.GetY());
		}
		return 0;
	}
	uint8_t BitmapImage::GetBlue(const Point& pos) const
	{
		if (m_Image)
		{
			return m_Image->GetBlue(pos.GetX(), pos.GetY());
		}
		return 0;
	}
	uint8_t BitmapImage::GetAlpha(const Point& pos) const
	{
		if (m_Image && m_Image->HasAlpha())
		{
			return m_Image->GetAlpha(pos.GetX(), pos.GetY());
		}
		return g_OpaqueAlpha;
	}

	// BitmapImage: Transformation
	BitmapImage BitmapImage::GetSubImage(const Rect& rect) const
	{
		if (m_Image)
		{
			return m_Image->GetSubImage(rect);
		}
		return {};
	}

	BitmapImage BitmapImage::Blur(int radius, Orientation orientation) const
	{
		if (m_Image)
		{
			switch (orientation)
			{
				case Orientation::Vertical:
				{
					return m_Image->BlurVertical(radius);
				}
				case Orientation::Horizontal:
				{
					return m_Image->BlurHorizontal(radius);
				}
				case Orientation::Both:
				{
					return m_Image->Blur(radius);
				}
			};
		}
		return {};
	}
	BitmapImage BitmapImage::Paste(const BitmapImage& image, const Point& pos, CompositionMode compositionMode)
	{
		if (m_Image && image)
		{
			wxImage copy = *m_Image;
			copy.Paste(*image.m_Image, pos.GetX(), pos.GetY());
			return copy;
		}
		return {};
	}
	BitmapImage BitmapImage::Mirror(Orientation orientation) const
	{
		if (m_Image && m_Image->IsOk())
		{
			switch (orientation)
			{
				case Orientation::Vertical:
				{
					return m_Image->Mirror(false);
				}
				case Orientation::Horizontal:
				{
					return m_Image->Mirror(true);
				}
			};
		}
		return {};
	}
	BitmapImage BitmapImage::Rotate(Angle angle, const Point& rotationCenter, InterpolationQuality interpolationQuality) const
	{
		if (m_Image && m_Image->IsOk())
		{
			const float deg = angle.ToDegrees();

			if (deg == 90.0f)
			{
				return m_Image->Rotate90(true);
			}
			else if (deg == -90.0f)
			{
				return m_Image->Rotate90(false);
			}
			else if (deg == 180.0f)
			{
				return m_Image->Rotate180();
			}
			else
			{
				return m_Image->Rotate(angle.ToRadians(), rotationCenter, interpolationQuality != InterpolationQuality::None);
			}
		}
		return {};
	}

	BitmapImage& BitmapImage::Resize(const Size& size, const Point& pos, const PackedRGB<uint8_t>& backgroundColor)
	{
		if (m_Image && size != m_Image->GetSize())
		{
			m_Image->Resize(size, pos, backgroundColor.Red, backgroundColor.Green, backgroundColor.Blue);
		}
		return *this;
	}
	BitmapImage& BitmapImage::Rescale(const Size& size, InterpolationQuality interpolationQuality)
	{
		if (m_Image && size != m_Image->GetSize())
		{
			m_Image->Rescale(size.GetWidth(), size.GetHeight(), MapInterpolarionQuality(interpolationQuality));
		}
		return *this;
	}

	// BitmapImage: Conversion
	wxImage& BitmapImage::AsWXImage() noexcept
	{
		return m_Image ? *m_Image : wxNullImage;
	}
	const wxImage& BitmapImage::AsWXImage() const noexcept
	{
		return m_Image ? *m_Image : wxNullImage;
	}

	wxIcon BitmapImage::ToWXIcon(const Size& size, InterpolationQuality interpolationQuality) const
	{
		if (m_Image && m_Image->IsOk())
		{
			wxIcon icon;
			icon.CopyFromBitmap(ToWXBitmap(size, interpolationQuality));

			return icon;
		}
		return {};
	}
	wxCursor BitmapImage::ToWXCursor(const Size& size, InterpolationQuality interpolationQuality) const
	{
		if (m_Image && m_Image->IsOk())
		{
			if (!size.IsFullySpecified() || m_Image->GetSize() == size)
			{
				return *m_Image;
			}
			else
			{
				return m_Image->Scale(size.GetWidth(), size.GetHeight(), MapInterpolarionQuality(interpolationQuality));
			}
		}
		return {};
	}
	wxBitmap BitmapImage::ToWXBitmap(const Size& size, InterpolationQuality interpolationQuality) const
	{
		if (m_Image && m_Image->IsOk())
		{
			if (!size.IsFullySpecified() || m_Image->GetSize() == size)
			{
				return wxBitmap(*m_Image, *ColorDepthDB::BPP32);
			}
			else
			{
				return wxBitmap(m_Image->Scale(size.GetWidth(), size.GetHeight(), MapInterpolarionQuality(interpolationQuality)), *ColorDepthDB::BPP32);
			}
		}
		return {};
	}
	BitmapImage BitmapImage::ConvertToDisabled(Angle brightness) const
	{
		if (m_Image && m_Image->IsOk())
		{
			uint8_t value = brightness.ToNormalized() * ColorTraits<uint8_t>::max();
			return m_Image->ConvertToDisabled(value);
		}
		return {};
	}
	BitmapImage BitmapImage::ConvertToMonochrome(const PackedRGB<uint8_t>& makeWhite) const
	{
		if (m_Image && m_Image->IsOk())
		{
			return m_Image->ConvertToMono(makeWhite.Red, makeWhite.Green, makeWhite.Blue);
		}
		return {};
	}
	BitmapImage BitmapImage::ConvertToGrayscale(const PackedRGB<float>& weight) const
	{
		if (m_Image && m_Image->IsOk())
		{
			return m_Image->ConvertToGreyscale(weight.Red, weight.Green, weight.Blue);
		}
		return {};
	}

	// BitmapImage: Misc
	std::optional<PackedRGB<uint8_t>> BitmapImage::FindFirstUnusedColour(const PackedRGB<uint8_t>& startAt) const
	{
		if (m_Image)
		{
			PackedRGB<uint8_t> firstUnused;
			if (m_Image->IsOk() && m_Image->FindFirstUnusedColour(&firstUnused.Red, &firstUnused.Green, &firstUnused.Blue, startAt.Red, startAt.Green, startAt.Blue))
			{
				return firstUnused;
			}
		}
		return {};
	}
	BitmapImage& BitmapImage::RotateHue(Angle angle)
	{
		if (m_Image)
		{
			m_Image->RotateHue(angle.ToNormalized());
		}
		return *this;
	}

	// BitmapImage: Mask
	bool BitmapImage::IsMaskEnabled() const
	{
		return m_Image ? m_Image->HasMask() : false;
	}
	void BitmapImage::EnableMask(bool enable)
	{
		if (m_Image)
		{
			m_Image->SetMask(enable);
		}
	}

	PackedRGB<uint8_t> BitmapImage::GetMask() const
	{
		if (m_Image)
		{
			return {m_Image->GetMaskRed(), m_Image->GetMaskGreen(), m_Image->GetMaskBlue()};
		}
		return {};
	}
	void BitmapImage::SetMask(const PackedRGB<uint8_t>& color)
	{
		if (m_Image)
		{
			m_Image->SetMaskColour(color.Red, color.Green, color.Blue);
		}
	}
	void BitmapImage::SetMask(const Color& color)
	{
		SetMask(color.GetFixed8().RemoveAlpha());
	}
	bool BitmapImage::SetMask(const BitmapImage& shape, const PackedRGB<uint8_t>& color)
	{
		if (m_Image && shape)
		{
			return m_Image->SetMaskFromImage(*shape.m_Image, color.Red, color.Green, color.Blue);
		}
		return false;
	}
	bool BitmapImage::SetMask(const BitmapImage& shape, const Color& color)
	{
		if (shape)
		{
			return SetMask(*shape.m_Image, color.GetFixed8().RemoveAlpha());
		}
		return false;
	}

	BitmapImage& BitmapImage::operator=(const BitmapImage& other)
	{
		if (other)
		{
			m_Image = std::make_unique<wxImage>(*other.m_Image);
		}
		else
		{
			m_Image = nullptr;
		}
		return *this;
	}
}

namespace kxf
{
	uint64_t BinarySerializer<BitmapImage>::Serialize(IOutputStream& stream, const BitmapImage& value) const
	{
		if (value)
		{
			auto written = Serialization::WriteObject(stream, value.GetSize());
			written += Serialization::WriteObject(stream, value.GetColorDepth());

			if (value.HasAlpha())
			{
				written += Serialization::WriteObject(stream, value.GetPixelDataRGBA());
			}
			else
			{
				written += Serialization::WriteObject(stream, value.GetPixelDataRGB());
			}
			return written;
		}
		else
		{
			return Serialization::WriteObject(stream, Size::UnspecifiedSize()) + Serialization::WriteObject<ColorDepth>(stream, {});
		}
	}
	uint64_t BinarySerializer<BitmapImage>::Deserialize(IInputStream& stream, BitmapImage& value) const
	{
		Size size;
		ColorDepth colorDepth;
		auto read = Serialization::ReadObject(stream, size);
		read += Serialization::ReadObject(stream, colorDepth);

		if (size.IsFullySpecified())
		{
			value = BitmapImage(size);

			if (colorDepth == ColorDepthDB::BPP32)
			{
				std::vector<PackedRGBA<uint8_t>> rgba;
				read += Serialization::ReadObject(stream, rgba);

				value.SetPixelDataRGBA(rgba.data());
			}
			else if (colorDepth == ColorDepthDB::BPP24)
			{
				std::vector<PackedRGB<uint8_t>> rgb;
				read += Serialization::ReadObject(stream, rgb);

				value.SetPixelDataRGB(rgb.data());
			}
			else
			{
				throw BinarySerializerException(Format("Invalid color depth value: '{}'", colorDepth.GetValue()));
			}
		}
		else
		{
			value = {};
		}
		return read;
	}
}
