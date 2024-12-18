#include "kxf-pch.h"
#include "StupidMemoryAllocator.h"
#include "kxf/Threading/LockGuard.h"

namespace kxf
{
	// StupidMemoryAllocator
	bool StupidMemoryAllocator::ConstructPool(void* pool, size_t poolSize, size_t alignment, size_t regionSize, size_t pageCount, FlagSet<MemoryAllocatorFlag> flags) noexcept
	{
		m_PoolSource = pool;

		// Align the pool if needed
		if (alignment != 0)
		{
			void* aligned = pool;
			size_t size = poolSize;
			if (std::align(alignment, poolSize, aligned, size))
			{
				pool = aligned;
				poolSize = size;
			}
			else
			{
				DoFreePool();
				return false;
			}
		}

		m_Pool = pool;
		m_PoolSize = poolSize;
		m_PoolAlignment = alignment;

		// Don't align the region, we don't really need it and 'Allocate' function will align the requested block as needed
		m_Region = reinterpret_cast<uint8_t*>(pool) + (poolSize - regionSize);
		m_RegionSize = regionSize;
		m_PageCount = pageCount;

		// Initialize pages
		auto pages = static_cast<PageInfo*>(pool);
		for (size_t i = 0; i < pageCount; i++)
		{
			pages[i] = {};
		}

		if (flags.Contains(MemoryAllocatorFlag::ZeroMemory))
		{
			std::memset(m_Region, 0, regionSize);
		}

		return true;
	}
	bool StupidMemoryAllocator::DoAllocatePool(void* buffer, size_t size, size_t alignment, FlagSet<MemoryAllocatorFlag> flags) noexcept
	{
		DoFreePool();

		const size_t pageCount = size / m_PageSize;
		const size_t controlTableSize = sizeof(PageInfo) * pageCount;

		if (size != 0 && pageCount != 0)
		{
			if (buffer)
			{
				// We need at least some extra space for the attached pool to have any meaningful allocation facilities
				if (size >= controlTableSize + m_PageSize)
				{
					m_PoolAttached = true;
					return ConstructPool(buffer, size, alignment, size - controlTableSize, pageCount, flags);
				}
			}
			else
			{
				// Allocate the required region size + control table size
				if (buffer = DoAllocateMemory(size + controlTableSize + alignment))
				{
					return ConstructPool(buffer, size + controlTableSize + alignment, alignment, size, pageCount, flags);
				}
			}
		}
		return false;
	}
	void StupidMemoryAllocator::DoFreePool() noexcept
	{
		if (!m_PoolAttached)
		{
			DoFreeMemory(m_PoolSource);
		}

		m_Pool = nullptr;
		m_PoolSource = nullptr;
		m_PoolSize = 0;
		m_PoolAlignment = 0;
		m_PoolAttached = false;

		m_Region = nullptr;
		m_RegionSize = 0;
		m_PageCount = 0;
		m_FirstFreePage = 0;

		m_PagesUsed = 0;
		m_BytesRequested = 0;
	}

	void* StupidMemoryAllocator::DoAllocateMemory(size_t size) noexcept
	{
		return std::malloc(size);
	}
	void StupidMemoryAllocator::DoFreeMemory(void* ptr) noexcept
	{
		std::free(ptr);
	}

	// IMemoryPoolAllocator
	void* StupidMemoryAllocator::AllocatePool(size_t size, size_t alignment, FlagSet<MemoryAllocatorFlag> flags) noexcept
	{
		WriteLockGuard lock(m_Lock);

		if (m_Pool)
		{
			return nullptr;
		}

		if (!DoAllocatePool(nullptr, size, alignment, flags))
		{
			DoFreePool();
			return nullptr;
		}
		return m_Pool;
	}
	void* StupidMemoryAllocator::AttachPool(void* buffer, size_t size, size_t alignment, FlagSet<MemoryAllocatorFlag> flags) noexcept
	{
		WriteLockGuard lock(m_Lock);

		if (m_Pool || !buffer)
		{
			return nullptr;
		}

		if (!DoAllocatePool(buffer, size, alignment, flags))
		{
			DoFreePool();
			return nullptr;
		}
		return m_Pool;
	}
	void StupidMemoryAllocator::FreePool() noexcept
	{
		WriteLockGuard lock(m_Lock);

		DoFreePool();
	}
	size_t StupidMemoryAllocator::GetPoolSize() const noexcept
	{
		ReadLockGuard lock(m_Lock);

		return m_PoolSize;
	}

	size_t StupidMemoryAllocator::GetPageSize() const noexcept
	{
		ReadLockGuard lock(m_Lock);

		return m_PageSize;
	}
	bool StupidMemoryAllocator::SetPageSize(size_t size) noexcept
	{
		WriteLockGuard lock(m_Lock);

		if (!m_Pool)
		{
			m_PageSize = std::max(size, sizeof(RegionInfo));
			return true;
		}
		return false;
	}

	// IMemoryAllocator
	void* StupidMemoryAllocator::Allocate(size_t size, size_t alignment, FlagSet<MemoryAllocatorFlag> flags) noexcept
	{
		if (size == 0)
		{
			return &m_DummyRegion;
		}

		const size_t actualSize = size + sizeof(RegionInfo);
		if (actualSize > m_RegionSize)
		{
			return nullptr;
		}
		if (alignment != 0)
		{
			// We don't support this yet
			return nullptr;
		}

		auto pages = static_cast<PageInfo*>(m_Pool);
		const size_t pagesRequired = (actualSize / m_PageSize) + 1;

		WriteLockGuard lock(m_Lock);
		for (size_t i = m_FirstFreePage; i < m_PageCount;)
		{
			auto& page = pages[i];
			if (page.RegionInfo)
			{
				// Look for next pages
				i += std::max<size_t>(page.RegionInfo->UsedNext, 1);
			}
			else
			{
				// Look for the rest of the contiguous region
				bool regionFree = true;
				size_t scanned = 0;
				for (size_t k = 1; (k < pagesRequired && i + k < m_PageCount); k++)
				{
					scanned++;
					if (pages[i + k].RegionInfo)
					{
						regionFree = false;
						break;
					}
				}

				if (regionFree)
				{
					// We allocate the region info structure at the start of the region and point every page info
					// for the entire region to it.
					auto region = reinterpret_cast<RegionInfo*>(reinterpret_cast<uint8_t*>(m_Region) + (i * m_PageSize));
					void* ptr = reinterpret_cast<uint8_t*>(region) + sizeof(RegionInfo);

					// Write control structure information
					region->Alignment = alignment;
					region->AllocationSize = size;
					region->UsedNext = pagesRequired - 1;
					region->Flags = flags;
					for (size_t k = 0; k < pagesRequired; k++)
					{
						pages[i + k].RegionInfo = region;
					}

					// Update free pages counter
					if (i >= m_FirstFreePage)
					{
						m_FirstFreePage = i + pagesRequired;
					}

					// Statistics
					m_PagesUsed += pagesRequired;
					m_BytesRequested += size;

					// Zero the memory if required
					if (flags.Contains(MemoryAllocatorFlag::ZeroMemory))
					{
						std::memset(ptr, 0, size);
					}

					return ptr;
				}
				else
				{
					i += scanned;
					continue;
				}
			}
		}
		return nullptr;
	}
	bool StupidMemoryAllocator::Free(void* ptr, size_t alignment) noexcept
	{
		WriteLockGuard lock(m_Lock);

		return ProcessPointer(ptr, [&](PageInfo* pages, size_t pageIndex, RegionInfo& regionInfo)
		{
			const size_t pageCount = regionInfo.UsedNext != 0 ? static_cast<size_t>(regionInfo.UsedNext) + 1 : 1;

			// Statistics
			m_PagesUsed -= pageCount;
			m_BytesRequested -= regionInfo.AllocationSize;

			// Deallocate the region
			for (size_t i = 0; i < pageCount; i++)
			{
				pages[pageIndex + i].RegionInfo = nullptr;
			}

			// Update free the free pages counter
			if (pageIndex < m_FirstFreePage)
			{
				m_FirstFreePage = pageIndex;
			}

			return true;
		}, false);
	}
	auto StupidMemoryAllocator::QueryAllocationInfo(void* ptr, size_t alignment) const noexcept -> AllocationInfo
	{
		ReadLockGuard lock(m_Lock);

		return const_cast<StupidMemoryAllocator&>(*this).ProcessPointer<AllocationInfo>(ptr, [](PageInfo* pages, size_t pageIndex, RegionInfo& regionInfo)
		{
			AllocationInfo info;
			info.Size = regionInfo.AllocationSize;
			info.Alignment = regionInfo.Alignment;
			info.Flags = regionInfo.Flags;

			return info;
		}, {});
	}
}
