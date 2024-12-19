#pragma once
#include "Common.h"
#include "Angle.h"
#include "Geometry.h"
#include "kxf/Core/Common.h"
#include "kxf/Core/FlagSet.h"
#include "kxf/Serialization/BinarySerializer.h"
#include <cmath>

namespace kxf::Geometry
{
	template<class TValue_>
	requires(std::is_arithmetic_v<TValue_>)
	class BasicAffineMatrix final
	{
		template<class T>
		requires(std::is_arithmetic_v<T>)
		friend class BasicAffineMatrix;

		friend struct BinarySerializer<BasicAffineMatrix<TValue_>>;

		public:
			using TValue = TValue_;

		private:
			TValue m_11 = 1;
			TValue m_12 = 0;
			TValue m_21 = 0;
			TValue m_22 = 1;
			TValue m_tx = 0;
			TValue m_ty = 0;

		public:
			constexpr BasicAffineMatrix() noexcept = default;

			template<class T>
			requires(std::is_arithmetic_v<T>)
			constexpr BasicAffineMatrix(T m11, T m12, T m21, T m22, T tx, T ty) noexcept
				:m_11(static_cast<TValue>(m11)), m_12(static_cast<TValue>(m12)),
				m_21(static_cast<TValue>(m21)), m_22(static_cast<TValue>(m22)),
				m_tx(static_cast<TValue>(tx)), m_ty(static_cast<TValue>(ty))
			{
			}

			template<class T>
			requires(std::is_arithmetic_v<T>)
			constexpr BasicAffineMatrix(const BasicAffineMatrix<T>& other) noexcept
				:m_11(static_cast<TValue>(other.m_11)), m_12(static_cast<TValue>(other.m_12)),
				m_21(static_cast<TValue>(other.m_21)), m_22(static_cast<TValue>(other.m_22)),
				m_tx(static_cast<TValue>(other.m_tx)), m_ty(static_cast<TValue>(other.m_ty))
			{
			}

		public:
			constexpr bool IsIdentity() const noexcept
			{
				return m_11 == 1 && m_12 == 0 && m_21 == 0 && m_22 == 1 && m_tx == 0 && m_ty == 0;
			}
			constexpr TValue GetDeterminant() const noexcept
			{
				return m_11 * m_22 - m_12 * m_21;
			}

			template<class T>
			requires(std::is_arithmetic_v<T>)
			constexpr void GetElements(T& m11, T& m12, T& m21, T& m22, T& tx, T& ty) const noexcept
			{
				m11 = static_cast<T>(m_11);
				m12 = static_cast<T>(m_12);
				m21 = static_cast<T>(m_21);
				m22 = static_cast<T>(m_22);
				tx = static_cast<T>(m_tx);
				ty = static_cast<T>(m_ty);
			}

			template<class T>
			requires(std::is_arithmetic_v<T>)
			constexpr void SetElements(T m11, T m12, T m21, T m22, T tx, T ty) noexcept
			{
				m_11 = static_cast<TValue>(m11);
				m_12 = static_cast<TValue>(m12);
				m_21 = static_cast<TValue>(m21);
				m_22 = static_cast<TValue>(m22);
				m_tx = static_cast<TValue>(tx);
				m_ty = static_cast<TValue>(ty);
			}

			template<class T>
			requires(std::is_arithmetic_v<T>)
			constexpr BasicAffineMatrix<T> Cast() const noexcept
			{
				return
				{
					static_cast<T>(m_11),
					static_cast<T>(m_12),
					static_cast<T>(m_21),
					static_cast<T>(m_22),
					static_cast<T>(m_tx),
					static_cast<T>(m_ty)
				};
			}

			constexpr bool IsInvertible() const noexcept
			{
				return GetDeterminant() != 0;
			}
			constexpr bool Invert() noexcept
			{
				// | m_11  m_12   0 |
				// | m_21  m_22   0 |
				// | m_tx  m_ty   1 |

				const TValue det = GetDeterminant();
				if (det != 0)
				{
					TValue ex = (m_21 * m_ty - m_22 * m_tx) / det;
					m_ty = (-m_11 * m_ty + m_12 * m_tx) / det;
					m_tx = ex;

					TValue e11 = m_22 / det;
					m_12 = -m_12 / det;
					m_21 = -m_21 / det;
					m_22 = m_11 / det;
					m_11 = e11;

					return true;
				}
				return false;
			}
			constexpr void Concat(const BasicAffineMatrix& other) noexcept
			{
				//                | m_11 m_12  0 |   | m_11  m_12   0 |
				// matrix' = other| m_21 m_22  0 | x | m_21  m_22   0 |
				//                | m_tx m_ty  1 |   | m_tx  m_ty   1 |

				TValue m11 = 0;
				TValue m12 = 0;
				TValue m21 = 0;
				TValue m22 = 0;
				TValue tx = 0;
				TValue ty = 0;
				other.GetElements(m11, m12, m21, m22, tx, ty);

				m_tx += tx * m_11 + ty * m_21;
				m_ty += tx * m_12 + ty * m_22;
				TValue e11 = m11 * m_11 + m12 * m_21;
				TValue e12 = m11 * m_12 + m12 * m_22;
				TValue e21 = m21 * m_11 + m22 * m_21;

				m_22 = m21 * m_12 + m22 * m_22;
				m_11 = e11;
				m_12 = e12;
				m_21 = e21;
			}

			constexpr void Scale(TValue xScale, TValue yScale) noexcept
			{
				// | xScale   0      0 |   | m_11  m_12   0 |
				// |   0    yScale   0 | x | m_21  m_22   0 |
				// |   0      0      1 |   | m_tx  m_ty   1 |

				m_11 *= xScale;
				m_12 *= xScale;
				m_21 *= yScale;
				m_22 *= yScale;
			}
			constexpr void Rotate(Angle angle) noexcept
			{
				// | cos    sin   0 |   | m_11  m_12   0 |
				// | -sin   cos   0 | x | m_21  m_22   0 |
				// |  0      0    1 |   | m_tx  m_ty   1 |

				auto s = std::sin(angle.ToRadians());
				auto c = std::cos(angle.ToRadians());

				TValue e11 = c * m_11 + s * m_21;
				TValue e12 = c * m_12 + s * m_22;
				m_21 = c * m_21 - s * m_11;
				m_22 = c * m_22 - s * m_12;
				m_11 = e11;
				m_12 = e12;
			}
			constexpr void Mirror(FlagSet<Orientation> direction) noexcept
			{
				TValue x = direction & Orientation::Horizontal ? -1 : 1;
				TValue y = direction & Orientation::Vertical ? -1 : 1;
				Scale(x, y);
			}
			constexpr void Translate(TValue dx, TValue dy) noexcept
			{
				// |  1   0   0 |   | m_11  m_12   0 |
				// |  0   1   0 | x | m_21  m_22   0 |
				// | dx  dy   1 |   | m_tx  m_ty   1 |

				m_tx += m_11 * dx + m_21 * dy;
				m_ty += m_12 * dx + m_22 * dy;
			}

			constexpr BasicPoint<TValue> TransformPoint(const BasicPoint<TValue>& point) const noexcept
			{
				//                           | m_11  m_12   0 |
				// | point.x  point.y  1 | x | m_21  m_22   0 |
				//                           | m_tx  m_ty   1 |

				if (IsIdentity())
				{
					return point;
				}
				else
				{
					return {point.GetX() * m_11 + point.GetY() * m_21 + m_tx, point.GetX() * m_12 + point.GetY() * m_22 + m_ty};
				}
			}
			constexpr BasicSize<TValue> TransformDistance(const BasicSize<TValue>& distance) const noexcept
			{
				//                                 | m_11  m_12   0 |
				// | distance.x  distance.y  0 | x | m_21  m_22   0 |
				//                                 | m_tx  m_ty   1 |

				if (IsIdentity())
				{
					return distance;
				}
				else
				{
					return {distance.GetX() * m_11 + distance.GetY() * m_21, distance.GetX() * m_12 + distance.GetY() * m_22};
				}
			}

		public:
			template<class T>
			requires(std::is_arithmetic_v<T>)
			constexpr bool operator==(const BasicAffineMatrix<T>& other) const noexcept
			{
				if (this == &other)
				{
					return true;
				}
				else
				{
					return m_11 == other.m_11 && m_12 == other.m_12 && m_21 == other.m_21 && m_22 == other.m_22 && m_tx == other.m_tx && m_ty == other.m_ty;
				}
			}

			template<class T>
			requires(std::is_arithmetic_v<T>)
			constexpr bool operator!=(const BasicAffineMatrix<T>& other) const noexcept
			{
				if (this != &other)
				{
					return true;
				}
				else
				{
					return m_11 != other.m_11 || m_12 != other.m_12 || m_21 != other.m_21 || m_22 != other.m_22 || m_tx != other.m_tx || m_ty != other.m_ty;
				}
			}
	};
}

namespace kxf
{
	using AffineMatrix = Geometry::BasicAffineMatrix<int>;
	using AffineMatrixF = Geometry::BasicAffineMatrix<float>;
	using AffineMatrixD = Geometry::BasicAffineMatrix<double>;
}

namespace kxf
{
	template<class T>
	struct BinarySerializer<Geometry::BasicAffineMatrix<T>> final
	{
		private:
			using TMatrix = Geometry::BasicAffineMatrix<T>;

		public:
			uint64_t Serialize(IOutputStream& stream, const TMatrix& value) const
			{
				return Serialization::WriteObject(stream, value.m_11) +
					Serialization::WriteObject(stream, value.m_12) +
					Serialization::WriteObject(stream, value.m_21) +
					Serialization::WriteObject(stream, value.m_22) +
					Serialization::WriteObject(stream, value.m_tx) +
					Serialization::WriteObject(stream, value.m_ty);
			}
			uint64_t Deserialize(IInputStream& stream, TMatrix& value) const
			{
				return Serialization::ReadObject(stream, value.m_11) +
					Serialization::ReadObject(stream, value.m_12) +
					Serialization::ReadObject(stream, value.m_21) +
					Serialization::ReadObject(stream, value.m_22) +
					Serialization::ReadObject(stream, value.m_tx) +
					Serialization::ReadObject(stream, value.m_ty);
			}
	};
}
