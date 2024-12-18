#pragma once
#include "Common.h"
#include "CellAttributeOptions.h"

namespace kxf::UI::DataView
{
	class KXF_API CellAttribute final
	{
		private:
			CellAttributes::Options m_Options;
			CellAttributes::CellBGOptions m_BGOptions;
			CellAttributes::FontOptions m_FontOptions;

		public:
			bool IsDefault() const
			{
				return m_Options.IsDefault() && m_BGOptions.IsDefault() && m_FontOptions.IsDefault();
			}
			Font GetEffectiveFont(const Font& baseFont) const;

		public:
			const CellAttributes::Options& Options() const
			{
				return m_Options;
			}
			CellAttributes::Options& Options()
			{
				return m_Options;
			}

			const CellAttributes::CellBGOptions& BGOptions() const
			{
				return m_BGOptions;
			}
			CellAttributes::CellBGOptions& BGOptions()
			{
				return m_BGOptions;
			}

			const CellAttributes::FontOptions& FontOptions() const
			{
				return m_FontOptions;
			}
			CellAttributes::FontOptions& FontOptions()
			{
				return m_FontOptions;
			}
	};
}
