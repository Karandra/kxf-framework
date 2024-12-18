#pragma once
#include "Common.h"
#include "CriticalSection.h"

namespace kxf
{
	// Using 'CriticalSection' for now until better solution is found (to just change the implementation later).
	// Worth trying: https://www.puredevsoftware.com/blog/2018/08/23/adding-recursion-to-a-readwrite-lock/

	class KXF_API RecursiveRWLock final
	{
		private:
			CriticalSection m_Lock;

		public:
			RecursiveRWLock() noexcept = default;
			RecursiveRWLock(const RecursiveRWLock&) = delete;
			~RecursiveRWLock() noexcept = default;

		public:
			void LockRead() noexcept
			{
				m_Lock.Enter();
			}
			bool TryLockRead() noexcept
			{
				return m_Lock.TryEnter();
			}
			void UnlockRead() noexcept
			{
				m_Lock.Leave();
			}

			void LockWrite() noexcept
			{
				m_Lock.Enter();
			}
			bool TryLockWrite() noexcept
			{
				return m_Lock.TryEnter();
			}
			void UnlockWrite() noexcept
			{
				m_Lock.Leave();
			}

		public:
			RecursiveRWLock& operator=(const RecursiveRWLock&) = delete;
	};
}
