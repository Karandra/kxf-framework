#pragma once
#include "Common.h"

namespace kxf::Utility
{
	class RecursionGuardFlag final
	{
		private:
			std::atomic<size_t> m_Level = 0;

		public:
			RecursionGuardFlag() noexcept = default;
			RecursionGuardFlag(const RecursionGuardFlag&) = delete;

		public:
			size_t GetLevel() const noexcept
			{
				return m_Level;
			}

			void OnEnter() noexcept
			{
				++m_Level;
			}
			void OnLeave() noexcept
			{
				--m_Level;
			}

		public:
			RecursionGuardFlag& operator=(const RecursionGuardFlag&) const = delete;
	};
}

namespace kxf::Utility
{
	class RecursionGuard final
	{
		private:
			RecursionGuardFlag& m_Flag;
			bool m_IsInside = false;

		public:
			RecursionGuard(RecursionGuardFlag& flag) noexcept
				:m_Flag(flag), m_IsInside(flag.GetLevel() != 0)
			{
				flag.OnEnter();
			}
			~RecursionGuard() noexcept
			{
				m_Flag.OnLeave();
			}

		public:
			bool IsInside() const noexcept
			{
				return m_IsInside;
			}
	};
}
