#pragma once
#include "Common.h"
#include "kxf/System/Common.h"
#include "kxf/Core/String.h"
#include "kxf/DateTime/TimeSpan.h"

namespace kxf
{
	class KXF_API Mutex final
	{
		private:
			void* m_Handle = nullptr;

		private:
			bool DoCreate(const String& name, bool acquireInitially, KernelObjectNamespace ns) noexcept;
			bool DoOpen(const String& name, KernelObjectNamespace ns) noexcept;

		public:
			Mutex() noexcept = default;
			Mutex(const Mutex&) = delete;
			Mutex(Mutex&& other) noexcept
				:m_Handle(other.m_Handle)
			{
				other.m_Handle = nullptr;
			}
			~Mutex() noexcept
			{
				Destroy();
			}

		public:
			bool IsNull() const noexcept
			{
				return m_Handle == nullptr;
			}
			void* GetHandle() const noexcept
			{
				return m_Handle;
			}

			bool Create(const String& name, KernelObjectNamespace ns = KernelObjectNamespace::Local) noexcept
			{
				return DoCreate(name, false, ns);
			}
			bool CreateAcquired(const String& name, KernelObjectNamespace ns = KernelObjectNamespace::Local) noexcept
			{
				return DoCreate(name, true, ns);
			}
			bool Open(const String& name, KernelObjectNamespace ns = KernelObjectNamespace::Local) noexcept
			{
				return DoOpen(name, ns);
			}

			bool Acquire(const TimeSpan& timeout = {}) noexcept;
			bool Release() noexcept;
			void Destroy() noexcept;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			Mutex& operator=(const Mutex&) = delete;
			Mutex& operator=(Mutex&& other) noexcept
			{
				Destroy();

				m_Handle = other.m_Handle;
				other.m_Handle = nullptr;

				return *this;
			}
	};
}
