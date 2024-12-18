#pragma once
#include "Common.h"
#include "IObject.h"
#include "ClassInfo.h"
#include "kxf/Utility/TypeTraits.h"

namespace kxf::RTTI
{
	template<class T>
	class Interface: public virtual IObject
	{
		template<class T>
		friend const ClassInfo& GetClassInfo() noexcept;

		private:
			static inline RTTI::InterfaceClassInfo<T, IObject> ms_ClassInfo;

		protected:
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override
			{
				RTTI::DebugPrint("Enter: " __FUNCSIG__);

				if (iid.IsOfType<RTTI::ClassInfo>())
				{
					RTTI::DebugPrint("Requested RTTI::ClassInfo -> success");
					RTTI::DebugPrint("Leave: " __FUNCSIG__);

					return static_cast<ClassInfo&>(ms_ClassInfo);
				}
				else if (iid.IsOfType<T>())
				{
					RTTI::DebugPrint("Requested T -> success");
					RTTI::DebugPrint("Leave: " __FUNCSIG__);

					return static_cast<T&>(*this);
				}

				RTTI::DebugPrint("Requesting base");
				auto query = IObject::DoQueryInterface(iid);
				RTTI::DebugPrint(query.GetTypeName());
				RTTI::DebugPrint("Leave: " __FUNCSIG__);

				return query;
			}

		public:
			Interface() = default;
	};

	template<class TDerived, class... TBase>
	class ExtendInterface: public TBase...
	{
		template<class T>
		friend const ClassInfo& GetClassInfo() noexcept;

		protected:
			using TBaseInterface = typename ExtendInterface<TDerived, TBase...>;

		private:
			static inline RTTI::InterfaceClassInfo<TDerived, TBase...> ms_ClassInfo;

		protected:
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override
			{
				static_assert((std::is_base_of_v<IObject, TBase> && ...), "[...] must inherit from 'IObject'");
				RTTI::DebugPrint("Enter: " __FUNCSIG__);

				if (iid.IsOfType<RTTI::ClassInfo>())
				{
					RTTI::DebugPrint("Requested RTTI::ClassInfo -> success");
					RTTI::DebugPrint("Leave: " __FUNCSIG__);

					return static_cast<ClassInfo&>(ms_ClassInfo);
				}
				else if (iid.IsOfType<TDerived>())
				{
					RTTI::DebugPrint("Requested TDerived -> success");
					RTTI::DebugPrint("Leave: " __FUNCSIG__);

					return static_cast<TDerived&>(*this);
				}
				else if (RTTI::QueryInfo query; ((query = TBase::DoQueryInterface(iid), !query.IsNull()) || ...))
				{
					RTTI::DebugPrint("Requested TBase -> success");
					RTTI::DebugPrint(query.GetTypeName());
					RTTI::DebugPrint("Leave: " __FUNCSIG__);

					return query;
				}

				RTTI::DebugPrint("Leave: " __FUNCSIG__);
				return nullptr;
			}

		public:
			ExtendInterface() = default;
	};

	template<class TDerived, class... TBase>
	class Implementation: public TBase...
	{
		template<class T>
		friend const ClassInfo& GetClassInfo() noexcept;

		protected:
			using TBaseClass = typename Implementation<TDerived, TBase...>;

		private:
			static inline RTTI::ImplementationClassInfo<TDerived, TBase...> ms_ClassInfo;

		protected:
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override
			{
				static_assert((std::is_base_of_v<IObject, TBase> && ...), "[...] must inherit from 'IObject'");
				RTTI::DebugPrint("Enter: " __FUNCSIG__);

				if (iid.IsOfType<RTTI::ClassInfo>())
				{
					RTTI::DebugPrint("Requested RTTI::ClassInfo -> success");
					RTTI::DebugPrint("Leave: " __FUNCSIG__);

					return static_cast<ClassInfo&>(ms_ClassInfo);
				}
				else if (RTTI::QueryInfo query; ((query = TBase::DoQueryInterface(iid), !query.IsNull()) || ...))
				{
					RTTI::DebugPrint("Requested TBase -> success");
					RTTI::DebugPrint(query.GetTypeName());
					RTTI::DebugPrint("Leave: " __FUNCSIG__);

					return query;
				}

				RTTI::DebugPrint("Leave: " __FUNCSIG__);
				return nullptr;
			}

		public:
			Implementation() = default;
	};

	template<class TDerived, class... TBase>
	class DynamicImplementation: public TBase...
	{
		template<class T>
		friend const ClassInfo& GetClassInfo() noexcept;

		template<class T>
		friend constexpr IID RTTI::GetInterfaceID() noexcept;

		protected:
			using TBaseClass = typename DynamicImplementation<TDerived, TBase...>;

		private:
			static constexpr IID ms_IID;
			static inline RTTI::DynamicImplementationClassInfo<TDerived, TBase...> ms_ClassInfo;

		protected:
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override
			{
				static_assert((std::is_base_of_v<IObject, TBase> && ...), "[...] must inherit from 'IObject'");
				RTTI::DebugPrint("Enter: " __FUNCSIG__);

				if (iid.IsOfType<RTTI::ClassInfo>())
				{
					RTTI::DebugPrint("Requested RTTI::ClassInfo -> success");
					RTTI::DebugPrint("Leave: " __FUNCSIG__);

					return static_cast<ClassInfo&>(ms_ClassInfo);
				}
				else if (iid.IsOfType<TDerived>())
				{
					RTTI::DebugPrint("Requested TDerived -> success");
					RTTI::DebugPrint("Leave: " __FUNCSIG__);

					return static_cast<TDerived&>(*this);
				}
				else if (RTTI::QueryInfo query; ((query = TBase::DoQueryInterface(iid), !query.IsNull()) || ...))
				{
					RTTI::DebugPrint("Requested TBase -> success");
					RTTI::DebugPrint(query.GetTypeName());
					RTTI::DebugPrint("Leave: " __FUNCSIG__);

					return query;
				}

				RTTI::DebugPrint("Leave: " __FUNCSIG__);
				return nullptr;
			}

		public:
			DynamicImplementation() = default;
	};

	template<class TDerived, class... TBase>
	class PrivateStub: public TBase...
	{
		template<class T>
		friend const ClassInfo& GetClassInfo() noexcept;

		protected:
			using TBaseClass = typename PrivateStub<TDerived, TBase...>;

		private:
			static inline RTTI::PrivateStubClassInfo<TDerived, TBase...> ms_ClassInfo;

		protected:
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override
			{
				static_assert((std::is_base_of_v<IObject, TBase> && ...), "[...] must inherit from 'IObject'");
				RTTI::DebugPrint("Enter: " __FUNCSIG__);

				if (iid.IsOfType<RTTI::ClassInfo>())
				{
					RTTI::DebugPrint("Requested RTTI::ClassInfo -> success");
					RTTI::DebugPrint("Leave: " __FUNCSIG__);

					return static_cast<ClassInfo&>(ms_ClassInfo);
				}
				else if (RTTI::QueryInfo query; ((query = TBase::DoQueryInterface(iid), !query.IsNull()) || ...))
				{
					RTTI::DebugPrint("Requested TBase -> success");
					RTTI::DebugPrint(query.GetTypeName());
					RTTI::DebugPrint("Leave: " __FUNCSIG__);

					return query;
				}

				RTTI::DebugPrint("Leave: " __FUNCSIG__);
				return nullptr;
			}

		public:
			PrivateStub() = default;
	};
}

#define kxf_RTTI_QueryInterface_Extend(T, TBase)	\
\
private:	\
	static inline kxf::RTTI::InterfaceClassInfo<T, TBase> ms_ClassInfo; \
	\
public:	\
	kxf::RTTI::QueryInfo DoQueryInterface(const kxf::IID& iid) noexcept override	\
	{	\
		RTTI::DebugPrint("Enter: " __FUNCSIG__);	\
		\
		if (iid.IsOfType<kxf::RTTI::ClassInfo>())	\
		{	\
			RTTI::DebugPrint("Requested RTTI::ClassInfo -> success");	\
			RTTI::DebugPrint("Leave: " __FUNCSIG__);	\
			\
			return static_cast<kxf::RTTI::ClassInfo&>(ms_ClassInfo);	\
		}	\
		else if (auto query = TBase::DoQueryInterface(iid))	\
		{	\
			RTTI::DebugPrint("Requested TBase -> success");	\
			RTTI::DebugPrint(query.GetTypeName());	\
			RTTI::DebugPrint("Leave: " __FUNCSIG__);	\
			\
			return query;	\
		}	\
		\
		RTTI::DebugPrint("Leave: " __FUNCSIG__);	\
		return nullptr;	\
	}

#define kxf_RTTI_QueryInterface_Base(T)	kxf_RTTI_QueryInterface_Extend(T)
