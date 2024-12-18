#include "kxf-pch.h"
#include "GraphicsPath.h"
#include "kxf/Sciter/ScriptValue.h"
#include "kxf/Sciter/SciterAPI.h"
#include "kxf/Sciter/Element.h"
#include "kxf/Sciter/Private/Conversion.h"

namespace
{
	constexpr bool g_UseRelativeCoordinates = true;
}

namespace kxf::Sciter
{
	bool GraphicsPath::DoAcquire(GraphicsPathHandle* handle)
	{
		return GetGrapchicsAPI()->pathAddRef(ToSciterPath(handle)) == GRAPHIN_OK;
	}
	void GraphicsPath::DoRelease()
	{
		GetGrapchicsAPI()->pathRelease(ToSciterPath(m_Handle));
	}

	GraphicsPath::GraphicsPath(const ScriptValue& value)
	{
		HPATH handle = nullptr;
		if (GetGrapchicsAPI()->vUnWrapPath(ToSciterScriptValue(value.GetNativeValue()), &handle))
		{
			Acquire(FromSciterPath(handle));
		}
	}

	ScriptValue GraphicsPath::ToScriptValue() const
	{
		ScriptValue value;
		if (GetGrapchicsAPI()->vWrapPath(ToSciterPath(m_Handle), ToSciterScriptValue(value.GetNativeValue())) == GRAPHIN_OK)
		{
			return value;
		}
		return {};
	}

	void GraphicsPath::CloseSubPath()
	{
		GetGrapchicsAPI()->pathClosePath(ToSciterPath(m_Handle));
	}
	void GraphicsPath::MoveToPoint(const wxPoint2DDouble& pos)
	{
		GetGrapchicsAPI()->pathMoveTo(ToSciterPath(m_Handle), pos.m_x, pos.m_y, g_UseRelativeCoordinates);
	}
	
	void GraphicsPath::AddLineToPoint(const wxPoint2DDouble& pos)
	{
		GetGrapchicsAPI()->pathLineTo(ToSciterPath(m_Handle), pos.m_x, pos.m_y, g_UseRelativeCoordinates);
	}
	void GraphicsPath::AddCurveToPoint(const wxPoint2DDouble& c1, const wxPoint2DDouble& c2, const wxPoint2DDouble& e)
	{
		GetGrapchicsAPI()->pathBezierCurveTo(ToSciterPath(m_Handle), c1.m_x, c1.m_y, c2.m_x, c2.m_y, e.m_x, e.m_y, g_UseRelativeCoordinates);
	}
	void GraphicsPath::AddQuadCurveToPoint(double cx, double cy, double x, double y)
	{
		GetGrapchicsAPI()->pathQuadraticCurveTo(ToSciterPath(m_Handle), cx, cy, x, y, g_UseRelativeCoordinates);
	}
	void GraphicsPath::AddArcToPoint(const wxPoint2DDouble& c, const wxPoint2DDouble& r, double angle, bool isLargeArc, bool closkwise)
	{
		GetGrapchicsAPI()->pathArcTo(ToSciterPath(m_Handle), c.m_x, c.m_y, angle, r.m_x, r.m_y, isLargeArc, closkwise, g_UseRelativeCoordinates);
	}
}
