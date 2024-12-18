#pragma once
#include "kxf/Common.hpp"
#include <typeinfo>
#include <type_traits>
#include <string_view>
#include <functional>

namespace kxf
{
	class IID;
	class IObject;
}
namespace kxf::RTTI
{
	class ClassInfo;
}

namespace kxf::RTTI
{
	template<class T>
	constexpr IID GetInterfaceID() noexcept
	{
		return T::ms_IID;
	}

	template<class T>
	const RTTI::ClassInfo& GetClassInfo() noexcept
	{
		return T::ms_ClassInfo;
	}

	KXF_API Enumerator<const ClassInfo&> EnumClassInfo() noexcept;

	KXF_API const ClassInfo* GetClassInfoByInterfaceID(const IID& iid) noexcept;
	KXF_API const ClassInfo* GetClassInfoByName(const char* fullyQualifiedName) noexcept;
	KXF_API const ClassInfo* GetClassInfoByName(std::string_view fullyQualifiedName) noexcept;
	KXF_API const ClassInfo* GetClassInfoByName(const kxf::String& fullyQualifiedName) noexcept;
}
