#pragma once
#include "Common.h"

namespace kxf::DataView
{
	class KXF_API TextRenderer: public IDataViewCellRenderer
	{
		private:
			String m_Value;

		protected:
			// IDataViewCellRenderer
			void DrawContent(const RenderInfo& renderInfo) override;
			Size GetCellSize(const RenderInfo& renderInfo) const override;

		public:
			TextRenderer() = default;

		public:
			// IDataViewCellRenderer
			String GetDisplayText(const Any& value) const override;
			bool SetDisplayValue(Any value) override;
	};
}
