#include "kxf-pch.h"
#include "SharedMemory.h"
#include <Windows.h>
#include "kxf/System/Private/System.h"
#include "kxf/Utility/Memory.h"

namespace
{
	using namespace kxf;

	constexpr FlagSet<uint32_t> ConvertProtection(FlagSet<MemoryProtection> protection) noexcept
	{
		if (protection & MemoryProtection::Execute)
		{
			if (protection & MemoryProtection::RW)
			{
				return PAGE_EXECUTE_READWRITE;
			}
			if (protection & MemoryProtection::Read)
			{
				return PAGE_EXECUTE_READ;
			}
		}
		else
		{
			if (protection & MemoryProtection::RW)
			{
				return PAGE_READWRITE;
			}
			if (protection & MemoryProtection::Read)
			{
				return PAGE_READONLY;
			}
		}
		return {};
	}
	constexpr FlagSet<uint32_t> ConvertMemoryAccess(FlagSet<MemoryProtection> protection) noexcept
	{
		FlagSet<uint32_t> value;
		if (protection & MemoryProtection::RW)
		{
			value = FILE_MAP_ALL_ACCESS;
		}
		else if (protection & MemoryProtection::Write)
		{
			value = FILE_MAP_WRITE;
		}
		else if (protection & MemoryProtection::Read)
		{
			value = FILE_MAP_READ;
		}

		value.Add(FILE_MAP_EXECUTE, protection & MemoryProtection::Execute);
		value.Add(FILE_MAP_COPY, protection & MemoryProtection::CopyOnWrite);
		return value;
	}
}

namespace kxf::IPC
{
	void* AllocateSharedMemoryRegion(void*& buffer, size_t size, FlagSet<MemoryProtection> protection, const String& name, KernelObjectNamespace ns) noexcept
	{
		ULARGE_INTEGER sizeULI = {};
		sizeULI.QuadPart = size;

		const auto protectionWin32 = ConvertProtection(protection);
		const auto memoryAccessWin32 = ConvertMemoryAccess(protection);

		String fullName = System::Private::GetKernelObjectName(name, ns);
		void* handle = ::CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, *protectionWin32, sizeULI.HighPart, sizeULI.LowPart, !fullName.IsEmpty() ? fullName.wc_str() : nullptr);
		if (handle && handle != INVALID_HANDLE_VALUE)
		{
			buffer = ::MapViewOfFile(handle, *memoryAccessWin32, 0, 0, size);
			if (buffer)
			{
				return handle;
			}
			::CloseHandle(handle);
		}
		return nullptr;
	}
	void* OpenSharedMemoryRegion(void*& buffer, const String& name, size_t size, FlagSet<MemoryProtection> protection, KernelObjectNamespace ns) noexcept
	{
		const auto memoryAccessWin32 = ConvertMemoryAccess(protection);
		String fullName = System::Private::GetKernelObjectName(name, ns);

		void* handle = ::OpenFileMappingW(*memoryAccessWin32, FALSE, !fullName.IsEmpty() ? fullName.wc_str() : nullptr);
		if (handle && handle != INVALID_HANDLE_VALUE)
		{
			buffer = ::MapViewOfFile(handle, *memoryAccessWin32, 0, 0, size);
			if (buffer)
			{
				return handle;
			}
			::CloseHandle(handle);
		}
		return nullptr;
	}
	void FreeSharedMemoryRegion(void* handle, void* buffer) noexcept
	{
		if (buffer)
		{
			::UnmapViewOfFile(buffer);
		}
		if (handle)
		{
			::CloseHandle(handle);
		}
	}
}

namespace kxf
{
	MemoryInputStream SharedMemoryBuffer::DoGetInputStream(size_t size, bool unchecked) const noexcept
	{
		if ((unchecked || m_Buffer) && m_Protection.Contains(MemoryProtection::Read))
		{
			return MemoryInputStream(m_Buffer, size);
		}
		return {};
	}
	MemoryOutputStream SharedMemoryBuffer::DoGetOutputStream(size_t size, bool unchecked) noexcept
	{
		if ((unchecked || m_Buffer) && m_Protection.Contains(MemoryProtection::Write))
		{
			return MemoryOutputStream(m_Buffer, size);
		}
		return {};
	}

	void SharedMemoryBuffer::ZeroBuffer() noexcept
	{
		Utility::SecureZeroMemory(m_Buffer, m_Size);
	}
}
