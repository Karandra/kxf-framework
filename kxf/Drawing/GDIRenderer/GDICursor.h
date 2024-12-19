#pragma once
#include "Common.h"
#include "IGDIObject.h"
#include "../IImage2D.h"
#include <wx/cursor.h>

namespace kxf
{
	class GDIIcon;
	class GDIBitmap;
}
namespace kxf::Drawing
{
	enum class StockCursor
	{
		None = wxCURSOR_NONE,
		Default = wxCURSOR_DEFAULT,

		ArrowLeft = wxCURSOR_ARROW,
		ArrowRight = wxCURSOR_RIGHT_ARROW,
		ArrowWait = wxCURSOR_ARROWWAIT,
		ArrowQuestion = wxCURSOR_QUESTION_ARROW,

		PointLeft = wxCURSOR_POINT_LEFT,
		PointRight = wxCURSOR_POINT_RIGHT,

		LeftButton = wxCURSOR_LEFT_BUTTON,
		MiddleButton = wxCURSOR_MIDDLE_BUTTON,
		MiddleRight = wxCURSOR_RIGHT_BUTTON,

		SizeAny = wxCURSOR_SIZING,
		SizeRightTopLeftBottom = wxCURSOR_SIZENESW,
		SizeLeftTopRightBottom = wxCURSOR_SIZENWSE,
		SizeTopBottom = wxCURSOR_SIZENS,
		SizeLeftRight = wxCURSOR_SIZEWE,

		Char = wxCURSOR_CHAR,
		Cross = wxCURSOR_CROSS,
		Hand = wxCURSOR_HAND,
		NoEntry = wxCURSOR_NO_ENTRY,
		Magnifier = wxCURSOR_MAGNIFIER,
		Wait = wxCURSOR_WAIT,
		Watch = wxCURSOR_WATCH,
		IBeam = wxCURSOR_IBEAM,
		Bullseye = wxCURSOR_BULLSEYE,
		PaintPrush = wxCURSOR_PAINT_BRUSH,
		SprayCan = wxCURSOR_SPRAYCAN,
		Pencil = wxCURSOR_PENCIL,
		Blank = wxCURSOR_BLANK,
	};
}

namespace kxf
{
	class KXF_API GDICursor: public RTTI::DynamicImplementation<GDICursor, IGDIObject, IImage2D>
	{
		kxf_RTTI_DeclareIID(GDICursor, {0xec12b28a, 0x111e, 0x4f00, {0x8c, 0xe0, 0xdd, 0xb, 0x18, 0x9, 0xf7, 0x5e}});

		private:
			wxCursor m_Cursor;
			Point m_HotSpot = Point::UnspecifiedPosition();

		private:
			std::optional<String> GetOption(const String& name) const override
			{
				return {};
			}
			void SetOption(const String& name, const String& value) override
			{
			}

		public:
			GDICursor() = default;
			GDICursor(const wxCursor& other)
				:m_Cursor(other)
			{
			}

			GDICursor(const GDIIcon& other);
			GDICursor(const GDIBitmap& other);
			GDICursor(const BitmapImage& other);
			GDICursor(const GDICursor& other)
				:m_Cursor(other.m_Cursor), m_HotSpot(other.m_HotSpot)
			{
			}

			virtual ~GDICursor() = default;

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_Cursor.IsOk();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				return m_Cursor.GetHandle() == other.GetHandle();
			}
			std::shared_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_shared<GDICursor>(m_Cursor);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// IImage2D
			bool IsSameAs(const IImage2D& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto cursor = other.QueryInterface<GDICursor>())
				{
					return m_Cursor.IsSameAs(cursor->m_Cursor);
				}
				return false;
			}
			std::shared_ptr<IImage2D> CloneImage2D() const override
			{
				return std::make_shared<GDICursor>(m_Cursor);
			}

			Size GetSize() const override
			{
				return m_Cursor.IsOk() ? Size(m_Cursor.GetSize()) : Size::UnspecifiedSize();
			}
			ColorDepth GetColorDepth() const override
			{
				return m_Cursor.GetDepth();
			}
			UniversallyUniqueID GetFormat() const override
			{
				return ImageFormat::CUR;
			}

			void Create(const Size& size) override;
			bool Load(IInputStream& stream, const UniversallyUniqueID& format = ImageFormat::Any, size_t index = npos);
			bool Save(IOutputStream& stream, const UniversallyUniqueID& format) const;

			std::optional<int> GetOptionInt(const String& name) const override;
			void SetOption(const String& name, int value) override;

			BitmapImage ToBitmapImage(const Size& size = Size::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const override;

			// GDICursor
			const wxCursor& ToWxCursor() const noexcept
			{
				return m_Cursor;
			}
			wxCursor& ToWxCursor() noexcept
			{
				return m_Cursor;
			}
			GDIIcon ToGDIIcon() const;
			GDIBitmap ToGDIBitmap(const Size& size = Size::UnspecifiedSize(), InterpolationQuality interpolationQuality = InterpolationQuality::Default) const;

			Point GetHotSpot() const
			{
				return Point(m_Cursor.GetHotSpot());
			}
			void SetHotSpot(Point hotSpot)
			{
				// TODO: Update the hotspot on the cursor in memory
				m_HotSpot = std::move(hotSpot);
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

			GDICursor& operator=(const GDICursor& other)
			{
				m_Cursor = other.m_Cursor;
				m_HotSpot = other.m_HotSpot;

				return *this;
			}
	};
}

namespace kxf::Drawing
{
	inline constexpr wxStockCursor ToWxStockCursor(StockCursor cursorType) noexcept
	{
		return static_cast<wxStockCursor>(cursorType);
	}
	inline constexpr StockCursor FromWxStockCursor(wxStockCursor cursorType) noexcept
	{
		return static_cast<StockCursor>(cursorType);
	}

	KXF_API GDICursor GetStockCursor(Drawing::StockCursor cursor);
}
