#pragma once
#include "Common.h"
#include "LockGuard.h"
#include "kxf/Core/AlignedStorage.h"
struct _RTL_CRITICAL_SECTION;

namespace kxf
{
	class KXF_API CriticalSection final
	{
		private:
			struct CritSec final
			{
				void* DebugInfo;
				uint32_t LockCount;
				uint32_t RecursionCount;
				void* OwningThread;
				void* LockSemaphore;
				size_t SpinCount;
			};
			AlignedStorage<_RTL_CRITICAL_SECTION, sizeof(CritSec), alignof(CritSec)> m_CritSec;

		public:
			CriticalSection() noexcept;
			CriticalSection(uint32_t spinCount) noexcept;
			CriticalSection(const CriticalSection&) = delete;
			~CriticalSection() noexcept;

		public:
			void Enter() noexcept;
			bool TryEnter() noexcept;
			void Leave() noexcept;

			_RTL_CRITICAL_SECTION& GetHandle() noexcept
			{
				return *m_CritSec;
			}
			uint32_t SetSpinCount(uint32_t spinCount) noexcept;

		public:
			CriticalSection& operator=(const CriticalSection&) = delete;
	};
}

namespace kxf
{
	template<>
	class LockGuard<CriticalSection> final
	{
		private:
			CriticalSection* m_Lock = nullptr;

		public:
			explicit LockGuard(CriticalSection& lock) noexcept
				:m_Lock(&lock)
			{
				m_Lock->Enter();
			}
			LockGuard(LockGuard&& other) noexcept
			{
				*this = std::move(other);
			}
			LockGuard(const LockGuard&) = delete;
			~LockGuard() noexcept
			{
				Unlock();
			}

		public:
			void Unlock()
			{
				if (CriticalSection* lock = m_Lock)
				{
					m_Lock = nullptr;
					lock->Leave();
				}
			}
			CriticalSection* get() const noexcept
			{
				return m_Lock;
			}

		public:
			LockGuard& operator=(const LockGuard&) = delete;
			LockGuard& operator=(LockGuard&& other) noexcept
			{
				m_Lock = other.m_Lock;
				other.m_Lock = nullptr;

				return *this;
			}
	};
}
