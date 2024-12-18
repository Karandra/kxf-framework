#include "kxf-pch.h"
#include "ReadWriteLock.h"
#include <Windows.h>

namespace kxf
{
	ReadWriteLock::ReadWriteLock() noexcept
	{
		m_Lock.Construct();
		::InitializeSRWLock(&m_Lock);
	}
	ReadWriteLock::~ReadWriteLock() noexcept
	{
		m_Lock.Destroy();
	}

	void ReadWriteLock::LockRead() noexcept
	{
		::AcquireSRWLockShared(&m_Lock);
	}
	bool ReadWriteLock::TryLockRead() noexcept
	{
		return ::TryAcquireSRWLockShared(&m_Lock);
	}
	void ReadWriteLock::UnlockRead() noexcept
	{
		::ReleaseSRWLockShared(&m_Lock);
	}

	void ReadWriteLock::LockWrite() noexcept
	{
		::AcquireSRWLockExclusive(&m_Lock);
	}
	bool ReadWriteLock::TryLockWrite() noexcept
	{
		return ::TryAcquireSRWLockExclusive(&m_Lock);
	}
	void ReadWriteLock::UnlockWrite() noexcept
	{
		::ReleaseSRWLockExclusive(&m_Lock);
	}
}
