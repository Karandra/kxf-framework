#pragma once
#include "Common.h"
#include "Geometry.h"
struct wxFontMetrics;

namespace kxf::Drawing
{
	template<class TValue>
	struct BasicFontMetrics final
	{
		public:
			TValue Height = 0; // Total character height
			TValue Ascent = 0; // Part of the height above the baseline
			TValue Descent = 0; // Part of the height below the baseline
			TValue AverageWidth = 0; // Average font width or "x-width"
			TValue InternalLeading = 0; // Intra-line spacing
			TValue ExternalLeading = 0; // Inter-line spacing

		public:
			constexpr BasicFontMetrics() noexcept = default;
			constexpr BasicFontMetrics(const Geometry::BasicSize<TValue>& size) noexcept
				:Height(size.GetHeight()), AverageWidth(size.GetWidth())
			{
			}
			constexpr BasicFontMetrics(TValue height, TValue ascent, TValue descent, TValue averageWidth, TValue internalLeading, TValue externalLeading) noexcept
				:Height(height), Ascent(ascent), Descent(descent),
				AverageWidth(averageWidth), InternalLeading(internalLeading), ExternalLeading(externalLeading)
			{
			}

			template<class T>
			constexpr BasicFontMetrics(const BasicFontMetrics<T>& other) noexcept
				:Height(other.Height), Ascent(other.Ascent), Descent(other.Descent),
				AverageWidth(other.AverageWidth), InternalLeading(other.InternalLeading), ExternalLeading(other.ExternalLeading)
			{
			}

	};
}

namespace kxf
{
	using FontMetrics = Drawing::BasicFontMetrics<int>;
	using FontMetricsF = Drawing::BasicFontMetrics<float>;
	using FontMetricsD = Drawing::BasicFontMetrics<double>;
}
