#pragma once
#include "GDIContext.h"
#include <wx/dcscreen.h>

namespace kxf
{
	class KXF_API GDIScreenContext final: public GDIContext
	{
		private:
			wxScreenDC m_DC;

		public:
			GDIScreenContext()
				:GDIContext(m_DC)
			{
			}

		public:
			const wxScreenDC& AsWXDC() const noexcept
			{
				return m_DC;
			}
			wxScreenDC& AsWXDC() noexcept
			{
				return m_DC;
			}
	};
}
