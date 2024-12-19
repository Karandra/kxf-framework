#pragma once
#include "GDIContext.h"
#include "../GDIRenderer/GDIGraphicsContext.h"
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>

namespace kxf::Drawing
{
	class KXF_API GDIMemoryContextBase: public GDIContext
	{
		protected:
			GDIBitmap* m_SelectedBitmap = nullptr;

		protected:
			const wxMemoryDC& GetMemoryDC() const noexcept
			{
				return static_cast<wxMemoryDC&>(*m_DC);
			}
			wxMemoryDC& GetMemoryDC() noexcept
			{
				return static_cast<wxMemoryDC&>(*m_DC);
			}

		public:
			GDIMemoryContextBase() = default;
			GDIMemoryContextBase(wxMemoryDC& other)
				:GDIContext(other)
			{
			}
			GDIMemoryContextBase(const GDIMemoryContextBase&) = delete;

		public:
			const wxMemoryDC& AsWXDC() const noexcept
			{
				return GetMemoryDC();
			}
			wxMemoryDC& AsWXDC() noexcept
			{
				return GetMemoryDC();
			}

			GDIBitmap GetSelectedBitmap() const
			{
				return GetMemoryDC().GetSelectedBitmap();
			}
			void SelectObject(GDIBitmap& bitmap)
			{
				m_SelectedBitmap = bitmap ? &bitmap : nullptr;
				GetMemoryDC().SelectObject(bitmap.AsWXBitmap());
			}
			void SelectObjectAsSource(const GDIBitmap& bitmap)
			{
				GetMemoryDC().SelectObjectAsSource(bitmap.AsWXBitmap());
			}
			void UnselectObject()
			{
				GetMemoryDC().SelectObject(wxNullBitmap);
				if (m_SelectedBitmap)
				{
					m_SelectedBitmap->UpdateAlpha();
					m_SelectedBitmap = nullptr;
				}
			}

		public:
			GDIContext& operator=(const GDIContext&) = delete;
	};

	class KXF_API GDIBufferedContextBase: public GDIMemoryContextBase
	{
		protected:
			const wxBufferedDC& GetBufferedDC() const noexcept
			{
				return static_cast<wxBufferedDC&>(*m_DC);
			}
			wxBufferedDC& GetBufferedDC() noexcept
			{
				return static_cast<wxBufferedDC&>(*m_DC);
			}

		public:
			GDIBufferedContextBase() = default;
			GDIBufferedContextBase(wxBufferedDC& other)
				:GDIMemoryContextBase(other)
			{
			}
			GDIBufferedContextBase(const GDIMemoryContextBase&) = delete;

		public:
			const wxBufferedDC& AsWXDC() const noexcept
			{
				return GetBufferedDC();
			}
			wxBufferedDC& AsWXDC() noexcept
			{
				return GetBufferedDC();
			}

			FlagSet<GDIBufferedContextFlag> GetFlags() const
			{
				return static_cast<GDIBufferedContextFlag>(GetBufferedDC().GetStyle());
			}
			void SetFlags(FlagSet<GDIBufferedContextFlag> flags)
			{
				GetBufferedDC().SetStyle(flags.ToInt<int>());
			}

			void UnMask()
			{
				GetBufferedDC().UnMask();
			}

		public:
			GDIContext& operator=(const GDIContext&) = delete;
	};
}

namespace kxf
{
	class KXF_API GDIMemoryContext final: public Drawing::GDIMemoryContextBase
	{
		private:
			wxMemoryDC m_DC;

		public:
			GDIMemoryContext()
				:GDIMemoryContextBase(m_DC)
			{
			}
			GDIMemoryContext(const GDIContext& other)
				:GDIMemoryContextBase(m_DC), m_DC(const_cast<wxDC*>(&other.AsWXDC()))
			{
			}
			GDIMemoryContext(GDIBitmap& bitmap)
				:GDIMemoryContextBase(m_DC), m_DC(bitmap.AsWXBitmap())
			{
				m_SelectedBitmap = &bitmap;
			}
			~GDIMemoryContext()
			{
				UnselectObject();
			}
	};

	class KXF_API GDIBufferedContext final: public Drawing::GDIBufferedContextBase
	{
		private:
			wxBufferedDC m_DC;

		public:
			GDIBufferedContext(GDIContext& other, const Size& size, FlagSet<GDIBufferedContextFlag> flags)
				:GDIBufferedContextBase(m_DC), m_DC(&other.AsWXDC(), size, flags.ToInt<int>())
			{
			}
			GDIBufferedContext(GDIContext& other, GDIBitmap& bitmap, FlagSet<GDIBufferedContextFlag> flags)
				:GDIBufferedContextBase(m_DC), m_DC(&other.AsWXDC(), bitmap.AsWXBitmap(), flags.ToInt<int>())
			{
			}
	};

	class KXF_API GDIBufferedPaintContext final: public Drawing::GDIBufferedContextBase
	{
		private:
			wxBufferedPaintDC m_DC;

		public:
			GDIBufferedPaintContext(wxWindow& window, FlagSet<GDIBufferedContextFlag> flags)
				:GDIBufferedContextBase(m_DC), m_DC(&window, flags.ToInt<int>())
			{
			}
			GDIBufferedPaintContext(wxWindow& window, GDIBitmap& bitmap, FlagSet<GDIBufferedContextFlag> flags)
				:GDIBufferedContextBase(m_DC), m_DC(&window, bitmap.AsWXBitmap(), flags.ToInt<int>())
			{
			}

		public:
			const wxBufferedPaintDC& AsWXDC() const noexcept
			{
				return m_DC;
			}
			wxBufferedPaintDC& AsWXDC() noexcept
			{
				return m_DC;
			}
	};

	class KXF_API GDIAutoBufferedPaintContext final: public Drawing::GDIBufferedContextBase
	{
		private:
			wxAutoBufferedPaintDC m_DC;

		public:
			GDIAutoBufferedPaintContext(wxWindow& window)
				:GDIBufferedContextBase(m_DC), m_DC(&window)
			{
			}

		public:
			const wxAutoBufferedPaintDC& AsWXDC() const noexcept
			{
				return m_DC;
			}
			wxAutoBufferedPaintDC& AsWXDC() noexcept
			{
				return m_DC;
			}
	};
}
