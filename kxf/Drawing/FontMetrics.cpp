#include "kxf-pch.h"
#include "FontMetrics.h"
#include <wx/dc.h>

namespace kxf::Private
{
	wxFontMetrics ToWxFontMetrics(const FontMetrics& metrics) noexcept
	{
		wxFontMetrics fontMetrics;
		fontMetrics.height = metrics.Height;
		fontMetrics.ascent = metrics.Ascent;
		fontMetrics.descent = metrics.Descent;
		fontMetrics.averageWidth = metrics.AverageWidth;
		fontMetrics.internalLeading = metrics.InternalLeading;
		fontMetrics.externalLeading = metrics.ExternalLeading;

		return fontMetrics;
	}
	FontMetrics FromWxFontMetrics(const wxFontMetrics& metricsWx) noexcept
	{
		FontMetrics fontMetrics;
		fontMetrics.Height = metricsWx.height;
		fontMetrics.Ascent = metricsWx.ascent;
		fontMetrics.Descent = metricsWx.descent;
		fontMetrics.AverageWidth = metricsWx.averageWidth;
		fontMetrics.InternalLeading = metricsWx.internalLeading;
		fontMetrics.ExternalLeading = metricsWx.externalLeading;

		return fontMetrics;
	}
}
