#pragma once
#include "Common.h"
#include "kxf/Core/String.h"
#include "kxf/System/Common.h"
#include "kxf/IO/MemoryStream.h"

namespace kxf::IPC
{
	void* AllocateSharedMemoryRegion(void*& buffer, size_t size, FlagSet<MemoryProtection> protection, const String& name = {}, KernelObjectNamespace ns = KernelObjectNamespace::Local) noexcept;
	void* OpenSharedMemoryRegion(void*& buffer, const String& name, size_t size, FlagSet<MemoryProtection> protection, KernelObjectNamespace ns = KernelObjectNamespace::Local) noexcept;
	void FreeSharedMemoryRegion(void* handle, void* buffer) noexcept;
}

namespace kxf
{
	class KXF_API SharedMemoryBuffer final
	{
		private:
			void* m_Handle = nullptr;
			void* m_Buffer = nullptr;
			size_t m_Size = 0;
			FlagSet<MemoryProtection> m_Protection;

		private:
			void MakeNull() noexcept
			{
				m_Handle = nullptr;
				m_Buffer = nullptr;
				m_Size = 0;
				m_Protection = MemoryProtection::None;
			}

			MemoryInputStream DoGetInputStream(size_t size, bool unchecked = false) const noexcept;
			MemoryOutputStream DoGetOutputStream(size_t size, bool unchecked = false) noexcept;

		public:
			SharedMemoryBuffer() noexcept = default;
			SharedMemoryBuffer(SharedMemoryBuffer&& other) noexcept
			{
				*this = std::move(other);
			}
			SharedMemoryBuffer(const SharedMemoryBuffer&) = delete;
			~SharedMemoryBuffer() noexcept
			{
				Free();
			}

		public:
			bool IsNull() const noexcept
			{
				return m_Handle == nullptr || m_Buffer == nullptr || m_Size == 0;
			}

			void Free() noexcept
			{
				IPC::FreeSharedMemoryRegion(m_Handle, m_Buffer);
				MakeNull();
			}
			bool Open(const String& name, size_t size, FlagSet<MemoryProtection> protection, KernelObjectNamespace ns = KernelObjectNamespace::Local) noexcept
			{
				Free();
				if (m_Handle = IPC::OpenSharedMemoryRegion(m_Buffer, name, size, protection, ns))
				{
					m_Size = size;
					m_Protection = protection;
					return true;
				}
				else
				{
					MakeNull();
					return false;
				}
			}
			bool Allocate(size_t size, FlagSet<MemoryProtection> protection, const String& name = {}, KernelObjectNamespace ns = KernelObjectNamespace::Local) noexcept
			{
				Free();
				if (m_Handle = IPC::AllocateSharedMemoryRegion(m_Buffer, size, protection, name, ns))
				{
					m_Size = size;
					m_Protection = protection;
					return true;
				}
				else
				{
					MakeNull();
					return false;
				}
			}
			void ZeroBuffer() noexcept;

			void Attach(void* handle, void* data, size_t size, FlagSet<MemoryProtection> protection) noexcept
			{
				Free();
				
				m_Handle = handle;
				m_Buffer = data;
				m_Size = size;
				m_Protection = protection;
			}
			void* Detach() noexcept
			{
				void* handle = m_Handle;
				MakeNull();
				return handle;
			}

			FlagSet<MemoryProtection> GetProtection() const noexcept
			{
				return m_Protection;
			}
			bool IsReadable() const noexcept
			{
				return m_Protection & MemoryProtection::Read;
			}
			bool IsWriteable() const noexcept
			{
				return m_Protection & MemoryProtection::Write;
			}
			bool IsExecutable() const noexcept
			{
				return m_Protection & MemoryProtection::Execute;
			}

			size_t GetSize() const noexcept
			{
				return m_Size;
			}
			const void* GetBuffer() const noexcept
			{
				return m_Buffer;
			}
			void* GetBuffer() noexcept
			{
				return m_Buffer;
			}

		public:
			MemoryInputStream GetInputStream(size_t maxSize = std::numeric_limits<size_t>::max()) const noexcept
			{
				return DoGetInputStream(std::min(m_Size, maxSize));
			}
			MemoryInputStream GetInputStreamUnchecked(size_t size) const noexcept
			{
				return DoGetInputStream(size, true);
			}

			MemoryOutputStream GetOutputStream(size_t maxSize = std::numeric_limits<size_t>::max()) noexcept
			{
				return DoGetOutputStream(std::min(m_Size, maxSize));
			}
			MemoryOutputStream GetOutputStreamUnchecked(size_t size) noexcept
			{
				return DoGetOutputStream(size, true);
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			SharedMemoryBuffer& operator=(SharedMemoryBuffer&& other) noexcept
			{
				if (this != &other)
				{
					Attach(other.m_Handle, other.m_Buffer, other.m_Size, other.m_Protection);
					other.Detach();
				}
				return *this;
			}
			SharedMemoryBuffer& operator=(const SharedMemoryBuffer&) = delete;
	};
}
