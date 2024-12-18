#pragma once
#include "Common.h"
#include "kxf/Core/AlignedStorage.h"
struct _RTL_SRWLOCK;

namespace kxf
{
	class NullReadWriteLock final
	{
		public:
			NullReadWriteLock() noexcept = default;
			NullReadWriteLock(const NullReadWriteLock&) = delete;
			~NullReadWriteLock() noexcept = default;

		public:
			void LockRead() noexcept { }
			void UnlockRead() noexcept { }

			void LockWrite() noexcept { }
			void UnlockWrite() noexcept { }

		public:
			NullReadWriteLock& operator=(const NullReadWriteLock&) = delete;
	};
}

namespace kxf
{
	class KXF_API ReadWriteLock final
	{
		private:
			AlignedStorage<_RTL_SRWLOCK, sizeof(void*), alignof(void*)> m_Lock;

		public:
			ReadWriteLock() noexcept;
			ReadWriteLock(const ReadWriteLock&) = delete;
			~ReadWriteLock() noexcept;

		public:
			void LockRead() noexcept;
			bool TryLockRead() noexcept;
			void UnlockRead() noexcept;

			void LockWrite() noexcept;
			bool TryLockWrite() noexcept;
			void UnlockWrite() noexcept;

			_RTL_SRWLOCK& GetHandle() noexcept
			{
				return *m_Lock;
			}

		public:
			ReadWriteLock& operator=(const ReadWriteLock&) = delete;
	};
}
