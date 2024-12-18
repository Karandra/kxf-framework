#include "kxf-pch.h"
#include "GDIAction.h"

namespace kxf::Drawing
{
	Color GetAreaAverageColor(const GDIContext& dc, const Rect& rect)
	{
		PackedRGBA<uint32_t> rgba;
		size_t pixelCount = 0;

		for (int y = 0; y < rect.GetHeight(); y++)
		{
			for (int x = 0; x < rect.GetWidth(); x++)
			{
				if (Color color = dc.GetPixel(rect.GetPosition() + Point(x, y)))
				{
					const auto pixel = color.GetFixed8();
					rgba.Red += pixel.Red;
					rgba.Green += pixel.Green;
					rgba.Blue += pixel.Blue;
					rgba.Alpha += pixel.Alpha;

					++pixelCount;
				}
			}
		}

		if (pixelCount != 0)
		{
			return Color::FromFixed8(rgba.Red / pixelCount, rgba.Green / pixelCount, rgba.Blue / pixelCount, rgba.Alpha / pixelCount);
		}
		return {};
	}
}
