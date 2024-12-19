#include "kxf-pch.h"
#include "MapDrawing.h"
#include "kxf/Drawing/ImageDefines.h"
#include <wx/dc.h>
#include <wx/font.h>
#include <wx/defs.h>
#include <wx/pen.h>
#include <wx/brush.h>
#include <wx/peninfobase.h>
#include <wx/affinematrix2d.h>

namespace
{
	class BitmapTypeRegistry final
	{
		private:
			std::unordered_map<kxf::UniversallyUniqueID, wxBitmapType> m_Registry;
			std::atomic<int> m_Counter = static_cast<int>(wxBITMAP_TYPE_MAX) + 1000;

		public:
			BitmapTypeRegistry()
			{
				using namespace kxf;

				m_Registry.reserve(wxBITMAP_TYPE_MAX);
				m_Registry.insert_or_assign(ImageFormat::Any, wxBITMAP_TYPE_ANY);
				m_Registry.insert_or_assign(ImageFormat::ANI, wxBITMAP_TYPE_ANI);
				m_Registry.insert_or_assign(ImageFormat::BMP, wxBITMAP_TYPE_BMP);
				m_Registry.insert_or_assign(ImageFormat::CUR, wxBITMAP_TYPE_CUR);
				m_Registry.insert_or_assign(ImageFormat::GIF, wxBITMAP_TYPE_GIF);
				m_Registry.insert_or_assign(ImageFormat::ICO, wxBITMAP_TYPE_ICO);
				m_Registry.insert_or_assign(ImageFormat::PCX, wxBITMAP_TYPE_PCX);
				m_Registry.insert_or_assign(ImageFormat::IFF, wxBITMAP_TYPE_IFF);
				m_Registry.insert_or_assign(ImageFormat::PNG, wxBITMAP_TYPE_PNG);
				m_Registry.insert_or_assign(ImageFormat::PNM, wxBITMAP_TYPE_PNM);
				m_Registry.insert_or_assign(ImageFormat::TGA, wxBITMAP_TYPE_TGA);
				m_Registry.insert_or_assign(ImageFormat::XBM, wxBITMAP_TYPE_XBM);
				m_Registry.insert_or_assign(ImageFormat::XPM, wxBITMAP_TYPE_XPM);
				m_Registry.insert_or_assign(ImageFormat::PICT, wxBITMAP_TYPE_PICT);
				m_Registry.insert_or_assign(ImageFormat::TIFF, wxBITMAP_TYPE_TIFF);
				m_Registry.insert_or_assign(ImageFormat::JPEG, wxBITMAP_TYPE_JPEG);
			}

		public:
			std::optional<wxBitmapType> Find(const kxf::UniversallyUniqueID& format) const
			{
				if (auto it = m_Registry.find(format); it != m_Registry.end())
				{
					return it->second;
				}
				return {};
			}
			kxf::UniversallyUniqueID Find(wxBitmapType type) const
			{
				for (auto&& [uuid, typeWx]: m_Registry)
				{
					if (typeWx == type)
					{
						return uuid;
					}
				}
				return kxf::ImageFormat::None;
			}

			wxBitmapType Regsiter(const kxf::UniversallyUniqueID& format)
			{
				if (auto type = Find(format))
				{
					return *type;
				}
				else
				{
					return m_Registry.insert_or_assign(format, static_cast<wxBitmapType>(++m_Counter)).first->second;
				}
			}
	} g_BitmapTypeRegistry;
}

namespace kxf::wxWidgets
{
	std::optional<int> MapNativeHatchStyle(wxHatchStyle style) noexcept
	{
		switch (style)
		{
			case wxHATCHSTYLE_BDIAGONAL:
			{
				return HS_BDIAGONAL;
			}
			case wxHATCHSTYLE_CROSSDIAG:
			{
				return HS_DIAGCROSS;
			}
			case wxHATCHSTYLE_FDIAGONAL:
			{
				return HS_FDIAGONAL;
			}
			case wxHATCHSTYLE_CROSS:
			{
				return HS_CROSS;
			}
			case wxHATCHSTYLE_HORIZONTAL:
			{
				return HS_HORIZONTAL;
			}
			case wxHATCHSTYLE_VERTICAL:
			{
				return HS_VERTICAL;
			}
		};
		return {};
	}
	wxHatchStyle MapNativeHatchStyle(int style) noexcept
	{
		switch (style)
		{
			case HS_BDIAGONAL:
			{
				return wxHATCHSTYLE_BDIAGONAL;
			}
			case HS_DIAGCROSS:
			{
				return wxHATCHSTYLE_CROSSDIAG;
			}
			case HS_FDIAGONAL:
			{
				return wxHATCHSTYLE_FDIAGONAL;
			}
			case HS_CROSS:
			{
				return wxHATCHSTYLE_CROSS;
			}
			case HS_HORIZONTAL:
			{
				return wxHATCHSTYLE_HORIZONTAL;
			}
			case HS_VERTICAL:
			{
				return wxHATCHSTYLE_VERTICAL;
			}
		};
		return {};
	}

	wxPenStyle MapNativePenStyle(int style) noexcept
	{
		switch (style)
		{
			case PS_DASH:
			{
				return wxPENSTYLE_SHORT_DASH;
			}
			case PS_DOT:
			{
				return wxPENSTYLE_DOT;
			}
			case PS_DASHDOT:
			case PS_DASHDOTDOT:
			{
				return wxPENSTYLE_DOT_DASH;
			}
			case PS_SOLID:
			{
				return wxPENSTYLE_SOLID;
			}
			case PS_USERSTYLE:
			{
				return wxPENSTYLE_USER_DASH;
			}
			case PS_NULL:
			{
				return wxPENSTYLE_TRANSPARENT;
			}
		};
		return wxPENSTYLE_INVALID;
	}
	wxPenJoin MapNativePenJoin(int join) noexcept
	{
		switch (join)
		{
			case PS_JOIN_BEVEL:
			{
				return wxJOIN_BEVEL;
			}
			case PS_JOIN_MITER:
			{
				return wxJOIN_MITER;
			}
			case PS_JOIN_ROUND:
			{
				return wxJOIN_ROUND;
			}
		};
		return wxJOIN_INVALID;
	}

	wxHatchStyle MapHatchStyle(HatchStyle style) noexcept
	{
		switch (style)
		{
			case HatchStyle::Vertical:
			{
				return wxHATCHSTYLE_VERTICAL;
			}
			case HatchStyle::Horizontal:
			{
				return wxHATCHSTYLE_HORIZONTAL;
			}
			case HatchStyle::Cross:
			{
				return wxHATCHSTYLE_CROSS;
			}
			case HatchStyle::DiagonalCross:
			{
				return wxHATCHSTYLE_CROSSDIAG;
			}
			case HatchStyle::ForwardDiagonal:
			{
				return wxHATCHSTYLE_FDIAGONAL;
			}
			case HatchStyle::BackwardDiagonal:
			{
				return wxHATCHSTYLE_BDIAGONAL;
			}
		};
		return wxHATCHSTYLE_INVALID;
	}
	HatchStyle MapHatchStyle(wxHatchStyle style) noexcept
	{
		switch (style)
		{
			case wxHATCHSTYLE_VERTICAL:
			{
				return HatchStyle::Vertical;
			}
			case wxHATCHSTYLE_HORIZONTAL:
			{
				return HatchStyle::Horizontal;
			}
			case wxHATCHSTYLE_CROSS:
			{
				return HatchStyle::Cross;
			}
			case wxHATCHSTYLE_CROSSDIAG:
			{
				return HatchStyle::DiagonalCross;
			}
			case wxHATCHSTYLE_FDIAGONAL:
			{
				return HatchStyle::ForwardDiagonal;
			}
			case wxHATCHSTYLE_BDIAGONAL:
			{
				return HatchStyle::BackwardDiagonal;
			}
		};
		return HatchStyle::None;
	}

	std::optional<wxDeprecatedGUIConstants> MapDashStyle(DashStyle style) noexcept
	{
		switch (style)
		{
			case DashStyle::Dot:
			{
				return wxDOT;
			}
			case DashStyle::DashDot:
			{
				return wxDOT_DASH;
			}
			case DashStyle::Dash:
			{
				return wxSHORT_DASH;
			}
			case DashStyle::DashDotDot:
			{
				return wxLONG_DASH;
			}
			case DashStyle::Custom:
			{
				return wxUSER_DASH;
			}
		};
		return {};
	}
	DashStyle MapDashStyle(wxDeprecatedGUIConstants style) noexcept
	{
		switch (style)
		{
			case wxDOT:
			{
				return DashStyle::Dot;
			}
			case wxDOT_DASH:
			{
				return DashStyle::DashDot;
			}
			case wxSHORT_DASH:
			{
				return DashStyle::Dash;
			}
			case wxLONG_DASH:
			{
				return DashStyle::DashDotDot;
			}
			case wxUSER_DASH:
			{
				return DashStyle::Custom;
			}
		};
		return DashStyle::None;
	}

	wxPenJoin MapLineJoin(LineJoin join) noexcept
	{
		switch (join)
		{
			case LineJoin::Miter:
			{
				return wxJOIN_MITER;
			}
			case LineJoin::Bevel:
			{
				return wxJOIN_BEVEL;
			}
			case LineJoin::Round:
			{
				return wxJOIN_ROUND;
			}
		};
		return wxJOIN_INVALID;
	}
	LineJoin MapLineJoin(wxPenJoin join) noexcept
	{
		switch (join)
		{
			case wxJOIN_MITER:
			{
				return LineJoin::Miter;
			}
			case wxJOIN_BEVEL:
			{
				return LineJoin::Bevel;
			}
			case wxJOIN_ROUND:
			{
				return LineJoin::Round;
			}
		};
		return LineJoin::None;
	}

	wxPenCap MapLineCap(LineCap cap) noexcept
	{
		switch (cap)
		{
			case LineCap::Flat:
			{
				return wxCAP_BUTT;
			}
			case LineCap::Round:
			{
				return wxCAP_ROUND;
			}
			case LineCap::Square:
			{
				return wxCAP_PROJECTING;
			}
		};
		return wxCAP_INVALID;
	}
	LineCap MapLineCap(wxPenCap cap) noexcept
	{
		switch (cap)
		{
			case wxCAP_BUTT:
			{
				return LineCap::Flat;
			}
			case wxCAP_ROUND:
			{
				return LineCap::Round;
			}
			case wxCAP_PROJECTING:
			{
				return LineCap::Square;
			}
		};
		return LineCap::None;
	}

	std::optional<wxFloodFillStyle> MapFloodFillMode(FloodFillMode fill) noexcept
	{
		switch (fill)
		{
			case FloodFillMode::Surface:
			{
				return wxFLOOD_SURFACE;
			}
			case FloodFillMode::Border:
			{
				return wxFLOOD_BORDER;
			}
		};
		return {};
	}
	std::optional<wxPolygonFillMode> MapPolygonFillMode(PolygonFillMode fill) noexcept
	{
		switch (fill)
		{
			case PolygonFillMode::OddEvenRule:
			{
				return wxODDEVEN_RULE;
			}
			case PolygonFillMode::WindingRule:
			{
				return wxWINDING_RULE;
			}
		};
		return {};
	}
}

namespace kxf::wxWidgets
{
	wxFontStyle MapFontStyle(FlagSet<FontStyle> style) noexcept
	{
		if (style.Contains(FontStyle::Normal))
		{
			return wxFONTSTYLE_NORMAL;
		}
		if (style.Contains(FontStyle::Italic))
		{
			return wxFONTSTYLE_ITALIC;
		}
		if (style.Contains(FontStyle::Oblique))
		{
			return wxFONTSTYLE_SLANT;
		}
		return wxFONTSTYLE_MAX;
	}
	FlagSet<FontStyle> MapFontStyle(wxFontStyle style) noexcept
	{
		switch (style)
		{
			case wxFONTSTYLE_NORMAL:
			{
				return FontStyle::Normal;
			}
			case wxFONTSTYLE_ITALIC:
			{
				return FontStyle::Italic;
			}
			case wxFONTSTYLE_SLANT:
			{
				return FontStyle::Oblique;
			}
		};
		return {};
	}

	wxFontWeight MapFontWeight(FontWeight weight) noexcept
	{
		switch (weight)
		{
			case FontWeight::Thin:
			{
				return wxFONTWEIGHT_THIN;
			}
			case FontWeight::ExtraLight:
			{
				return wxFONTWEIGHT_EXTRALIGHT;
			}
			case FontWeight::Light:
			{
				return wxFONTWEIGHT_LIGHT;
			}
			case FontWeight::Normal:
			{
				return wxFONTWEIGHT_NORMAL;
			}
			case FontWeight::Medium:
			{
				return wxFONTWEIGHT_MEDIUM;
			}
			case FontWeight::SemiBold:
			{
				return wxFONTWEIGHT_SEMIBOLD;
			}
			case FontWeight::Bold:
			{
				return wxFONTWEIGHT_BOLD;
			}
			case FontWeight::ExtraBold:
			{
				return wxFONTWEIGHT_EXTRABOLD;
			}
			case FontWeight::Heavy:
			{
				return wxFONTWEIGHT_HEAVY;
			}
			case FontWeight::ExtraHeavy:
			{
				return wxFONTWEIGHT_EXTRAHEAVY;
			}
		};
		return wxFONTWEIGHT_INVALID;
	}
	FontWeight MapFontWeight(wxFontWeight weight) noexcept
	{
		switch (weight)
		{
			case wxFONTWEIGHT_THIN:
			{
				return FontWeight::Thin;
			}
			case wxFONTWEIGHT_EXTRALIGHT:
			{
				return FontWeight::ExtraLight;
			}
			case wxFONTWEIGHT_LIGHT:
			{
				return FontWeight::Light;
			}
			case wxFONTWEIGHT_NORMAL:
			{
				return FontWeight::Normal;
			}
			case wxFONTWEIGHT_MEDIUM:
			{
				return FontWeight::Medium;
			}
			case wxFONTWEIGHT_SEMIBOLD:
			{
				return FontWeight::SemiBold;
			}
			case wxFONTWEIGHT_BOLD:
			{
				return FontWeight::Bold;
			}
			case wxFONTWEIGHT_EXTRABOLD:
			{
				return FontWeight::ExtraBold;
			}
			case wxFONTWEIGHT_HEAVY:
			{
				return FontWeight::Heavy;
			}
			case wxFONTWEIGHT_EXTRAHEAVY:
			{
				return FontWeight::ExtraHeavy;
			}
		};
		return FontWeight::None;
	}

	wxFontSymbolicSize MapFontSymbolicSize(FontSymbolicSize symbolicSize) noexcept
	{
		switch (symbolicSize)
		{
			case FontSymbolicSize::ExtraSmall:
			{
				return wxFONTSIZE_XX_SMALL;
			}
			case FontSymbolicSize::VerySmall:
			{
				return wxFONTSIZE_X_SMALL;
			}
			case FontSymbolicSize::Small:
			{
				return wxFONTSIZE_SMALL;
			}
			case FontSymbolicSize::Normal:
			{
				return wxFONTSIZE_MEDIUM;
			}
			case FontSymbolicSize::Large:
			{
				return wxFONTSIZE_LARGE;
			}
			case FontSymbolicSize::VeryLarge:
			{
				return wxFONTSIZE_X_LARGE;
			}
			case FontSymbolicSize::ExtraLarge:
			{
				return wxFONTSIZE_XX_LARGE;
			}
		};
		return wxFontSymbolicSize::wxFONTSIZE_MEDIUM;
	}
	FontSymbolicSize MapFontSymbolicSize(wxFontSymbolicSize symbolicSize) noexcept
	{
		switch (symbolicSize)
		{
			case wxFONTSIZE_XX_SMALL:
			{
				return FontSymbolicSize::ExtraSmall;
			}
			case wxFONTSIZE_X_SMALL:
			{
				return FontSymbolicSize::VerySmall;
			}
			case wxFONTSIZE_SMALL:
			{
				return FontSymbolicSize::Small;
			}
			case wxFONTSIZE_MEDIUM:
			{
				return FontSymbolicSize::Normal;
			}
			case wxFONTSIZE_LARGE:
			{
				return FontSymbolicSize::Large;
			}
			case wxFONTSIZE_X_LARGE:
			{
				return FontSymbolicSize::VeryLarge;
			}
			case wxFONTSIZE_XX_LARGE:
			{
				return FontSymbolicSize::ExtraLarge;
			}
		};
		return FontSymbolicSize::Normal;
	}

	wxFontFamily MapFontFamily(FontFamily family) noexcept
	{
		switch (family)
		{
			case FontFamily::Default:
			{
				return wxFONTFAMILY_DEFAULT;
			}
			case FontFamily::Fantasy:
			{
				return wxFONTFAMILY_DECORATIVE;
			}
			case FontFamily::Serif:
			{
				return wxFONTFAMILY_ROMAN;
			}
			case FontFamily::SansSerif:
			{
				return wxFONTFAMILY_SWISS;
			}
			case FontFamily::Cursive:
			{
				return wxFONTFAMILY_SCRIPT;
			}
			case FontFamily::FixedWidth:
			{
				// Or 'wxFONTFAMILY_MODERN', wxWidgets doesn't make any distinction between them.
				return wxFONTFAMILY_TELETYPE;
			}
		};
		return wxFONTFAMILY_UNKNOWN;
	}
	FontFamily MapFontFamily(wxFontFamily family) noexcept
	{
		switch (family)
		{
			case wxFONTFAMILY_DEFAULT:
			{
				return FontFamily::Default;
			}
			case wxFONTFAMILY_DECORATIVE:
			{
				return FontFamily::Fantasy;
			}
			case wxFONTFAMILY_ROMAN:
			{
				return FontFamily::Serif;
			}
			case wxFONTFAMILY_SWISS:
			{
				return FontFamily::SansSerif;
			}
			case wxFONTFAMILY_SCRIPT:
			{
				return FontFamily::Cursive;
			}
			case wxFONTFAMILY_MODERN:
			case wxFONTFAMILY_TELETYPE:
			{
				return FontFamily::FixedWidth;
			}
		};
		return FontFamily::None;
	}

	wxFontEncoding MapFontEncoding(FontEncoding encoding) noexcept
	{
		return static_cast<wxFontEncoding>(encoding);
	}
	FontEncoding MapFontEncoding(wxFontEncoding encoding) noexcept
	{
		return static_cast<FontEncoding>(encoding);
	}
}

namespace kxf::wxWidgets
{
	wxBitmapType RegisterWxBitmapType(const UniversallyUniqueID& format)
	{
		return g_BitmapTypeRegistry.Regsiter(format);
	}

	UniversallyUniqueID MapImageFormat(wxBitmapType bitmapType) noexcept
	{
		return g_BitmapTypeRegistry.Find(bitmapType);
	}
	wxBitmapType MapImageFormat(const UniversallyUniqueID& format) noexcept
	{
		return g_BitmapTypeRegistry.Find(format).value_or(wxBITMAP_TYPE_INVALID);
	}

	AffineMatrixD MapAffineMatrix(const wxAffineMatrix2D& matrix) noexcept
	{
		wxMatrix2D matrix2D;
		wxPoint2DDouble txy;
		matrix.Get(&matrix2D, &txy);

		return
		{
			static_cast<double>(matrix2D.m_11),
			static_cast<double>(matrix2D.m_12),
			static_cast<double>(matrix2D.m_21),
			static_cast<double>(matrix2D.m_22),
			static_cast<double>(txy.m_x),
			static_cast<double>(txy.m_y)
		};
	}
	wxAffineMatrix2D MapAffineMatrix(const AffineMatrixD& matrix) noexcept
	{
		double m11 = 0;
		double m12 = 0;
		double m21 = 0;
		double m22 = 0;
		double tx = 0;
		double ty = 0;
		matrix.GetElements(m11, m12, m21, m22, tx, ty);

		wxMatrix2D matrix2D(static_cast<wxDouble>(m11), static_cast<wxDouble>(m12), static_cast<wxDouble>(m21), static_cast<wxDouble>(m22));
		wxPoint2DDouble txy(static_cast<wxDouble>(tx), static_cast<wxDouble>(ty));

		wxAffineMatrix2D affineMatrix;
		affineMatrix.Set(matrix2D, txy);
		return affineMatrix;
	}

	wxFontMetrics MapFontMetrics(const FontMetrics& metrics) noexcept
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
	FontMetrics MapFontMetrics(const wxFontMetrics& metricsWx) noexcept
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
