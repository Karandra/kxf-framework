#pragma once
#include "Common.h"
#include "CellAttributeOptions.h"

namespace kxf::DataView
{
	class KXF_API CellAttributes final
	{
		private:
			CellOptions m_Options;
			CellBGOptions m_BGOptions;
			CellFontOptions m_FontOptions;

		public:
			bool IsDefault() const noexcept
			{
				return m_Options.IsDefault() && m_BGOptions.IsDefault() && m_FontOptions.IsDefault();
			}
			Font GetEffectiveFont(const Font& baseFont) const;

		public:
			const CellOptions& Options() const noexcept
			{
				return m_Options;
			}
			CellOptions& Options() noexcept
			{
				return m_Options;
			}

			const CellBGOptions& BGOptions() const noexcept
			{
				return m_BGOptions;
			}
			CellBGOptions& BGOptions() noexcept
			{
				return m_BGOptions;
			}

			const CellFontOptions& FontOptions() const noexcept
			{
				return m_FontOptions;
			}
			CellFontOptions& FontOptions() noexcept
			{
				return m_FontOptions;
			}
	};
}
