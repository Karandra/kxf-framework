#include "kxf-pch.h"
#include "Common.h"
#include "ClassInfo.h"
#include "kxf/Core/String.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/Utility/Enumerator.h"

namespace
{
	template<class TFunc>
	const kxf::RTTI::ClassInfo* DoGetClassInfo(TFunc&& func) noexcept
	{
		using namespace kxf::RTTI;

		for (const ClassInfo& classInfo: EnumClassInfo())
		{
			if (std::invoke(func, classInfo))
			{
				return &classInfo;
			}
		};
		return nullptr;
	}
}

namespace kxf::RTTI
{
	Enumerator<const ClassInfo&> EnumClassInfo() noexcept
	{
		return [classInfo = ClassInfo::GetFirstClassInfo()]() mutable -> optional_ref<const ClassInfo>
		{
			if (classInfo)
			{
				decltype(auto) ref = *classInfo;
				classInfo = classInfo->GetNextClassInfo();

				return ref;
			}
			return {};
		};
	}

	const kxf::RTTI::ClassInfo* GetClassInfoByInterfaceID(const IID& iid) noexcept
	{
		if (iid)
		{
			return DoGetClassInfo([&](const ClassInfo& classInfo)
			{
				return classInfo.GetIID() == iid;
			});
		}
		return nullptr;
	}
	const ClassInfo* GetClassInfoByName(const char* fullyQualifiedName) noexcept
	{
		return GetClassInfoByName(std::string_view(fullyQualifiedName));
	}
	const ClassInfo* GetClassInfoByName(std::string_view fullyQualifiedName) noexcept
	{
		if (!fullyQualifiedName.empty())
		{
			return DoGetClassInfo([&](const ClassInfo& classInfo)
			{
				return classInfo.m_FullyQualifiedName == fullyQualifiedName;
			});
		}
		return nullptr;
	}
	const ClassInfo* GetClassInfoByName(const kxf::String& fullyQualifiedName) noexcept
	{
		if (!fullyQualifiedName.IsEmpty())
		{
			return DoGetClassInfo([&](const ClassInfo& classInfo)
			{
				return fullyQualifiedName.IsSameAs(classInfo.m_FullyQualifiedName);
			});
		}
		return nullptr;
	}
}
