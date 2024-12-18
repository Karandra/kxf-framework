#pragma once
#include "kxf-pch.h"
#include "StandardAllocator.h"
#include "kxf/Log/ScopedLogger.h"
#include "kxf/Log/Categories.h"
#include "kxf/System/COM.h"
#include "kxf/System/HResult.h"
#include "kxf/System/Win32Error.h"
#include "kxf/Utility/Common.h"

#include <malloc.h>
#include <Windows.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf::Private::StandardAllocator
{
	class CRuntimeAlloc final: public RTTI::Implementation<CRuntimeAlloc, IMemoryAllocator>
	{
		public:
			// IMemoryAllocator
			FlagSet<MemoryAllocatorCapabilities> GetAllocatorCapabilities() const noexcept override
			{
				return MemoryAllocatorCapabilities::QueryInfo|MemoryAllocatorCapabilities::Alignment;
			}

			void* Allocate(size_t size, size_t alignment = 0, FlagSet<MemoryAllocatorFlag> flags = {}) noexcept override
			{
				void* ptr = alignment != 0 ? ::_aligned_malloc(size, alignment) : ::malloc(size);
				if (ptr && flags.Contains(MemoryAllocatorFlag::ZeroMemory))
				{
					std::memset(ptr, 0, size);
				}

				return ptr;
			}
			bool Free(void* ptr, size_t alignment = 0) noexcept override
			{
				if (alignment != 0)
				{
					::_aligned_free(ptr);
				}
				else
				{
					::free(ptr);
				}
				return true;
			}
			AllocationInfo QueryAllocationInfo(void* ptr, size_t alignment = 0) const noexcept override
			{
				size_t size = alignment != 0 ? ::_aligned_msize(ptr, alignment, 0) : ::_msize(ptr);
				return {size, 0, {}};
			}

			size_t GetRequestedBytes() const noexcept override
			{
				return 0;
			}
			size_t GetAllocatedBytes() const noexcept override
			{
				return 0;
			}
	};

	class LocalAlloc final: public RTTI::Implementation<LocalAlloc, IMemoryAllocator>
	{
		public:
			// IMemoryAllocator
			FlagSet<MemoryAllocatorCapabilities> GetAllocatorCapabilities() const noexcept override
			{
				return MemoryAllocatorCapabilities::QueryInfo;
			}

			void* Allocate(size_t size, size_t alignment = 0, FlagSet<MemoryAllocatorFlag> flags = {}) noexcept override
			{
				FlagSet<DWORD> nativeFlags = LMEM_FIXED;
				nativeFlags.Add(LMEM_ZEROINIT, flags.Contains(MemoryAllocatorFlag::ZeroMemory));

				return ::LocalAlloc(*nativeFlags, size);
			}
			bool Free(void* ptr, size_t alignment = 0) noexcept override
			{
				return ::LocalFree(ptr) == nullptr;
			}
			AllocationInfo QueryAllocationInfo(void* ptr, size_t alignment = 0) const noexcept override
			{
				return {::LocalSize(ptr), 0, {}};
			}

			size_t GetRequestedBytes() const noexcept override
			{
				return 0;
			}
			size_t GetAllocatedBytes() const noexcept override
			{
				return 0;
			}
	};

	class GlobalAlloc final: public RTTI::Implementation<GlobalAlloc, IMemoryAllocator>
	{
		public:
			// IMemoryAllocator
			FlagSet<MemoryAllocatorCapabilities> GetAllocatorCapabilities() const noexcept override
			{
				return MemoryAllocatorCapabilities::QueryInfo;
			}

			void* Allocate(size_t size, size_t alignment = 0, FlagSet<MemoryAllocatorFlag> flags = {}) noexcept override
			{
				FlagSet<DWORD> nativeFlags = GMEM_FIXED;
				nativeFlags.Add(GMEM_ZEROINIT, flags.Contains(MemoryAllocatorFlag::ZeroMemory));

				return ::GlobalAlloc(*nativeFlags, size);
			}
			bool Free(void* ptr, size_t alignment = 0) noexcept override
			{
				return ::GlobalFree(ptr) == nullptr;
			}
			AllocationInfo QueryAllocationInfo(void* ptr, size_t alignment = 0) const noexcept override
			{
				return {::GlobalSize(ptr), 0, {}};
			}

			size_t GetRequestedBytes() const noexcept override
			{
				return 0;
			}
			size_t GetAllocatedBytes() const noexcept override
			{
				return 0;
			}
	};

	class CoTaskAlloc final: public RTTI::Implementation<CoTaskAlloc, IMemoryAllocator>
	{
		private:
			COMPtr<IMalloc> m_Alloc;

		public:
			CoTaskAlloc() noexcept
			{
				HResult hr = ::CoGetMalloc(1, &m_Alloc);
				if (!hr)
				{
					Log::ErrorCategory(LogCategory::WinAPI, "CoGetMalloc failed: {} '{}'", hr.GetValue(), hr.GetMessage());
				}
			}

		public:
			// IMemoryAllocator
			FlagSet<MemoryAllocatorCapabilities> GetAllocatorCapabilities() const noexcept override
			{
				return MemoryAllocatorCapabilities::QueryInfo;
			}

			void* Allocate(size_t size, size_t alignment = 0, FlagSet<MemoryAllocatorFlag> flags = {}) noexcept override
			{
				if (m_Alloc)
				{
					auto ptr = m_Alloc->Alloc(size);
					if (ptr && flags.Contains(MemoryAllocatorFlag::ZeroMemory))
					{
						std::memset(ptr, 0, size);
					}
					return ptr;
				}
				return nullptr;
			}
			bool Free(void* ptr, size_t alignment = 0) noexcept override
			{
				if (m_Alloc)
				{
					m_Alloc->Free(ptr);
					return true;
				}
				return false;
			}
			AllocationInfo QueryAllocationInfo(void* ptr, size_t alignment = 0) const noexcept override
			{
				if (m_Alloc)
				{
					return {m_Alloc->GetSize(ptr), 0, {}};
				}
				return {};
			}

			size_t GetRequestedBytes() const noexcept override
			{
				return 0;
			}
			size_t GetAllocatedBytes() const noexcept override
			{
				return 0;
			}
	};

	class HeapAlloc final: public RTTI::Implementation<HeapAlloc, IMemoryAllocator>
	{
		private:
			SystemHeapAllocatorConfig m_Config;

		private:
			bool WalkHeap(size_t& requested, size_t& total, size_t& committed) const noexcept
			{
				if (::HeapLock(m_Config.Heap))
				{
					Utility::ScopeGuard atExit = [&]()
					{
						::HeapUnlock(m_Config.Heap);
					};

					PROCESS_HEAP_ENTRY heapEntry = {};
					Win32Error::SetLastError(ERROR_SUCCESS);
					while (::HeapWalk(m_Config.Heap, &heapEntry))
					{
						if (heapEntry.wFlags & PROCESS_HEAP_ENTRY_BUSY)
						{
							requested += heapEntry.cbData;
							total += heapEntry.cbData + heapEntry.cbOverhead;
						}
						if (heapEntry.wFlags & PROCESS_HEAP_REGION)
						{
							committed += heapEntry.Region.dwCommittedSize;
						}
					}
					return Win32Error::GetLastError() == ERROR_NO_MORE_ITEMS;
				}
				return false;
			}

		public:
			HeapAlloc(const SystemHeapAllocatorConfig* config) noexcept
			{
				if (config)
				{
					m_Config = *config;
					if (!m_Config.Heap)
					{
						Log::ErrorCategory(LogCategory::WinAPI, "Null heap provided");
					}
				}
				else
				{
					m_Config.Heap = ::GetProcessHeap();
					if (!m_Config.Heap)
					{
						auto error = Win32Error::GetLastError();
						Log::ErrorCategory(LogCategory::WinAPI, "GetProcessHeap failed: {}", error);
					}
				}
			}
			~HeapAlloc()
			{
				if (m_Config.Heap && m_Config.Heap != ::GetProcessHeap())
				{
					::HeapDestroy(m_Config.Heap);
				}
			}

		public:
			// IMemoryAllocator
			FlagSet<MemoryAllocatorCapabilities> GetAllocatorCapabilities() const noexcept override
			{
				return MemoryAllocatorCapabilities::QueryInfo|MemoryAllocatorCapabilities::AllocationTracking;
			}

			void* Allocate(size_t size, size_t alignment = 0, FlagSet<MemoryAllocatorFlag> flags = {}) noexcept override
			{
				FlagSet<DWORD> nativeFlags;
				nativeFlags.Add(HEAP_ZERO_MEMORY, flags.Contains(MemoryAllocatorFlag::ZeroMemory));

				return ::HeapAlloc(m_Config.Heap, *nativeFlags, size);
			}
			bool Free(void* ptr, size_t alignment = 0) noexcept override
			{
				return ::HeapFree(m_Config.Heap, 0, ptr);
			}
			AllocationInfo QueryAllocationInfo(void* ptr, size_t alignment = 0) const noexcept override
			{
				auto value = ::HeapSize(m_Config.Heap, 0, ptr);
				if (value != std::numeric_limits<decltype(value)>::max())
				{
					return {value, 0, {}};
				}
				return {};
			}

			size_t GetRequestedBytes() const noexcept override
			{
				size_t requested = 0;
				size_t total = 0;
				size_t comitted = 0;
				if (WalkHeap(requested, total, comitted))
				{
					return requested;
				}
				return 0;
			}
			size_t GetAllocatedBytes() const noexcept override
			{
				size_t requested = 0;
				size_t total = 0;
				size_t comitted = 0;
				if (WalkHeap(requested, total, comitted))
				{
					return total;
				}
				return 0;
			}
	};

	class VirtualAlloc final: public RTTI::Implementation<VirtualAlloc, IMemoryAllocator>
	{
		private:
			SystemVirtualAllocatorConfig m_Config;

		public:
			VirtualAlloc(const SystemVirtualAllocatorConfig* config) noexcept
			{
				if (config)
				{
					m_Config = *config;
				}

				if (*m_Config.AllocationType == 0)
				{
					m_Config.AllocationType = MEM_COMMIT|MEM_RESERVE;
				}
				if (*m_Config.DeallocationType == 0)
				{
					m_Config.DeallocationType = MEM_RELEASE;
				}
			}

		public:
			// IMemoryAllocator
			FlagSet<MemoryAllocatorCapabilities> GetAllocatorCapabilities() const noexcept override
			{
				return MemoryAllocatorCapabilities::QueryInfo;
			}

			void* Allocate(size_t size, size_t alignment = 0, FlagSet<MemoryAllocatorFlag> flags = {}) noexcept override
			{
				void* ptr = nullptr;
				if (m_Config.Process)
				{
					ptr = ::VirtualAllocEx(m_Config.Process, m_Config.BaseAddress, size, *m_Config.AllocationType, *m_Config.PageProtection);
				}
				else
				{
					ptr = ::VirtualAlloc(m_Config.BaseAddress, size, *m_Config.AllocationType, *m_Config.PageProtection);
				}

				if (ptr && flags.Contains(MemoryAllocatorFlag::ZeroMemory) && !m_Config.AllocationType.Contains(MEM_COMMIT))
				{
					std::memset(ptr, 0, size);
				}
				return ptr;
			}
			bool Free(void* ptr, size_t alignment = 0) noexcept override
			{
				size_t size = 0;
				if (m_Config.Process)
				{
					return ::VirtualFreeEx(m_Config.Process, ptr, size, *m_Config.DeallocationType);
				}
				else
				{
					return ::VirtualFree(ptr, size, *m_Config.DeallocationType);
				}
			}
			AllocationInfo QueryAllocationInfo(void* ptr, size_t alignment = 0) const noexcept override
			{
				size_t result = 0;
				MEMORY_BASIC_INFORMATION memoryInfo = {};
				if (m_Config.Process)
				{
					result = ::VirtualQueryEx(m_Config.Process, ptr, &memoryInfo, sizeof(memoryInfo));
				}
				else
				{
					result = ::VirtualQuery(ptr, &memoryInfo, sizeof(memoryInfo));
				}

				if (result != 0)
				{
					return AllocationInfo{memoryInfo.RegionSize, 0, {}};
				}
				return {};
			}

			size_t GetRequestedBytes() const noexcept override
			{
				return 0;
			}
			size_t GetAllocatedBytes() const noexcept override
			{
				return 0;
			}
	};
}

namespace
{
	kxf::Private::StandardAllocator::CRuntimeAlloc g_DefaultMemoryAllocator;
}

namespace kxf
{
	std::shared_ptr<IMemoryAllocator> GetStandardAllocator(StandardAllocatorKind kind, const StandardAllocatorConfig* config)
	{
		switch (kind)
		{
			case StandardAllocatorKind::SystemHeap:
			{
				return std::make_shared<Private::StandardAllocator::HeapAlloc>(static_cast<const SystemHeapAllocatorConfig*>(config));
			}
			case StandardAllocatorKind::SystemLocal:
			{
				return std::make_shared<Private::StandardAllocator::LocalAlloc>();
			}
			case StandardAllocatorKind::SystemGlobal:
			{
				return std::make_shared<Private::StandardAllocator::GlobalAlloc>();
			}
			case StandardAllocatorKind::SystemVirtual:
			{
				return std::make_shared<Private::StandardAllocator::VirtualAlloc>(static_cast<const SystemVirtualAllocatorConfig*>(config));
			}
			case StandardAllocatorKind::SystemCoTask:
			{
				return std::make_shared<Private::StandardAllocator::CoTaskAlloc>();
			}
		};
		return nullptr;
	}
}

namespace kxf
{
	IMemoryAllocator& DefaultMemoryAllocator = g_DefaultMemoryAllocator;
}
