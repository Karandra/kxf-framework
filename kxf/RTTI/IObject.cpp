#include "kxf-pch.h"
#include "IObject.h"
#include "ClassInfo.h"
#include "kxf/Core/String.h"
#include "kxf/Core/StupidMemoryAllocator.h"

namespace
{
	std::array<uint8_t, 128> g_UnownedRefStorage;
	kxf::StupidMemoryAllocator g_UnownedRefAllocator;

	template<class T>
	class FixedAllocator final
	{
		template<class Tx>
		friend class FixedAllocator;

		public:
			using value_type = T;
			using size_type = size_t;
			using difference_type = ptrdiff_t;

		public:
			FixedAllocator() noexcept = default;

			template<class Tx>
			FixedAllocator(const FixedAllocator<Tx>& other) noexcept
			{
			}

			FixedAllocator(const FixedAllocator&) = default;

		public:
			T* allocate(size_t count)
			{
				g_UnownedRefAllocator.AttachPool(g_UnownedRefStorage.data(), g_UnownedRefStorage.size());

				auto ptr = g_UnownedRefAllocator.Allocate(count * sizeof(T));
				if (!ptr)
				{
					throw std::bad_alloc();
				}

				return static_cast<T*>(ptr);
			}
			void deallocate(T* ptr, size_t count)
			{
				if (!g_UnownedRefAllocator.Free(ptr))
				{
					throw std::bad_alloc();
				}
			}

		public:
			FixedAllocator& operator=(const FixedAllocator&) = default;
	};

	struct OwnerlessDeleter final
	{
		constexpr void operator()(kxf::IObject*) noexcept
		{
		}
	};

	std::shared_ptr<kxf::IObject> g_UnownedRef = {nullptr, OwnerlessDeleter(), FixedAllocator<kxf::IObject>()};
	kxf::RTTI::InterfaceClassInfo<kxf::IObject> g_ClassInfo;

	//#define RTTI_DEBUG 1
	#ifdef RTTI_DEBUG
	thread_local std::atomic<size_t> g_DbgPrintIndent = 0;
	#endif
}

namespace kxf::RTTI
{
	void DebugPrint(const char* str) noexcept
	{
		#ifdef RTTI_DEBUG
		auto Indent = [](size_t indent)
		{
			for (size_t i = 0; i < indent; i++)
			{
				::OutputDebugStringA("\t");
			}
		};

		std::string_view out = str ? str : "<null>";
		if (out.starts_with("Enter:"))
		{
			Indent(g_DbgPrintIndent++);
		}
		else if (out.starts_with("Leave:"))
		{
			Indent(--g_DbgPrintIndent);
		}
		else
		{
			Indent(g_DbgPrintIndent);
		}

		::OutputDebugStringA(out.data());
		::OutputDebugStringA("\n");
		#endif
	}
}

namespace kxf
{
	const RTTI::ClassInfo& IObject::ms_ClassInfo = g_ClassInfo;
	std::shared_ptr<IObject>& IObject::ms_UnownedRef = g_UnownedRef;

	RTTI::QueryInfo IObject::DoQueryInterface(const IID& iid) noexcept
	{
		RTTI::DebugPrint("Enter: " __FUNCSIG__);

		if (iid.IsOfType<IObject>())
		{
			RTTI::DebugPrint("Requested IObject -> success");
			RTTI::DebugPrint("Leave: " __FUNCSIG__);

			return *this;
		}
		else if (iid.IsOfType<RTTI::ClassInfo>())
		{
			RTTI::DebugPrint("Requested RTTI::ClassInfo -> success");
			RTTI::DebugPrint("Leave: " __FUNCSIG__);

			return static_cast<RTTI::ClassInfo&>(g_ClassInfo);
		}

		RTTI::DebugPrint("Leave: " __FUNCSIG__);
		return nullptr;
	}
}
