#pragma once
#include "Common.h"
#include "IGDIObject.h"
#include "Private/GDI.h"
#include "../Private/Common.h"
#include <wx/region.h>

namespace kxf
{
	class GDIBitmap;
}

namespace kxf
{
	class KXF_API GDIRegion: public RTTI::ExtendInterface<GDIRegion, IGDIObject>
	{
		kxf_RTTI_DeclareIID(GDIRegion, {0xd8f0f933, 0x9126, 0x4ef9, {0xa0, 0xf, 0x69, 0xe4, 0xd9, 0x9c, 0x7a, 0x97}});

		private:
			wxRegion m_Region;

		public:
			GDIRegion() = default;
			GDIRegion(const wxRegion& other)
				:m_Region(other)
			{
			}

			GDIRegion(const Rect& rect)
				:m_Region(rect)
			{
			}
			GDIRegion(const Point& pos, const Size& size)
				:m_Region(Rect(pos, size))
			{
			}
			GDIRegion(const Point& topLeft, const Point& bottomRight)
				:m_Region(topLeft, bottomRight)
			{
			}
			GDIRegion(const GDIBitmap& bitmap);
			GDIRegion(const GDIBitmap& bitmap, const Color& transparentColor, int tolerance = 0);

			GDIRegion(const Point* points, size_t count, PolygonFill fillMode = PolygonFill::OddEvenRule)
			{
				if (auto modeWx = Drawing::Private::MapPolygonFill(fillMode))
				{
					auto pointsBuffer = Private::ConvertWxPoints(points, count);
					m_Region = wxRegion(pointsBuffer.size(), pointsBuffer.data(), *modeWx);
				}
			}

			template<size_t N>
			GDIRegion(const Point(&points)[N], PolygonFill fillMode = PolygonFill::OddEvenRule)
			{
				if (auto modeWx = Drawing::Private::MapPolygonFill(fillMode))
				{
					auto pointsBuffer = Private::ConvertWxPoints(points);
					m_Region = wxRegion(pointsBuffer.size(), pointsBuffer.data(), *modeWx);
				}
			}

			template<size_t N>
			GDIRegion(const std::array<Point, N>& points, PolygonFill fillMode = PolygonFill::OddEvenRule)
			{
				if (auto modeWx = Drawing::Private::MapPolygonFill(fillMode))
				{
					auto pointsBuffer = Private::ConvertWxPoints(points);
					m_Region = wxRegion(pointsBuffer.size(), pointsBuffer.data(), *modeWx);
				}
			}

			virtual ~GDIRegion() = default;

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_Region.IsOk() || m_Region.IsEmpty();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				if (this == &other || m_Region.GetHRGN() == other.GetHandle())
				{
					return true;
				}
				else if (auto object = other.QueryInterface<GDIRegion>())
				{
					return m_Region == object->m_Region;
				}
				return false;
			}
			std::unique_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_unique<GDIRegion>(m_Region);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// GDIRegion
			const wxRegion& ToWxRegion() const noexcept
			{
				return m_Region;
			}
			wxRegion& ToWxRegion() noexcept
			{
				return m_Region;
			}

			void Clear()
			{
				m_Region.Clear();
			}
			Rect GetBox() const
			{
				return Rect(m_Region.GetBox());
			}
			bool Offset(const Point& pos)
			{
				return m_Region.Offset(pos);
			}
			GDIBitmap ToBitmap() const;

			bool Contains(const Point& point) const
			{
				return m_Region.Contains(point);
			}
			bool Contains(const Rect& rect) const
			{
				return m_Region.Contains(rect);
			}

			bool Intersect(const GDIRegion& other)
			{
				return m_Region.Intersect(other.m_Region);
			}
			bool Intersect(const Rect& rect)
			{
				return m_Region.Intersect(rect);
			}

			bool Subtract(const GDIRegion& other)
			{
				return m_Region.Subtract(other.m_Region);
			}
			bool Subtract(const Rect& rect)
			{
				return m_Region.Subtract(rect);
			}

			bool Union(const GDIRegion& other)
			{
				return m_Region.Union(other.m_Region);
			}
			bool Union(const Rect& rect)
			{
				return m_Region.Union(rect);
			}
			bool Union(const GDIBitmap& bitmap);
			bool Union(const GDIBitmap& bitmap, const Color& transparentColor, int tolerance = 0);

			bool Xor(const GDIRegion& other)
			{
				return m_Region.Xor(other.m_Region);
			}
			bool Xor(const Rect& rect)
			{
				return m_Region.Xor(rect);
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			GDIRegion& operator=(const GDIRegion& other)
			{
				m_Region = other.m_Region;

				return *this;
			}

			bool operator==(const GDIRegion& other) const
			{
				return m_Region == other.m_Region;
			}
			bool operator!=(const GDIRegion& other) const
			{
				return m_Region != other.m_Region;
			}
	};
}
