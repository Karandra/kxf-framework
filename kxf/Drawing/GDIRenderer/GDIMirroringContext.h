#pragma once
#include "GDIContext.h"
#include <wx/dcmirror.h>

namespace kxf
{
	class KXF_API GDIMirroringContext final: public GDIContext
	{
		private:
			wxMirrorDC m_DC;

		public:
			GDIMirroringContext(GDIContext& other, bool enableMirroring)
				:GDIContext(m_DC), m_DC(other.ToWxDC(), enableMirroring)
			{
			}

		public:
			const wxMirrorDC& ToWxDC() const noexcept
			{
				return m_DC;
			}
			wxMirrorDC& ToWxDC() noexcept
			{
				return m_DC;
			}
	};
}
