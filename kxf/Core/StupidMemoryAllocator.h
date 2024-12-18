#pragma once
#include "Common.h"
#include "IMemoryAllocator.h"
#include "kxf/Threading/ReadWriteLock.h"

namespace kxf
{
	class KXF_API StupidMemoryAllocator: public RTTI::Implementation<StupidMemoryAllocator, IMemoryPoolAllocator, IMemoryAllocator>
	{
		private:
			struct RegionInfo
			{
				size_t AllocationSize = 0;
				uint32_t Alignment = 0;
				uint32_t UsedNext = 0;
				FlagSet<MemoryAllocatorFlag> Flags;
			};
			struct PageInfo
			{
				RegionInfo* RegionInfo = nullptr;
			};

		private:
			void* m_Pool = nullptr;
			void* m_PoolSource = nullptr;
			size_t m_PoolSize = 0;
			size_t m_PoolAlignment = 0;
			bool m_PoolAttached = false;

			RegionInfo m_DummyRegion = {};
			void* m_Region = nullptr;
			size_t m_RegionSize = 0;
			size_t m_PageCount = 0;
			size_t m_FirstFreePage = 0;

			std::atomic<size_t> m_PageSize = sizeof(RegionInfo);
			std::atomic<size_t> m_PagesUsed = 0;
			std::atomic<size_t> m_BytesRequested = 0;

			mutable ReadWriteLock m_Lock;

		private:
			bool ConstructPool(void* pool, size_t poolSize, size_t alignment, size_t regionSize, size_t pageCount, FlagSet<MemoryAllocatorFlag> flags) noexcept;
			bool DoAllocatePool(void* buffer, size_t size, size_t alignment, FlagSet<MemoryAllocatorFlag> flags) noexcept;
			void DoFreePool() noexcept;

			template<class T, class TFunc>
			T ProcessPointer(void* ptr, TFunc&& func, T&& nullValue) noexcept
			{
				if (!ptr || ptr == &m_DummyRegion)
				{
					// Null pointer or zero-allocated region
					return nullValue;
				}
				else if (ptr < m_Region || reinterpret_cast<size_t>(ptr) > reinterpret_cast<size_t>(m_Region) + m_RegionSize)
				{
					// Memory outside of the allocated region
					return nullValue;
				}

				auto pages = static_cast<PageInfo*>(m_Pool);
				const size_t pageIndex = (reinterpret_cast<size_t>(ptr) - sizeof(RegionInfo) - reinterpret_cast<size_t>(m_Region)) / m_PageSize;
				if (pageIndex > m_PageCount)
				{
					return nullValue;
				}

				if (auto& regionInfo = pages[pageIndex].RegionInfo)
				{
					return static_cast<T>(std::invoke(func, pages, pageIndex, *regionInfo));
				}
				return nullValue;
			}

		protected:
			virtual void* DoAllocateMemory(size_t size) noexcept;
			virtual void DoFreeMemory(void* ptr) noexcept;

		public:
			StupidMemoryAllocator() noexcept = default;
			StupidMemoryAllocator(const StupidMemoryAllocator&) = delete;
			~StupidMemoryAllocator()
			{
				DoFreePool();
			}

		public:
			// IMemoryPoolAllocator
			void* AllocatePool(size_t size, size_t alignment = 0, FlagSet<MemoryAllocatorFlag> flags = {}) noexcept override;
			void* AttachPool(void* buffer, size_t size, size_t alignment = 0, FlagSet<MemoryAllocatorFlag> flags = {}) noexcept override;
			void FreePool() noexcept override;
			size_t GetPoolSize() const noexcept override;

			size_t GetPageSize() const noexcept override;
			bool SetPageSize(size_t size) noexcept override;

			// IMemoryAllocator
			FlagSet<MemoryAllocatorCapabilities> GetAllocatorCapabilities() const noexcept override
			{
				return MemoryAllocatorCapabilities::QueryInfo|MemoryAllocatorCapabilities::AllocationTracking;
			}

			void* Allocate(size_t size, size_t alignment = 0, FlagSet<MemoryAllocatorFlag> flags = {}) noexcept override;
			bool Free(void* ptr, size_t alignment = 0) noexcept override;
			AllocationInfo QueryAllocationInfo(void* ptr, size_t alignment = 0) const noexcept override;

			size_t GetRequestedBytes() const noexcept override
			{
				return m_BytesRequested;
			}
			size_t GetAllocatedBytes() const noexcept override
			{
				return m_PagesUsed * m_PageSize;
			}

			// StupidMemoryAllocator
			size_t GetAllocatedPages() const noexcept
			{
				return m_PagesUsed;
			}

		public:
			StupidMemoryAllocator& operator=(const StupidMemoryAllocator&) = delete;
	};
}
