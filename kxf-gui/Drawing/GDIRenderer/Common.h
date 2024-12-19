#pragma once
#include "kxf/Drawing/Common.h"
#include "kxf/Drawing/Geometry.h"
#include "kxf/Drawing/Font.h"
#include "kxf/Drawing/Angle.h"
#include "kxf/Drawing/Color.h"
#include "kxf/Drawing/ColorDepth.h"
#include "kxf/Drawing/FontMetrics.h"

namespace kxf
{
	enum class GDIMappingMode
	{
		Text = wxMM_TEXT,
		Metric = wxMM_METRIC,
		LoMetric = wxMM_LOMETRIC,
		Twips = wxMM_TWIPS,
		Points = wxMM_POINTS,
	};
	enum class GDILogicalFunction
	{
		Nop = wxNO_OP, // dst

		Set = wxSET, // 1
		Clear = wxCLEAR, // 0
		Invert = wxINVERT, // NOT dst
		Copy = wxCOPY, // src
		And = wxAND, // src AND dst
		Or = wxOR, // src OR dst
		Xor = wxXOR, // src XOR dst
		Nor = wxNOR, // (NOT src) AND (NOT dst)
		Nand = wxNAND, // (NOT src) OR (NOT dst)
		Equiv = wxEQUIV, // (NOT src) XOR dst

		OrReverse = wxOR_REVERSE, // src OR (NOT dst)
		AndReverse = wxAND_REVERSE, // src AND (NOT dst)
		AndInvert = wxAND_INVERT, // (NOT src) AND dst
		OrInvert = wxOR_INVERT, // (NOT src) OR dst
		SrcInvert = wxSRC_INVERT, // (NOT src)
	};
}
