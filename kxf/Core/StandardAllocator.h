#pragma once
#include "Common.h"
#include "IMemoryAllocator.h"

namespace kxf
{
	enum class StandardAllocatorKind
	{
		Unknown = -1,

		CRuntime,
		SystemHeap,
		SystemLocal,
		SystemGlobal,
		SystemVirtual,
		SystemCoTask
	};

	struct StandardAllocatorConfig
	{
	};
	struct SystemHeapAllocatorConfig: public StandardAllocatorConfig
	{
		void* Heap = nullptr;
	};
	struct SystemVirtualAllocatorConfig: public StandardAllocatorConfig
	{
		void* Process = nullptr;
		void* BaseAddress = nullptr;
		FlagSet<uint32_t> AllocationType = 0;
		FlagSet<uint32_t> DeallocationType = 0;
		FlagSet<uint32_t> PageProtection = 0;
	};
}

namespace kxf
{
	KXF_API std::shared_ptr<IMemoryAllocator> GetStandardAllocator(StandardAllocatorKind kind, const StandardAllocatorConfig* config = nullptr);
}

namespace kxf
{
	extern IMemoryAllocator& DefaultMemoryAllocator;

	template<class T>
	class StdDefaultMemoryAllocator final
	{
		public:
			using value_type = T;
			using size_type = size_t;
			using difference_type = ptrdiff_t;

		public:
			StdDefaultMemoryAllocator() noexcept = default;

			template<class Tx>
			StdDefaultMemoryAllocator(const StdDefaultMemoryAllocator<Tx>& other) noexcept
			{
			}

		public:
			IMemoryAllocator& get_allocator() const noexcept
			{
				return DefaultMemoryAllocator;
			}
			FlagSet<MemoryAllocatorFlag> get_allocation_flags() const noexcept
			{
				return {};
			}

			T* allocate(size_t count)
			{
				auto ptr = get_allocator().Allocate(count * sizeof(T), 0, get_allocation_flags());
				if (!ptr)
				{
					throw std::bad_alloc();
				}

				return static_cast<T*>(ptr);
			}
			void deallocate(T* ptr, size_t count)
			{
				if (!get_allocator().Free(ptr))
				{
					throw std::bad_alloc();
				}
			}

		public:
			template<class Tx>
			StdDefaultMemoryAllocator& operator=(const StdDefaultMemoryAllocator<Tx>& other) noexcept
			{
				return *this;
			}
	};
}
