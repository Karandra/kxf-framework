#pragma once
#include "kxf/Drawing/Geometry.h"
#include "kxf/Drawing/ColorDepth.h"

namespace kxf::Private
{
	inline std::vector<wxPoint> ConvertWxPoints(const Point* points, size_t count)
	{
		return {points, points + count};
	}

	template<size_t N>
	auto ConvertWxPoints(const Point(&points)[N]) noexcept
	{
		std::array<wxPoint, N> pointsBuffer;
		std::copy_n(std::begin(points), N, pointsBuffer.begin());

		return pointsBuffer;
	}

	template<size_t N>
	auto ConvertWxPoints(const std::array<Point, N>& points) noexcept
	{
		std::array<wxPoint, N> pointsBuffer;
		std::copy_n(points.begin(), N, pointsBuffer.begin());

		return pointsBuffer;
	}
}
