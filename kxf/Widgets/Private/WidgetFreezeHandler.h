#pragma once
#include "Common.h"

namespace kxf::Private
{
	class WidgetFreezeHandler final
	{
		private:
			size_t m_FreezeCount = 0;

		public:
			WidgetFreezeHandler() noexcept = default;

		public:
			bool IsFrozen() const noexcept
			{
				return m_FreezeCount != 0;
			}
			void Freeze() noexcept
			{
				m_FreezeCount++;
			}
			void Thaw() noexcept
			{
				if (m_FreezeCount != 0)
				{
					m_FreezeCount--;
				}
			}
	};
}
