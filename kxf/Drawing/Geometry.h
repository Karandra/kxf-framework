#pragma once
#include "Common.h"
#include "Angle.h"
#include "kxf/Core/Common.h"
#include "kxf/Serialization/BinarySerializer.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/Numeric.h"
#include <cmath>
class wxSize;
class wxPoint;
class wxRealPoint;
class wxPoint2DInt;
class wxPoint2DDouble;
class wxRect;
class wxRect2DInt;
class wxRect2DDouble;

namespace kxf::Geometry
{
	constexpr int DefaultCoord = wxDefaultCoord;

	enum class OutCode: uint32_t
	{
		Inside = 0,
		OutLeft = 1 << 0,
		OutRight = 1 << 1,
		OutTop = 1 << 2,
		OutBottom = 1 << 3
	};
}
namespace kxf
{
	KxFlagSet_Declare(Geometry::OutCode);
}

namespace kxf::Geometry
{
	template<class TDerived_, class TValue_> requires(std::is_arithmetic_v<TValue_>)
	class OrderedPairTemplate
	{
		static_assert(std::is_arithmetic_v<TValue_>, "arithmetic type required");

		public:
			using TDerived = TDerived_;
			using TValue = TValue_;

		protected:
			TValue m_X = 0;
			TValue m_Y = 0;

		protected:
			constexpr TDerived& Self() noexcept
			{
				return static_cast<TDerived&>(*this);
			}
			constexpr const TDerived& Self() const noexcept
			{
				return static_cast<const TDerived&>(*this);
			}

		public:
			constexpr OrderedPairTemplate() noexcept = default;
			constexpr OrderedPairTemplate(TValue x, TValue y) noexcept
				:m_X(x), m_Y(y)
			{
			}
			constexpr OrderedPairTemplate(const OrderedPairTemplate&) noexcept = default;
			constexpr OrderedPairTemplate(OrderedPairTemplate&&) noexcept = default;

		public:
			constexpr TDerived Clone() const noexcept
			{
				return Self();
			}
			constexpr bool IsNegative() const noexcept
			{
				return m_X < 0 && m_Y < 0;
			}
			constexpr bool IsPositive() const noexcept
			{
				return m_X > 0 && m_Y > 0;
			}
			constexpr bool IsAnyComponentNegative() const noexcept
			{
				return m_X < 0 || m_Y < 0;
			}
			constexpr bool IsAnyComponentPositive() const noexcept
			{
				return m_X > 0 || m_Y > 0;
			}
			constexpr bool IsFullySpecified() const noexcept
			{
				return m_X != DefaultCoord && m_Y != DefaultCoord;
			}
			constexpr TDerived& SetDefaults(const TDerived& other) noexcept
			{
				if (m_X == DefaultCoord)
				{
					m_X = other.m_X;
				}
				if (m_Y == DefaultCoord)
				{
					m_Y = other.m_Y;
				}
				return Self();
			}
			constexpr TDerived& SwapComponents() noexcept
			{
				std::swap(m_X, m_Y);
				return Self();
			}

			constexpr TValue GetX() const noexcept
			{
				return m_X;
			}
			constexpr TDerived& SetX(TValue x) noexcept
			{
				m_X = x;
				return Self();
			}

			constexpr TValue GetY() const noexcept
			{
				return m_Y;
			}
			constexpr TDerived& SetY(TValue y) noexcept
			{
				m_Y = y;
				return Self();
			}

			constexpr TValue& X() noexcept
			{
				return m_X;
			}
			constexpr TValue& Y() noexcept
			{
				return m_Y;
			}

			template<class TOrderedPair>
			constexpr TOrderedPair ConvertCast() const noexcept
			{
				using T = typename TOrderedPair::TValue;
				return {static_cast<T>(m_X), static_cast<T>(m_Y)};
			}

			template<class TOrderedPair> requires(std::is_floating_point_v<TValue> && std::is_integral_v<typename TOrderedPair::TValue>)
			constexpr TOrderedPair ConvertRound() const noexcept
			{
				using T = typename TOrderedPair::TValue;
				return {static_cast<T>(std::round(m_X)), static_cast<T>(std::round(m_Y))};
			}

			template<class TOrderedPair> requires(std::is_floating_point_v<TValue>&& std::is_integral_v<typename TOrderedPair::TValue>)
			constexpr TOrderedPair ConvertCeil() const noexcept
			{
				using T = typename TOrderedPair::TValue;
				return {static_cast<T>(std::ceil(m_X)), static_cast<T>(std::ceil(m_Y))};
			}

			template<class TOrderedPair> requires(std::is_floating_point_v<TValue>&& std::is_integral_v<typename TOrderedPair::TValue>)
			constexpr TOrderedPair ConvertFloor() const noexcept
			{
				using T = typename TOrderedPair::TValue;
				return {static_cast<T>(std::floor(m_X)), static_cast<T>(std::floor(m_Y))};
			}

		public:
			constexpr TDerived& Scale(double x, double y) noexcept
			{
				m_X *= x;
				m_Y *= y;

				return Self();
			}
			constexpr TDerived& Scale(double factor) noexcept
			{
				return Scale(factor, factor);
			}
			constexpr TDerived GetScaled(double x, double y) const noexcept
			{
				return Clone().Scale(x, y);
			}
			constexpr TDerived GetScaled(double factor) const noexcept
			{
				return Clone().Scale(factor);
			}

			constexpr TDerived& IncBy(TValue x, TValue y) noexcept
			{
				m_X += x;
				m_Y += y;

				return Self();
			}
			constexpr TDerived& IncBy(TValue value) noexcept
			{
				return IncBy(value, value);
			}
			constexpr TDerived& IncBy(const TDerived& other) noexcept
			{
				return IncBy(other.m_X, other.m_Y);
			}

			constexpr TDerived& DecBy(TValue x, TValue y) noexcept
			{
				return IncBy(-x, -y);
			}
			constexpr TDerived& DecBy(TValue value) noexcept
			{
				return DecBy(value, value);
			}
			constexpr TDerived& DecBy(const TDerived& other) noexcept
			{
				return DecBy(other.m_X, other.m_Y);
			}

			constexpr TDerived& IncTo(const TDerived& other) noexcept
			{
				if (other.m_X > m_X)
				{
					m_X = other.m_X;
				}
				if (other.m_Y > m_Y)
				{
					m_Y = other.m_Y;
				}
				return Self();
			}
			constexpr TDerived& IncToIfSpecified(const TDerived& other) noexcept
			{
				if (other.m_X != DefaultCoord && other.m_X > m_X)
				{
					m_X = other.m_X;
				}
				if (other.m_Y != DefaultCoord && other.m_Y > m_Y)
				{
					m_Y = other.m_Y;
				}
				return Self();
			}

			constexpr TDerived& DecTo(const TDerived& other) noexcept
			{
				if (other.m_X < m_X)
				{
					m_X = other.m_X;
				}
				if (other.m_Y < m_Y)
				{
					m_Y = other.m_Y;
				}
				return Self();
			}
			constexpr TDerived& DecToIfSpecified(const TDerived& other) noexcept
			{
				if (other.m_X != DefaultCoord && other.m_X < m_X)
				{
					m_X = other.m_X;
				}
				if (other.m_Y != DefaultCoord && other.m_Y < m_Y)
				{
					m_Y = other.m_Y;
				}
				return Self();
			}

		public:
			constexpr double GetVectorLength() const noexcept
			{
				return std::sqrt(static_cast<double>(m_X * m_X) + static_cast<double>(m_Y + m_Y));
			}
			constexpr TDerived& SetVectorLength(double length) noexcept
			{
				const double original = GetVectorLength();
				m_X = m_X * length / original;
				m_Y = m_Y * length / original;

				return Self();
			}
			constexpr TDerived& Normalize() const noexcept
			{
				return SetVectorLength(1.0);
			}

			constexpr Angle GetVectorAngle() const noexcept
			{
				if (Utility::AlmostZero(m_X))
				{
					return m_Y >= 0 ? Angle::FromDegrees(90) : Angle::FromDegrees(270);
				}
				else if (Utility::AlmostZero(m_Y))
				{
					return m_X >= 0 ? Angle::FromDegrees(0) : Angle::FromDegrees(180);
				}
				else
				{
					double deg = std::atan2(m_Y, m_X) * 180.0 / M_PI;
					if (deg < 0)
					{
						deg += 360;
					}
					return Angle::FromDegrees(deg);
				}
			}
			constexpr TDerived& SetVectorAngle(Angle angle) noexcept
			{
				const double length = GetVectorLength();
				const double degrees = angle.ToDegrees();

				m_X = length * std::cos(degrees / 180.0 * M_PI);
				m_Y = length * std::sin(degrees / 180.0 * M_PI);
				return Self();
			}

			constexpr double GetDistance(const TDerived& other) const noexcept
			{
				return std::sqrt(GetDistanceSquare(other));
			}
			constexpr double GetDistanceSquare(const TDerived& other) const noexcept
			{
				return (other.m_X - m_X) * (other.m_X - m_X) + (other.m_Y - m_Y) * (other.m_Y - m_Y);
			}

			constexpr double GetDotProduct(const TDerived& vector) const noexcept
			{
				return m_X * vector.m_X + m_Y * vector.m_Y;
			}
			constexpr double GetCrossProduct(const TDerived& vector) const noexcept
			{
				return m_X * vector.m_Y - vector.m_X * m_Y;
			}

		public:
			constexpr OrderedPairTemplate& operator=(const OrderedPairTemplate&) noexcept = default;
			constexpr OrderedPairTemplate& operator=(OrderedPairTemplate&&) noexcept = default;

			constexpr auto operator<=>(const OrderedPairTemplate&) const noexcept = default;

			constexpr TDerived& operator+=(const TDerived& other) noexcept
			{
				m_X += other.m_X;
				m_Y += other.m_Y;

				return Self();
			}
			constexpr TDerived& operator-=(const TDerived& other) noexcept
			{
				m_X -= other.m_X;
				m_Y -= other.m_Y;

				return Self();
			}
			constexpr TDerived& operator*=(TDerived n) noexcept
			{
				m_X *= n;
				m_Y *= n;

				return Self();
			}
			constexpr TDerived& operator/=(TDerived n) noexcept
			{
				m_X /= n;
				m_Y /= n;

				return Self();
			}
	};

	template<class TDerived, class TValue>
	constexpr TDerived operator-(const OrderedPairTemplate<TDerived, TValue>& value) noexcept
	{
		return {-value.GetX(), -value.GetY()};
	}

	template<class TDerived, class TValue>
	constexpr TDerived operator+(const OrderedPairTemplate<TDerived, TValue>& left, const OrderedPairTemplate<TDerived, TValue>& right) noexcept
	{
		return {left.GetX() + right.GetX(), left.GetY() + right.GetY()};
	}

	template<class TDerived, class TValue>
	constexpr TDerived operator-(const OrderedPairTemplate<TDerived, TValue>& left, const OrderedPairTemplate<TDerived, TValue>& right) noexcept
	{
		return {left.GetX() - right.GetX(), left.GetY() - right.GetY()};
	}

	template<class TDerived, class TValue>
	constexpr TDerived operator*(const OrderedPairTemplate<TDerived, TValue>& left, const OrderedPairTemplate<TDerived, TValue>& right) noexcept
	{
		return {left.GetX() * right.GetX(), left.GetY() * right.GetY()};
	}

	template<class TDerived, class TValue>
	constexpr TDerived operator/(const OrderedPairTemplate<TDerived, TValue>& left, const OrderedPairTemplate<TDerived, TValue>& right) noexcept
	{
		return {left.GetX() / right.GetX(), left.GetY() / right.GetY()};
	}

	template<class TDerived, class TValue, class T> requires(std::is_arithmetic_v<T>)
	constexpr TDerived operator*(const OrderedPairTemplate<TDerived, TValue>& left, T right) noexcept
	{
		return {static_cast<TValue>(left.GetX() * right), static_cast<TValue>(left.GetY() * right)};
	}

	template<class TDerived, class TValue, class T> requires(std::is_arithmetic_v<T>)
	constexpr TDerived operator/(const OrderedPairTemplate<TDerived, TValue>& left, T right) noexcept
	{
		return {static_cast<TValue>(left.GetX() / right), static_cast<TValue>(left.GetY() / right)};
	}

	template<class TDerived, class TValue, class T> requires(std::is_arithmetic_v<T>)
	constexpr TDerived operator*(T left, const OrderedPairTemplate<TDerived, TValue>& right) noexcept
	{
		return {static_cast<TValue>(left * right.GetX()), static_cast<TValue>(left * right.GetY())};
	}

	template<class TDerived, class TValue, class T> requires(std::is_arithmetic_v<T>)
	constexpr TDerived operator/(T left, const OrderedPairTemplate<TDerived, TValue>& right) noexcept
	{
		return {static_cast<TValue>(left / right.GetX()), static_cast<TValue>(left / right.GetY())};
	}
}

namespace kxf::Geometry
{
	template<class TDerived_, class TValue_, class TPoint_, class TSize_>
	class RectTemplate
	{
		public:
			using TDerived = TDerived_;
			using TValue = TValue_;
			using TPoint = TPoint_;
			using TSize = TSize_;

		private:
			template<class T>
			using TSelf = RectTemplate<T, TValue_, TPoint_, TSize_>;

		protected:
			TValue m_X = 0;
			TValue m_Y = 0;
			TValue m_Width = 0;
			TValue m_Height = 0;

		protected:
			constexpr TDerived& Self() noexcept
			{
				return static_cast<TDerived&>(*this);
			}
			constexpr const TDerived& Self() const noexcept
			{
				return static_cast<const TDerived&>(*this);
			}

		public:
			constexpr RectTemplate() noexcept = default;
			constexpr RectTemplate(TValue x, TValue y, TValue width, TValue height) noexcept
				:m_X(x), m_Y(y), m_Width(width), m_Height(height)
			{
			}
			constexpr RectTemplate(const TPoint& topLeft, const TPoint& bottomRight) noexcept
				:m_X(topLeft.GetX()), m_Y(topLeft.GetY()), m_Width(bottomRight.GetX() - topLeft.GetX()), m_Height(bottomRight.GetY() - topLeft.GetY())
			{
				if (m_Width < 0)
				{
					m_Width = -m_Width;
					m_X = topLeft.GetX();
				}
				m_Width++;

				if (m_Height < 0)
				{
					m_Height = -m_Height;
					m_Y = topLeft.GetY();
				}
				m_Height++;
			}
			constexpr RectTemplate(const TPoint& position, const TSize& size) noexcept
				:m_X(position.GetX()), m_Y(position.GetY()), m_Width(size.GetWidth()), m_Height(size.GetHeight())
			{
			}
			constexpr RectTemplate(const TSize& size) noexcept
				:m_Width(size.GetWidth()), m_Height(size.GetHeight())
			{
			}
			constexpr RectTemplate(const RectTemplate&) noexcept = default;

		public:
			constexpr bool IsEmpty() const noexcept
			{
				return m_Width <= 0 || m_Height <= 0;
			}
			constexpr bool IsFullySpecified() const noexcept
			{
				return m_X != DefaultCoord && m_Y != DefaultCoord && m_Width != DefaultCoord && m_Height != DefaultCoord;
			}
			constexpr TDerived Clone() const noexcept
			{
				return Self();
			}

			constexpr TValue GetX() const noexcept
			{
				return m_X;
			}
			constexpr TDerived& SetX(TValue x) noexcept
			{
				m_X = x;
				return Self();
			}

			constexpr TValue GetY() const noexcept
			{
				return m_Y;
			}
			constexpr TDerived& SetY(TValue y) noexcept
			{
				m_Y = y;
				return Self();
			}

			constexpr TValue GetWidth() const noexcept
			{
				return m_Width;
			}
			constexpr TDerived& SetWidth(TValue width) noexcept
			{
				m_Width = width;
				return Self();
			}

			constexpr TValue GetHeight() const noexcept
			{
				return m_Height;
			}
			constexpr TDerived& SetHeight(TValue height) noexcept
			{
				m_Height = height;
				return Self();
			}

			constexpr TValue& X() noexcept
			{
				return m_X;
			}
			constexpr TValue& Y() noexcept
			{
				return m_Y;
			}
			constexpr TValue& Width() noexcept
			{
				return m_Width;
			}
			constexpr TValue& Height() noexcept
			{
				return m_Height;
			}

			template<class TRect>
			constexpr TRect ConvertCast() const noexcept
			{
				using T = typename TRect::TValue;
				return {static_cast<T>(m_X), static_cast<T>(m_Y), static_cast<T>(m_Width), static_cast<T>(m_Height)};
			}

			template<class TRect> requires(std::is_floating_point_v<TValue>&& std::is_integral_v<typename TRect::TValue>)
			constexpr TRect ConvertRound() const noexcept
			{
				using T = typename TRect::TValue;
				return {static_cast<T>(std::round(m_X)), static_cast<T>(std::round(m_Y)), static_cast<T>(std::round(m_Width)), static_cast<T>(std::round(m_Height))};
			}

			template<class TRect> requires(std::is_floating_point_v<TValue>&& std::is_integral_v<typename TRect::TValue>)
			constexpr TRect ConvertCeil() const noexcept
			{
				using T = typename TRect::TValue;
				return {static_cast<T>(std::ceil(m_X)), static_cast<T>(std::ceil(m_Y)), static_cast<T>(std::ceil(m_Width)), static_cast<T>(std::ceil(m_Height))};
			}

			template<class TRect> requires(std::is_floating_point_v<TValue>&& std::is_integral_v<typename TRect::TValue>)
			constexpr TRect ConvertFloor() const noexcept
			{
				using T = typename TRect::TValue;
				return {static_cast<T>(std::floor(m_X)), static_cast<T>(std::floor(m_Y)), static_cast<T>(std::floor(m_Width)), static_cast<T>(std::floor(m_Height))};
			}

		public:
			constexpr TPoint GetPosition() const noexcept
			{
				return {m_X, m_Y};
			}
			constexpr TDerived& SetPosition(const TPoint& value) noexcept
			{
				m_X = value.GetX();
				m_Y = value.GetY();
				return Self();
			}

			constexpr TSize GetSize() const noexcept
			{
				return {m_Width, m_Height};
			}
			constexpr TDerived& SetSize(const TSize& value) noexcept
			{
				m_Width = value.GetWidth();
				m_Height = value.GetHeight();
				return Self();
			}

			constexpr TValue GetLeft() const noexcept
			{
				return m_X;
			}
			constexpr TDerived& SetLeft(TValue left) noexcept
			{
				m_X = left;
				return Self();
			}
			constexpr TDerived& MoveLeftBy(TValue value) noexcept
			{
				m_X += value;
				return Self();
			}
			constexpr TDerived& MoveLeftTo(TValue value) noexcept
			{
				m_X = value;
				return Self();
			}

			constexpr TValue GetTop() const noexcept
			{
				return m_Y;
			}
			constexpr TDerived& SetTop(TValue top) noexcept
			{
				m_Y = top;
				return Self();
			}
			constexpr TDerived& MoveTopBy(TValue left) noexcept
			{
				m_Y += left;
				return Self();
			}
			constexpr TDerived& MoveTopTo(TValue value) noexcept
			{
				m_Y = value;
				return Self();
			}

			constexpr TValue GetRight() const noexcept
			{
				return m_X + m_Width - 1;
			}
			constexpr TDerived& SetRight(TValue right) noexcept
			{
				m_Width = right - m_X + 1;
				return Self();
			}
			constexpr TDerived& MoveRightBy(TValue value) noexcept
			{
				m_Width += value;
				return Self();
			}
			constexpr TDerived& MoveRightTo(TValue value) noexcept
			{
				m_X = value - m_Width;
				return Self();
			}

			constexpr TValue GetBottom() const noexcept
			{
				return m_Y + m_Height - 1;
			}
			constexpr TDerived& SetBottom(TValue bottom) noexcept
			{
				m_Height = bottom - m_Y + 1;
				return Self();
			}
			constexpr TDerived& MoveBottomBy(TValue value) noexcept
			{
				m_Height += value;
				return Self();
			}
			constexpr TDerived& MoveBottomTo(TValue value) noexcept
			{
				m_Y = value - m_Height;
				return Self();
			}

		public:
			constexpr TPoint GetTopLeft() const noexcept
			{
				return GetPosition();
			}
			constexpr TDerived& SetTopLeft(const TPoint& value) noexcept
			{
				return SetPosition(value);
			}
			constexpr TPoint GetLeftTop() const noexcept
			{
				return GetTopLeft();
			}
			constexpr TDerived& SetLeftTop(const TPoint& value) noexcept
			{
				return SetTopLeft(value);
			}

			constexpr TPoint GetBottomRight() const noexcept
			{
				return {GetRight(), GetBottom()};
			}
			constexpr TDerived& SetBottomRight(const TPoint& value) noexcept
			{
				SetRight(value.GetX());
				SetBottom(value.GetY());
				return Self();
			}
			constexpr TPoint GetRightBottom() const noexcept
			{
				return GetBottomRight();
			}
			constexpr TDerived& SetRightBottom(const TPoint& value) noexcept
			{
				return SetBottomRight(value);
			}

			constexpr TPoint GetTopRight() const noexcept
			{
				return {GetRight(), GetTop()};
			}
			constexpr TDerived& SetTopRight(const TPoint& value) noexcept
			{
				SetRight(value.GetX());
				SetTop(value.GetY());
				return Self();
			}
			constexpr TPoint GetRightTop() const noexcept
			{
				return GetTopRight();
			}
			constexpr TDerived& SetRightTop(const TPoint& value) noexcept
			{
				return SetTopRight(value);
			}

			constexpr TPoint GetBottomLeft() const noexcept
			{
				return {GetLeft(), GetBottom()};
			}
			constexpr TDerived& SetBottomLeft(const TPoint& value) noexcept
			{
				SetLeft(value.GetX());
				SetBottom(value.GetY());
				return Self();
			}
			constexpr TPoint GetLeftBottom() const noexcept
			{
				return GetBottomLeft();
			}
			constexpr TDerived& SetLeftBottom(const TPoint& value) noexcept
			{
				return SetBottomLeft(value);
			}

			constexpr bool Contains(TValue x, TValue y) const noexcept
			{
				return (x >= m_X) && (y >= m_Y) && (y - m_Y < m_Height) && (x - m_X < m_Width);
			}
			constexpr bool Contains(const TPoint& value) const noexcept
			{
				return Contains(value.GetX(), value.GetY());
			}
			constexpr bool Contains(const TDerived& other) noexcept
			{
				return Contains(other.GetTopLeft()) && Contains(other.GetBottomRight());
			}
			constexpr TDerived& IncludePoint(const TPoint& point) noexcept
			{
				const auto x = point.GetX();
				const auto y = point.GetY();

				if (x < m_X)
				{
					m_X = x;
				}
				if (y < m_Y)
				{
					m_Y = y;
				}
				if (x > GetRight())
				{
					SetRight(x);
				}
				if (y > GetBottom())
				{
					SetBottom(y);
				}

				return Self();
			}

			constexpr TDerived& Intersect(const TDerived& other) noexcept
			{
				TValue x2 = GetRight();
				TValue y2 = GetBottom();

				if (m_X < other.m_X)
				{
					m_X = other.m_X;
				}
				if (m_Y < other.m_Y)
				{
					m_Y = other.m_Y;
				}

				if (x2 > other.GetRight())
				{
					x2 = other.GetRight();
				}
				if (y2 > other.GetBottom())
				{
					y2 = other.GetBottom();
				}

				m_Width = x2 - m_X + 1;
				m_Height = y2 - m_Y + 1;
				if (m_Width <= 0 || m_Height <= 0)
				{
					m_Width = 0;
					m_Height = 0;
				}

				return Self();
			}
			constexpr TDerived GetIntersection(const TDerived& other) const noexcept
			{
				return Clone().Intersect(other);
			}
			constexpr bool Intersects(const TDerived& other) const noexcept
			{
				// If there is no intersection, both width and height are 0
				return GetIntersection(other).GetWidth() != 0;
			}

			constexpr TDerived& Union(const TDerived& other) noexcept
			{
				// Ignore empty rectangles: union with an empty rectangle shouldn't extend this one to (0, 0)
				if (m_Width == 0 || m_Height == 0)
				{
					*this = other;
				}
				else if (other.m_Width != 0 && other.m_Height != 0)
				{
					const TValue x1 = std::min(m_X, other.m_X);
					const TValue y1 = std::min(m_Y, other.m_Y);
					const TValue y2 = std::max(m_Y + m_Height, other.m_Height + other.m_Y);
					const TValue x2 = std::max(m_X + m_Width, other.m_Width + other.m_X);

					m_X = x1;
					m_Y = y1;
					m_Width = x2 - x1;
					m_Height = y2 - y1;
				}
				//else: we're not empty and 'other' is empty

				return Self();
			}
			constexpr TDerived GetUnion(const TDerived& other) const noexcept
			{
				return Clone().Union(other);
			}

			constexpr TDerived CenterIn(const TDerived& other, FlagSet<Orientation> direction = Orientation::Both) const noexcept
			{
				return
				{
					direction & Orientation::Horizontal ? other.m_X + (other.m_Width - m_Width) / 2 : m_X,
					direction & Orientation::Vertical ? other.m_Y + (other.m_Height - m_Height) / 2 : m_Y,
					m_Width,
					m_Height
				};
			}
			constexpr TDerived& Move(TValue dx, TValue dy) noexcept
			{
				m_X += dx;
				m_Y += dy;

				return Self();
			}
			constexpr TDerived& Move(const TPoint& offset) noexcept
			{
				return Move(offset.GetX(), offset.GetY());
			}

			constexpr TDerived& Inflate(TValue dx, TValue dy) noexcept
			{
				if (-2 * dx > m_Width)
				{
					// Don't allow deflate to eat more width than we have, a well-defined rectangle cannot have negative width.
					m_X += m_Width / 2;
					m_Width = 0;
				}
				else
				{
					// The inflate is valid.
					m_X -= dx;
					m_Width += 2 * dx;
				}

				if (-2 * dy > m_Height)
				{
					// Don't allow deflate to eat more height than we have, a well-defined rectangle cannot have negative height.
					m_Y += m_Height / 2;
					m_Height = 0;
				}
				else
				{
					// The inflate is valid.
					m_Y -= dy;
					m_Height += 2 * dy;
				}
				return Self();
			}
			constexpr TDerived& Inflate(TValue dxy) noexcept
			{
				return Inflate(dxy, dxy);
			}
			constexpr TDerived& Inflate(const TSize& dxy) noexcept
			{
				return Inflate(dxy.GetWidth(), dxy.GetHeight());
			}

			constexpr TDerived& Deflate(TValue dx, TValue dy) noexcept
			{
				return Inflate(-dx, -dy);
			}
			constexpr TDerived& Deflate(TValue dxy) noexcept
			{
				return Inflate(-dxy, -dxy);
			}
			constexpr TDerived& Deflate(const TSize& dxy) noexcept
			{
				return Inflate(-dxy.GetWidth(), -dxy.GetHeight());
			}

		public:
			constexpr bool EqualSize(const TDerived& other) noexcept
			{
				return GetSize() == other.GetSize();
			}
			constexpr void ConstrainTo(const TDerived& other) noexcept
			{
				if (GetLeft() < other.GetLeft())
				{
					SetLeft(other.GetLeft());
				}
				if (GetRight() > other.GetRight())
				{
					SetRight(other.GetRight());
				}
				if (GetBottom() > other.GetBottom())
				{
					SetBottom(other.GetBottom());
				}
				if (GetTop() < other.GetTop())
				{
					SetTop(other.GetTop());
				}
			}

			constexpr TPoint GetCenter() const noexcept
			{
				return {m_X + m_Width / 2, m_Y + m_Height / 2};
			}
			constexpr TDerived& MoveCenterTo(const TPoint& center) noexcept
			{
				m_X += center.GetX() - (m_X + m_Width / 2);
				m_Y += center.GetY() - (m_Y + m_Height / 2);

				return Self();
			}
			constexpr FlagSet<OutCode> GetOutCode(const TPoint& point) const noexcept
			{
				FlagSet<OutCode> outCode;
				outCode.Add(OutCode::OutLeft, point.GetX() < m_X);
				outCode.Add(OutCode::OutRight, point.GetX() > m_X + m_Width);
				outCode.Add(OutCode::OutTop, point.GetY() < m_Y);
				outCode.Add(OutCode::OutBottom, point.GetY() > m_Y + m_Height);

				return outCode;
			}

			constexpr TDerived& Inset(const TPoint& point) noexcept
			{
				m_X += point.GetX();
				m_Y += point.GetY();
				m_Width -= 2 * point.GetX();
				m_Height -= 2 * point.GetY();

				return Self();
			}
			constexpr TDerived& Inset(const TDerived& other) noexcept
			{
				m_X += other.GetLeft();
				m_Y += other.GetTop();
				m_Width -= other.GetLeft() + other.GetRight();
				m_Height -= other.GetTop() + other.GetBottom();

				return Self();
			}

			constexpr TDerived& Scale(double dx, double dy, double dw, double dh) noexcept
			{
				m_X *= dx;
				m_Y *= dy;
				m_Width *= dw;
				m_Height *= dh;

				return Self();
			}
			constexpr TDerived& Scale(double dxy, double dwh) noexcept
			{
				return Scale(dxy, dxy, dwh, dwh);
			}
			constexpr TDerived& Scale(double scale) noexcept
			{
				return Scale(scale, scale);
			}
			constexpr TDerived& ScaleByRatio(double numerator, double denominator) noexcept
			{
				if (denominator != 0)
				{
					const double value = numerator / denominator;

					m_X *= value;
					m_Y *= value;
					m_Width *= value;
					m_Height *= value;
				}
				else
				{
					*this = {};
				}
				return Self();
			}
			constexpr TPoint Interpolate(double widthFactor, double heightFactor) const noexcept
			{
				return {m_X + m_Width * widthFactor, m_Y + m_Height * heightFactor};
			}

		public:
			constexpr auto operator<=>(const RectTemplate&) const noexcept = default;

			constexpr TDerived operator+(const TDerived& other) const noexcept
			{
				// Like Union() but don't treat empty rectangles specially
				const TValue x1 = std::min(m_X, other.m_X);
				const TValue y1 = std::min(m_Y, other.m_Y);
				const TValue y2 = std::max(m_Y + m_Height, other.m_Height + other.m_Y);
				const TValue x2 = std::max(m_X + m_Width, other.m_Width + other.m_X);

				return {x1, y1, x2 - x1, y2 - y1};
			}
			constexpr TDerived& operator+=(const TDerived& other) noexcept
			{
				Self() = Clone() + other;
				return Self();
			}

			constexpr TDerived operator*(const TDerived& other) const noexcept
			{
				// In original wxWidgets' code it's the same as 'operator+' for some reason
				return Clone() + other;
			}
			constexpr TDerived& operator*=(const TDerived& other) noexcept
			{
				Self() = Clone() * other;
				return Self();
			}
	};
}

namespace kxf::Geometry
{
	template<class TValue_>
	class BasicPoint final: public OrderedPairTemplate<BasicPoint<TValue_>, TValue_>
	{
		public:
			using TValue = TValue_;

		private:
			using TBase = OrderedPairTemplate<BasicPoint<TValue_>, TValue_>;

		public:
			static constexpr BasicPoint UnspecifiedPosition() noexcept
			{
				return BasicPoint(Geometry::DefaultCoord, Geometry::DefaultCoord);
			}

			template<class T>
			static constexpr BasicPoint FromSize(const T& size) noexcept
			{
				return BasicPoint(size.GetWidth(), size.GetHeight());
			}

		public:
			using TBase::OrderedPairTemplate;

			explicit BasicPoint(const wxPoint& other) noexcept;
			explicit BasicPoint(const wxRealPoint& other) noexcept;
			explicit BasicPoint(const wxPoint2DInt& other) noexcept;
			explicit BasicPoint(const wxPoint2DDouble& other) noexcept;

			template<class T>
			constexpr BasicPoint(const BasicPoint<T>& other) noexcept
				:TBase(other.GetX(), other.GetY())
			{
			}

		public:
			constexpr auto operator<=>(const BasicPoint& other) const noexcept = default;
			constexpr bool operator==(const BasicPoint& other) const noexcept = default;

			operator wxPoint() const noexcept;
			operator wxRealPoint() const noexcept;
			operator wxPoint2DInt() const noexcept;
			operator wxPoint2DDouble() const noexcept;
	};

	template<class TValue_>
	class BasicSize final: public OrderedPairTemplate<BasicSize<TValue_>, TValue_>
	{
		public:
			using TValue = TValue_;

		private:
			using TBase = OrderedPairTemplate<BasicSize<TValue_>, TValue_>;

		public:
			static constexpr BasicSize UnspecifiedSize() noexcept
			{
				return BasicSize(Geometry::DefaultCoord, Geometry::DefaultCoord);
			}

			template<class T>
			static constexpr BasicSize FromPoint(const T& point) noexcept
			{
				return BasicSize(point.GetX(), point.GetY());
			}

		private:
			using TBase::X;
			using TBase::Y;
			using TBase::SetX;
			using TBase::SetY;
			using TBase::GetX;
			using TBase::GetY;

		public:
			using TBase::OrderedPairTemplate;

			explicit BasicSize(const wxSize& other) noexcept;

			template<class T>
			constexpr BasicSize(const BasicSize<T>& other) noexcept
				:TBase(other.GetWidth(), other.GetHeight())
			{
			}

		public:
			constexpr TValue GetWidth() const noexcept
			{
				return  this->GetX();
			}
			constexpr BasicSize& SetWidth(TValue width) noexcept
			{
				return this->SetX(width);
			}

			constexpr TValue GetHeight() const noexcept
			{
				return this->GetY();
			}
			constexpr BasicSize& SetHeight(TValue height) noexcept
			{
				return  this->SetY(height);
			}

			constexpr TValue& Width() noexcept
			{
				return this->X();
			}
			constexpr TValue& Height() noexcept
			{
				return this->Y();
			}

		public:
			constexpr auto operator<=>(const BasicSize& other) const noexcept = default;
			constexpr bool operator==(const BasicSize& other) const noexcept = default;

			operator wxSize() const noexcept;
	};

	template<class TValue_>
	class BasicRect final: public RectTemplate<BasicRect<TValue_>, TValue_, BasicPoint<TValue_>, BasicSize<TValue_>>
	{
		public:
			using TValue = TValue_;

		public:
			static constexpr BasicRect UnspecifiedRect() noexcept
			{
				return BasicRect(Geometry::DefaultCoord, Geometry::DefaultCoord, Geometry::DefaultCoord, Geometry::DefaultCoord);
			}

		private:
			using TBase = RectTemplate<BasicRect<TValue_>, TValue_, BasicPoint<TValue_>, BasicSize<TValue_>>;

		public:
			using TBase::RectTemplate;

			explicit BasicRect(const wxRect& other) noexcept;
			explicit BasicRect(const wxRect2DInt& other) noexcept;
			explicit BasicRect(const wxRect2DDouble& other) noexcept;

			template<class T>
			constexpr BasicRect(const BasicRect<T>& other) noexcept
				:TBase(other.GetX(), other.GetY(), other.GetWidth(), other.GetHeight())
			{
			}

		public:
			constexpr auto operator<=>(const BasicRect& other) const noexcept = default;
			constexpr bool operator==(const BasicRect& other) const noexcept = default;

			operator wxRect() const noexcept;
			operator wxRect2DInt() const noexcept;
			operator wxRect2DDouble() const noexcept;
	};
}

namespace kxf
{
	using Point = Geometry::BasicPoint<int>;
	using Size = Geometry::BasicSize<int>;
	using Rect = Geometry::BasicRect<int>;

	using PointF = Geometry::BasicPoint<float>;
	using SizeF = Geometry::BasicSize<float>;
	using RectF = Geometry::BasicRect<float>;

	using PointD = Geometry::BasicPoint<double>;
	using SizeD = Geometry::BasicSize<double>;
	using RectD = Geometry::BasicRect<double>;
}

namespace kxf
{
	std::strong_ordering operator<=>(const Point& left, const wxPoint& right) noexcept;
	bool operator==(const Point& left, const wxPoint& right) noexcept;

	std::strong_ordering operator<=>(const Point& left, const wxPoint2DInt& right) noexcept;
	bool operator==(const Point& left, const wxPoint2DInt& right) noexcept;

	std::partial_ordering operator<=>(const PointD& left, const wxRealPoint& right) noexcept;
	bool operator==(const PointD& left, const wxRealPoint& right) noexcept;

	std::partial_ordering operator<=>(const PointD& left, const wxPoint2DDouble& right) noexcept;
	bool operator==(const PointD& left, const wxPoint2DDouble& right) noexcept;
}
namespace kxf
{
	std::strong_ordering operator<=>(const Size& left, const wxSize& right) noexcept;
	bool operator==(const Size& left, const wxSize& right) noexcept;
}
namespace kxf
{
	std::strong_ordering operator<=>(const Rect& left, const wxRect& right) noexcept;
	bool operator==(const Rect& left, const wxRect& right) noexcept;

	std::strong_ordering operator<=>(const Rect& left, const wxRect2DInt& right) noexcept;
	bool operator==(const Rect& left, const wxRect2DInt& right) noexcept;

	std::partial_ordering operator<=>(const RectD& left, const wxRect2DDouble& right) noexcept;
	bool operator==(const RectD& left, const wxRect2DDouble& right) noexcept;
}

namespace kxf
{
	template<class T>
	struct BinarySerializer<Geometry::BasicPoint<T>> final
	{
		private:
			using TPoint = Geometry::BasicPoint<T>;

		public:
			uint64_t Serialize(IOutputStream& stream, const TPoint& value) const
			{
				return Serialization::WriteObject(stream, value.GetX()) + Serialization::WriteObject(stream, value.GetY());
			}
			uint64_t Deserialize(IInputStream& stream, TPoint& value) const
			{
				return Serialization::ReadObject(stream, value.X()) + Serialization::ReadObject(stream, value.Y());
			}
	};

	template<class T>
	struct BinarySerializer<Geometry::BasicSize<T>> final
	{
		private:
			using TSize = Geometry::BasicSize<T>;

		public:
			uint64_t Serialize(IOutputStream& stream, const TSize& value) const
			{
				return Serialization::WriteObject(stream, value.GetWidth()) + Serialization::WriteObject(stream, value.GetHeight());
			}
			uint64_t Deserialize(IInputStream& stream, TSize& value) const
			{
				return Serialization::ReadObject(stream, value.Width()) + Serialization::ReadObject(stream, value.Height());
			}
	};

	template<class T>
	struct BinarySerializer<Geometry::BasicRect<T>> final
	{
		private:
			using TRect = Geometry::BasicRect<T>;

		public:
			uint64_t Serialize(IOutputStream& stream, const TRect& value) const
			{
				return Serialization::WriteObject(stream, value.GetX()) +
					Serialization::WriteObject(stream, value.GetY()) +
					Serialization::WriteObject(stream, value.GetWidth()) +
					Serialization::WriteObject(stream, value.GetHeight());
			}
			uint64_t Deserialize(IInputStream& stream, TRect& value) const
			{
				return Serialization::ReadObject(stream, value.X()) +
					Serialization::ReadObject(stream, value.Y()) +
					Serialization::ReadObject(stream, value.Width()) +
					Serialization::ReadObject(stream, value.Height());
			}
	};
}

namespace std
{
	// Point
	template<class TValue>
	struct tuple_size<kxf::Geometry::BasicPoint<TValue>> final: integral_constant<size_t, 2> {};

	template<size_t Index, class TValue>
	struct tuple_element<Index, kxf::Geometry::BasicPoint<TValue>> final
	{
		using type = TValue;
	};

	template<std::size_t Index, class TValue>
	constexpr std::tuple_element_t<Index, kxf::Geometry::BasicPoint<TValue>> get(const kxf::Geometry::BasicPoint<TValue>& obj) noexcept
	{
		if constexpr(Index == 0)
		{
			return obj.GetX();
		}
		else if constexpr(Index == 1)
		{
			return obj.GetY();
		}
		else
		{
			static_assert(sizeof(TValue*) == 0, "Index out of bounds for kxf::Geometry::BasicPoint<TValue>");
		}
	}

	// Size
	template<class TValue>
	struct tuple_size<kxf::Geometry::BasicSize<TValue>> final: integral_constant<size_t, 2> {};

	template<size_t Index, class TValue>
	struct tuple_element<Index, kxf::Geometry::BasicSize<TValue>> final
	{
		using type = TValue;
	};

	template<std::size_t Index, class TValue>
	constexpr std::tuple_element_t<Index, kxf::Geometry::BasicSize<TValue>> get(const kxf::Geometry::BasicSize<TValue>& obj) noexcept
	{
		if constexpr(Index == 0)
		{
			return obj.GetWidth();
		}
		else if constexpr(Index == 1)
		{
			return obj.GetHeight();
		}
		else
		{
			static_assert(sizeof(TValue*) == 0, "Index out of bounds for kxf::Geometry::BasicSize<TValue>");
		}
	}

	// Rect
	template<class TValue>
	struct tuple_size<kxf::Geometry::BasicRect<TValue>> final: integral_constant<size_t, 4> {};

	template<size_t Index, class TValue>
	struct tuple_element<Index, kxf::Geometry::BasicRect<TValue>> final
	{
		using type = TValue;
	};

	template<std::size_t Index, class TValue>
	constexpr std::tuple_element_t<Index, kxf::Geometry::BasicRect<TValue>> get(const kxf::Geometry::BasicRect<TValue>& obj) noexcept
	{
		if constexpr(Index == 0)
		{
			return obj.GetX();
		}
		else if constexpr(Index == 1)
		{
			return obj.GetY();
		}
		if constexpr(Index == 2)
		{
			return obj.GetWidth();
		}
		else if constexpr(Index == 3)
		{
			return obj.GetHeight();
		}
		else
		{
			static_assert(sizeof(TValue*) == 0, "Index out of bounds for kxf::Geometry::BasicRect<TValue>");
		}
	}
}
