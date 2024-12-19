#pragma once
#include "kxf/Core/UniversallyUniqueID.h"
#include "kxf/Drawing/Common.h"
#include "kxf/Drawing/ImageDefines.h"
#include "kxf/Drawing/AffineMatrix.h"
#include "kxf/Drawing/FontMetrics.h"

enum wxPenCap;
enum wxPenJoin;
enum wxPenStyle;
enum wxHatchStyle;
enum wxFloodFillStyle;
enum wxPolygonFillMode;

enum wxFontStyle;
enum wxFontWeight;
enum wxFontFamily;
enum wxFontEncoding;
enum wxFontSymbolicSize;

enum wxBitmapType;
class wxAffineMatrix2D;
struct wxFontMetrics;

namespace kxf::wxWidgets
{
	std::optional<int> MapNativeHatchStyle(wxHatchStyle style) noexcept;
	wxHatchStyle MapNativeHatchStyle(int style) noexcept;

	wxPenStyle MapNativePenStyle(int style) noexcept;
	wxPenJoin MapNativePenJoin(int join) noexcept;

	wxHatchStyle MapHatchStyle(HatchStyle style) noexcept;
	HatchStyle MapHatchStyle(wxHatchStyle style) noexcept;

	std::optional<wxDeprecatedGUIConstants> MapDashStyle(DashStyle style) noexcept;
	DashStyle MapDashStyle(wxDeprecatedGUIConstants style) noexcept;

	wxPenJoin MapLineJoin(LineJoin join) noexcept;
	LineJoin MapLineJoin(wxPenJoin join) noexcept;

	wxPenCap MapLineCap(LineCap cap) noexcept;
	LineCap MapLineCap(wxPenCap cap) noexcept;

	std::optional<wxFloodFillStyle> MapFloodFillMode(FloodFillMode fill) noexcept;
	std::optional<wxPolygonFillMode> MapPolygonFillMode(PolygonFillMode fill) noexcept;
}

namespace kxf::wxWidgets
{
	wxFontStyle MapFontStyle(FlagSet<FontStyle> style) noexcept;
	FlagSet<FontStyle> MapFontStyle(wxFontStyle style) noexcept;

	wxFontWeight MapFontWeight(FontWeight weight) noexcept;
	FontWeight MapFontWeight(wxFontWeight weight) noexcept;

	wxFontSymbolicSize MapFontSymbolicSize(FontSymbolicSize symbolicSize) noexcept;
	FontSymbolicSize MapFontSymbolicSize(wxFontSymbolicSize symbolicSize) noexcept;

	wxFontFamily MapFontFamily(FontFamily family) noexcept;
	FontFamily MapFontFamily(wxFontFamily family) noexcept;

	wxFontEncoding MapFontEncoding(FontEncoding encoding) noexcept;
	FontEncoding MapFontEncoding(wxFontEncoding encoding) noexcept;
}

namespace kxf::wxWidgets
{
	wxBitmapType RegisterWxBitmapType(const UniversallyUniqueID& format);

	UniversallyUniqueID MapImageFormat(wxBitmapType bitmapType) noexcept;
	wxBitmapType MapImageFormat(const UniversallyUniqueID& format) noexcept;

	AffineMatrixD MapAffineMatrix(const wxAffineMatrix2D& matrixWx) noexcept;
	wxAffineMatrix2D MapAffineMatrix(const AffineMatrixD& matrix) noexcept;

	wxFontMetrics MapFontMetrics(const FontMetrics& metrics) noexcept;
	FontMetrics MapFontMetrics(const wxFontMetrics& metricsWx) noexcept;
}
