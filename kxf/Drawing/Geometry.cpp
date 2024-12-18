#include "kxf-pch.h"
#include "Geometry.h"
#include <wx/gdicmn.h>
#include <wx/geometry.h>

namespace kxf::Geometry
{
	BasicPoint<int>::BasicPoint(const wxPoint& other) noexcept
		:TBase(other.x, other.y)
	{
	}
	BasicPoint<float>::BasicPoint(const wxPoint& other) noexcept
		:TBase(other.x, other.y)
	{
	}
	BasicPoint<double>::BasicPoint(const wxPoint& other) noexcept
		:TBase(other.x, other.y)
	{
	}

	BasicPoint<float>::BasicPoint(const wxRealPoint& other) noexcept
		:TBase(other.x, other.y)
	{
	}
	BasicPoint<double>::BasicPoint(const wxRealPoint& other) noexcept
		:TBase(other.x, other.y)
	{
	}

	BasicPoint<int>::BasicPoint(const wxPoint2DInt& other) noexcept
		:TBase(other.m_x, other.m_y)
	{
	}

	BasicPoint<float>::BasicPoint(const wxPoint2DDouble& other) noexcept
		:TBase(other.m_x, other.m_y)
	{
	}
	BasicPoint<double>::BasicPoint(const wxPoint2DDouble& other) noexcept
		:TBase(other.m_x, other.m_y)
	{
	}

	BasicPoint<int>::operator wxPoint() const noexcept
	{
		return {this->GetX(), this->GetY()};
	}
	BasicPoint<float>::operator wxPoint() const noexcept
	{
		return wxPoint(this->GetX(), this->GetY());
	}
	BasicPoint<double>::operator wxPoint() const noexcept
	{
		return wxPoint(this->GetX(), this->GetY());
	}

	BasicPoint<float>::operator wxRealPoint() const noexcept
	{
		return wxRealPoint(this->GetX(), this->GetY());
	}
	BasicPoint<double>::operator wxRealPoint() const noexcept
	{
		return {this->GetX(), this->GetY()};
	}

	BasicPoint<int>::operator wxPoint2DInt() const noexcept
	{
		return {this->GetX(), this->GetY()};
	}

	BasicPoint<float>::operator wxPoint2DDouble() const noexcept
	{
		return wxPoint2DDouble(this->GetX(), this->GetY());
	}
	BasicPoint<double>::operator wxPoint2DDouble() const noexcept
	{
		return {this->GetX(), this->GetY()};
	}
}
namespace kxf::Geometry
{
	BasicSize<int>::BasicSize(const wxSize& other) noexcept
		:TBase(other.GetWidth(), other.GetHeight())
	{
	}
	BasicSize<float>::BasicSize(const wxSize& other) noexcept
		:TBase(other.GetWidth(), other.GetHeight())
	{
	}
	BasicSize<double>::BasicSize(const wxSize& other) noexcept
		:TBase(other.GetWidth(), other.GetHeight())
	{
	}

	BasicSize<int>::operator wxSize() const noexcept
	{
		return {this->GetWidth(), this->GetHeight()};
	}
	BasicSize<float>::operator wxSize() const noexcept
	{
		return wxSize(this->GetWidth(), this->GetHeight());
	}
	BasicSize<double>::operator wxSize() const noexcept
	{
		return wxSize(this->GetWidth(), this->GetHeight());
	}
}
namespace kxf::Geometry
{
	BasicRect<int>::BasicRect(const wxRect& other) noexcept
		:TBase(other.GetX(), other.GetY(), other.GetWidth(), other.GetHeight())
	{
	}
	BasicRect<float>::BasicRect(const wxRect& other) noexcept
		:TBase(other.GetX(), other.GetY(), other.GetWidth(), other.GetHeight())
	{
	}
	BasicRect<double>::BasicRect(const wxRect& other) noexcept
		:TBase(other.GetX(), other.GetY(), other.GetWidth(), other.GetHeight())
	{
	}

	BasicRect<int>::BasicRect(const wxRect2DInt& other) noexcept
		:TBase(other.m_x, other.m_y, other.m_width, other.m_height)
	{
	}
	BasicRect<float>::BasicRect(const wxRect2DDouble& other) noexcept
		:TBase(other.m_x, other.m_y, other.m_width, other.m_height)
	{
	}
	BasicRect<double>::BasicRect(const wxRect2DDouble& other) noexcept
		:TBase(other.m_x, other.m_y, other.m_width, other.m_height)
	{
	}

	BasicRect<int>::operator wxRect() const noexcept
	{
		return {this->GetX(), this->GetY(), this->GetWidth(), this->GetHeight()};
	}
	BasicRect<float>::operator wxRect() const noexcept
	{
		return wxRect(this->GetX(), this->GetY(), this->GetWidth(), this->GetHeight());
	}
	BasicRect<double>::operator wxRect() const noexcept
	{
		return wxRect(this->GetX(), this->GetY(), this->GetWidth(), this->GetHeight());
	}

	BasicRect<int>::operator wxRect2DInt() const noexcept
	{
		return {this->GetX(), this->GetY(), this->GetWidth(), this->GetHeight()};
	}
	BasicRect<float>::operator wxRect2DDouble() const noexcept
	{
		return wxRect2DDouble(this->GetX(), this->GetY(), this->GetWidth(), this->GetHeight());
	}
	BasicRect<double>::operator wxRect2DDouble() const noexcept
	{
		return {this->GetX(), this->GetY(), this->GetWidth(), this->GetHeight()};
	}
}

namespace kxf
{
	std::strong_ordering operator<=>(const Point& left, const wxPoint& right) noexcept
	{
		if (auto cmp = left.GetX() <=> right.x; cmp != 0)
		{
			return cmp;
		}
		return left.GetY() <=> right.y;
	}
	bool operator==(const Point& left, const wxPoint& right) noexcept
	{
		return left <=> right == 0;
	}

	std::strong_ordering operator<=>(const Point& left, const wxPoint2DInt& right) noexcept
	{
		if (auto cmp = left.GetX() <=> right.m_x; cmp != 0)
		{
			return cmp;
		}
		return left.GetY() <=> right.m_y;
	}
	bool operator==(const Point& left, const wxPoint2DInt& right) noexcept
	{
		return left <=> right == 0;
	}

	std::partial_ordering operator<=>(const PointD& left, const wxRealPoint& right) noexcept
	{
		if (auto cmp = left.GetX() <=> right.x; cmp != 0)
		{
			return cmp;
		}
		return left.GetY() <=> right.y;
	}
	bool operator==(const PointD& left, const wxRealPoint& right) noexcept
	{
		return left <=> right == 0;
	}

	std::partial_ordering operator<=>(const PointD& left, const wxPoint2DDouble& right) noexcept
	{
		if (auto cmp = left.GetX() <=> right.m_x; cmp != 0)
		{
			return cmp;
		}
		return left.GetY() <=> right.m_y;
	}
	bool operator==(const PointD& left, const wxPoint2DDouble& right) noexcept
	{
		return left <=> right == 0;
	}
}
namespace kxf
{
	std::strong_ordering operator<=>(const Size& left, const wxSize& right) noexcept
	{
		if (auto cmp = left.GetWidth() <=> right.GetWidth(); cmp != 0)
		{
			return cmp;
		}
		return left.GetHeight() <=> right.GetHeight();
	}
	bool operator==(const Size& left, const wxSize& right) noexcept
	{
		return left <=> right == 0;
	}
}
namespace kxf
{
	std::strong_ordering operator<=>(const Rect& left, const wxRect& right) noexcept
	{
		if (auto cmp = left.GetX() <=> right.GetX(); cmp != 0)
		{
			return cmp;
		}
		if (auto cmp = left.GetY() <=> right.GetY(); cmp != 0)
		{
			return cmp;
		}
		if (auto cmp = left.GetWidth() <=> right.GetWidth(); cmp != 0)
		{
			return cmp;
		}
		return left.GetHeight() <=> right.GetHeight();
	}
	bool operator==(const Rect& left, const wxRect& right) noexcept
	{
		return left <=> right == 0;
	}

	std::strong_ordering operator<=>(const Rect& left, const wxRect2DInt& right) noexcept
	{
		if (auto cmp = left.GetX() <=> right.m_x; cmp != 0)
		{
			return cmp;
		}
		if (auto cmp = left.GetY() <=> right.m_y; cmp != 0)
		{
			return cmp;
		}
		if (auto cmp = left.GetWidth() <=> right.m_width; cmp != 0)
		{
			return cmp;
		}
		return left.GetHeight() <=> right.m_height;
	}
	bool operator==(const Rect& left, const wxRect2DInt& right) noexcept
	{
		return left <=> right == 0;
	}

	std::partial_ordering operator<=>(const Rect& left, const wxRect2DDouble& right) noexcept
	{
		if (auto cmp = left.GetX() <=> right.m_x; cmp != 0)
		{
			return cmp;
		}
		if (auto cmp = left.GetY() <=> right.m_y; cmp != 0)
		{
			return cmp;
		}
		if (auto cmp = left.GetWidth() <=> right.m_width; cmp != 0)
		{
			return cmp;
		}
		return left.GetHeight() <=> right.m_height;
	}
	bool operator==(const Rect& left, const wxRect2DDouble& right) noexcept
	{
		return left <=> right == 0;
	}
}
