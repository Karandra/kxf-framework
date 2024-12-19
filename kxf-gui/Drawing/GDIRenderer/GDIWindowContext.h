#pragma once
#include "GDIContext.h"
#include <wx/dcclient.h>

namespace kxf::Drawing
{
	class KXF_API GDIWindowContextBase: public GDIContext
	{
		protected:
			const wxWindowDC& GetWindowDC() const noexcept
			{
				return static_cast<wxWindowDC&>(*m_DC);
			}
			wxWindowDC& GetWindowDC() noexcept
			{
				return static_cast<wxWindowDC&>(*m_DC);
			}

		public:
			GDIWindowContextBase() = default;
			GDIWindowContextBase(wxWindowDC& other)
				:GDIContext(other)
			{
			}
			GDIWindowContextBase(const GDIWindowContextBase&) = delete;

		public:
			const wxWindowDC& AsWXDC() const noexcept
			{
				return GetWindowDC();
			}
			wxWindowDC& AsWXDC() noexcept
			{
				return GetWindowDC();
			}

		public:
			GDIContext& operator=(const GDIContext&) = delete;
	};
}

namespace kxf
{
	class KXF_API GDIWindowContext final: public Drawing::GDIWindowContextBase
	{
		private:
			wxWindowDC m_DC;

		public:
			GDIWindowContext(wxWindow& window)
				:GDIWindowContextBase(m_DC), m_DC(&window)
			{
			}

	};

	class KXF_API GDIClientContext final: public Drawing::GDIWindowContextBase
	{
		private:
			wxClientDC m_DC;

		public:
			GDIClientContext(wxWindow& window)
				:GDIWindowContextBase(m_DC), m_DC(&window)
			{
			}

		public:
			const wxClientDC& AsWXDC() const noexcept
			{
				return m_DC;
			}
			wxClientDC& AsWXDC() noexcept
			{
				return m_DC;
			}
	};

	class KXF_API GDIPaintContext final: public Drawing::GDIWindowContextBase
	{
		private:
			wxPaintDC m_DC;

		public:
			GDIPaintContext(wxWindow& window)
				:GDIWindowContextBase(m_DC), m_DC(&window)
			{
			}

		public:
			const wxPaintDC& AsWXDC() const noexcept
			{
				return m_DC;
			}
			wxPaintDC& AsWXDC() noexcept
			{
				return m_DC;
			}
	};
}