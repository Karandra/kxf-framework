#pragma once
#include "Common.h"
#include "IID.h"
#include "kxf/Core/FlagSet.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/TypeTraits.h"

namespace kxf::RTTI
{
	class ClassInfo;

	template<std::derived_from<IObject> T>
	std::shared_ptr<T> assume_non_owned(T& value) noexcept
	{
		return {IObject::ms_UnownedRef, &value};
	}
}

namespace kxf::RTTI
{
	class QueryInfo final
	{
		private:
			void* m_Ref = nullptr;
			std::shared_ptr<void> m_Lock;
			const std::type_info* m_TypeInfo = nullptr;

		public:
			QueryInfo() noexcept = default;
			QueryInfo(std::nullptr_t) noexcept
			{
			}

			template<std::derived_from<IObject> T>
			QueryInfo(T& ref) noexcept
				:m_TypeInfo(&typeid(T))
			{
				if (auto weak = ref.weak_from_this(); weak.expired())
				{
					// Assume it was never allocated on the heap
					m_Lock = assume_non_owned(ref);
				}
				else
				{
					m_Lock = weak.lock();
				}
				m_Ref = &ref;
			}

			template<std::derived_from<IObject> T>
			QueryInfo(std::shared_ptr<T> ptr) noexcept
				:m_Ref(m_Lock.get()), m_Lock(std::move(ptr)), m_TypeInfo(&typeid(T))
			{
			}

			QueryInfo(const QueryInfo&) = delete;
			QueryInfo(QueryInfo&&) noexcept = default;
			~QueryInfo() = default;

		public:
			bool IsNull() const noexcept
			{
				return m_Ref == nullptr || m_Lock == nullptr;
			}
			const char* GetTypeName() const noexcept
			{
				return m_TypeInfo ? m_TypeInfo->name() : nullptr;
			}

			template<std::derived_from<IObject> T>
			std::shared_ptr<T> LockObject() const&& noexcept
			{
				if (m_Ref)
				{
					return std::shared_ptr<T>(m_Lock, reinterpret_cast<T*>(m_Ref));
				}
				return nullptr;
			}

			template<std::derived_from<IObject> T>
			std::shared_ptr<T> LockObject() && noexcept
			{
				if (void* ref = m_Ref)
				{
					m_Ref = nullptr;
					return std::shared_ptr<T>(std::move(m_Lock), reinterpret_cast<T*>(ref));
				}
				return nullptr;
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

			QueryInfo& operator=(const QueryInfo&) = delete;
			QueryInfo& operator=(QueryInfo&&) noexcept = default;
	};

	void DebugPrint(const char* str) noexcept;

	template<class T>
	RTTI::QueryInfo Cast(T& object, const IID& iid) noexcept
	{
		static_assert((std::is_base_of_v<IObject, T>), "T must inherit from 'IObject'");
		DebugPrint("Enter: " __FUNCSIG__);

		if (iid.IsOfType<T>())
		{
			DebugPrint("Cast -> success");
			DebugPrint("Leave: " __FUNCSIG__);
			return object;
		}

		DebugPrint("Leave: " __FUNCSIG__);
		return nullptr;
	}

	template<class... Args>
	RTTI::QueryInfo UseAnyOf(const IID& iid, std::add_lvalue_reference_t<Args>&&... arg) noexcept
	{
		DebugPrint("Enter: " __FUNCSIG__);

		RTTI::QueryInfo query;
		if (((query = Cast<Args>(arg, iid), !query.IsNull()) || ...))
		{
			DebugPrint("UseAnyOf -> success");
			DebugPrint(query.GetTypeName());
			DebugPrint("Leave: " __FUNCSIG__);

			return query;
		}

		DebugPrint("Leave: " __FUNCSIG__);
		return nullptr;
	}
}

namespace kxf
{
	class KX_API IObject: public std::enable_shared_from_this<IObject>
	{
		template<class T>
		friend constexpr IID RTTI::GetInterfaceID() noexcept;

		template<class T>
		friend const RTTI::ClassInfo& RTTI::GetClassInfo() noexcept;

		template<std::derived_from<IObject> T>
		friend std::shared_ptr<T> RTTI::assume_non_owned(T&) noexcept;

		friend class RTTI::QueryInfo;

		private:
			static constexpr IID ms_IID = NativeUUID{0, 0, 0, {0xC0, 0, 0x4f, 0x62, 0x6a, 0x65, 0x63, 0x74}};
			static const RTTI::ClassInfo& ms_ClassInfo;
			static std::shared_ptr<IObject>& ms_UnownedRef;

		protected:
			virtual RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept = 0;

		public:
			IObject() noexcept = default;
			virtual ~IObject() = default;

		public:
			RTTI::QueryInfo QueryInterface(const IID& iid) noexcept
			{
				return this->DoQueryInterface(iid);
			}
			RTTI::QueryInfo QueryInterface(const IID& iid) const noexcept
			{
				return const_cast<IObject*>(this)->DoQueryInterface(iid);
			}

			template<std::derived_from<IObject> T>
			std::shared_ptr<T> QueryInterface() noexcept
			{
				return this->DoQueryInterface(RTTI::GetInterfaceID<T>()).LockObject<T>();
			}

			template<std::derived_from<IObject> T>
			std::shared_ptr<const T> QueryInterface() const noexcept
			{
				return const_cast<IObject*>(this)->DoQueryInterface(RTTI::GetInterfaceID<T>()).LockObject<const T>();
			}

			template<std::derived_from<IObject> T>
			bool QueryInterface(std::shared_ptr<T>& ptr) noexcept
			{
				ptr = this->QueryInterface<T>();
				return ptr != nullptr;
			}

			template<std::derived_from<IObject> T>
			bool QueryInterface(std::shared_ptr<const T>& ptr) const noexcept
			{
				ptr = this->QueryInterface<T>();
				return ptr != nullptr;
			}
	};
}

#define KxRTTI_DeclareIID(T, ...)	\
friend class kxf::IObject;	\
\
template<class T>	\
friend constexpr kxf::IID kxf::RTTI::GetInterfaceID() noexcept;	\
\
private:	\
	static constexpr kxf::IID ms_IID = kxf::NativeUUID __VA_ARGS__;

#define KxRTTI_DeclareIID_Using(T, iid)	\
friend class kxf::IObject;	\
\
template<class T>	\
friend constexpr kxf::IID kxf::RTTI::GetInterfaceID() noexcept;	\
\
private:	\
	static constexpr kxf::IID ms_IID = (iid);

#define KxRTTI_DeclareIID_External(T, ...)	\
namespace RTTI	\
{	\
	template<>	\
	constexpr kxf::IID GetInterfaceID<T>() noexcept	\
	{	\
		return kxf::NativeUUID __VA_ARGS__;	\
	}	\
}
