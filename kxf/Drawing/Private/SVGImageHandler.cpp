#include "kxf-pch.h"
#include "SVGImageHandler.h"
#include "../SVGImage.h"
#include "../BitmapImage.h"
#include "kxf/wxWidgets/MapDrawing.h"
#include "kxf/wxWidgets/StreamWrapper.h"
#include <wx/image.h>

namespace kxf::Drawing::Private
{
	// wxImageHandler
	bool SVGImageHandler::DoCanRead(wxInputStream& stream)
	{
		wxWidgets::InputStreamWrapper wrapper(stream);
		return SVGImageHandler::GetSubImageCount(wrapper) != 0;
	}
	int SVGImageHandler::DoGetImageCount(wxInputStream& stream)
	{
		wxWidgets::InputStreamWrapper wrapper(stream);
		return static_cast<int>(SVGImageHandler::GetSubImageCount(wrapper));
	}

	SVGImageHandler::SVGImageHandler()
	{
		m_name = "Scalable Vector Graphics";
		m_mime = "image/svg+xml";
		m_extension = "svg";
		m_type = wxWidgets::RegisterWxBitmapType(ImageFormat::SVG);
	}

	// wxImageHandler
	bool SVGImageHandler::LoadFile(wxImage* image, wxInputStream& stream, bool verbose, int index)
	{
		if (image && index <= 0)
		{
			wxWidgets::InputStreamWrapper wrapper(stream);

			SVGImage svgImage;
			if (svgImage.Load(wrapper, ImageFormat::SVG))
			{
				const int dpi = image->GetOptionInt(wxIMAGE_OPTION_RESOLUTION);
				const int width = image->GetOptionInt(wxIMAGE_OPTION_MAX_WIDTH);
				const int height = image->GetOptionInt(wxIMAGE_OPTION_MAX_HEIGHT);

				svgImage.SetOption(ImageOption::DPI, dpi);
				*image = svgImage.ToBitmapImage({width, height}).ToWxImage();
				return image->IsOk();
			}
		}
		return false;
	}
	bool SVGImageHandler::SaveFile(wxImage* image, wxOutputStream& stream, bool verbose)
	{
		// We can't save our raster image back to SVG
		return false;
	}

	// IImageHandler
	std::shared_ptr<IImage2D> SVGImageHandler::CreateImage()
	{
		return std::make_shared<SVGImage>();
	}

	size_t SVGImageHandler::GetSubImageCount(IInputStream& stream)
	{
		SVGImage image;
		return image.Load(stream, ImageFormat::SVG) ? 1 : 0;
	}
	bool SVGImageHandler::LoadImage(IImage2D& image, IInputStream& stream, size_t index)
	{
		if (std::shared_ptr<SVGImage> svgImage; image.QueryInterface(svgImage))
		{
			return svgImage->Load(stream, ImageFormat::SVG);
		}
		return false;
	}
	bool SVGImageHandler::SaveImage(const IImage2D& image, IOutputStream& stream) const
	{
		if (std::shared_ptr<const SVGImage> svgImage; image.QueryInterface(svgImage))
		{
			return svgImage->Save(stream, ImageFormat::SVG);
		}
		return false;
	}
}
