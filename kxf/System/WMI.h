#pragma once
#include "Common.h"
#include "COM.h"
#include "kxf/Core/Any.h"
#include "kxf/Core/String.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/Crypto/UserCredentials.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/Localization/Locale.h"
struct IWbemLocator;
struct IWbemServices;
struct IWbemClassObject;
struct IWbemQualifierSet;

namespace kxf
{
	class WMIClassObject;
	class WMIQualifierSet;

	enum class WMIClassObjectFlag: uint32_t
	{
		None = 0,

		LocalOnly = 1 << 0,
		PropagatedOnly = 1 << 1,
		SystemOnly = 1 << 2,
		NonSystemOnly = 1 << 3
	};
	kxf_FlagSet_Declare(WMIClassObjectFlag);

	enum class WMIClassObjectComparisonFlag: uint32_t
	{
		None = 0,

		IgnoreObjectSource = 1 << 0,
		IgnoreDefaultValues = 1 << 1,
		IgnoreQualifiers = 1 << 2,
		IgnoreFlavor = 1 << 3,
		IgnoreClass = 1 << 4,
		IgnoreCase = 1 << 5
	};
	kxf_FlagSet_Declare(WMIClassObjectComparisonFlag);
}

namespace kxf
{
	class KXF_API WMINamespace final
	{
		private:
			COMPtr<IWbemLocator> m_Locator;
			COMPtr<IWbemServices> m_Service;

		private:
			bool Initialize(const kxf::FSPath& wmiNamespace, const UserCredentials* credentials = nullptr, const Locale& locale = {});

		public:
			WMINamespace() noexcept;
			WMINamespace(const FSPath& wmiNamespace, const Locale& locale = {});
			WMINamespace(const FSPath& wmiNamespace, const UserCredentials& credentials, const Locale& locale = {});
			WMINamespace(const WMINamespace&) noexcept;
			WMINamespace(WMINamespace&&) noexcept;
			~WMINamespace();

		public:
			bool IsNull() const noexcept;

			Enumerator<String> EnumClassNames() const;
			Enumerator<String> EnumChildNamespaces() const;

			Enumerator<WMIClassObject> ExecuteQuery(const kxf::String& query);
			Enumerator<WMIClassObject> SelectAll(const kxf::String& fromLocation);

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			WMINamespace& operator=(WMINamespace&& other) noexcept;
			WMINamespace& operator=(const WMINamespace&) noexcept;
	};
}

namespace kxf
{
	class KXF_API WMIClassObject final
	{
		private:
			COMPtr<IWbemClassObject> m_ClassObject;

		public:
			WMIClassObject() noexcept;
			WMIClassObject(COMPtr<IWbemClassObject> classObject) noexcept;
			WMIClassObject(const WMIClassObject&) noexcept;
			WMIClassObject(WMIClassObject&&) noexcept;
			~WMIClassObject();

		public:
			bool IsNull() const noexcept;
			bool IsSameAs(const WMIClassObject& other, FlagSet<WMIClassObjectComparisonFlag> flags = {}) const noexcept;

			String GetName() const;
			String GetClassName() const;

			Enumerator<String> EnumPropertyNames(FlagSet<WMIClassObjectFlag> flags = {}) const;
			Any GetProperty(const kxf::String& name) const;

			WMIQualifierSet GetQualifierSet() const;
			WMIQualifierSet GetMethodQualifierSet(const kxf::String& name) const;
			WMIQualifierSet GetPropertyQualifierSet(const kxf::String& name) const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			bool operator==(const WMIClassObject& other) const noexcept
			{
				return IsSameAs(other);
			}
			bool operator!=(const WMIClassObject& other) const noexcept
			{
				return !IsSameAs(other);
			}

			WMIClassObject& operator=(WMIClassObject&& other) noexcept;
			WMIClassObject& operator=(const WMIClassObject&) noexcept;
	};
}
namespace kxf
{
	class KXF_API WMIQualifierSet final
	{
		private:
			COMPtr<IWbemQualifierSet> m_QualifierSet;

		public:
			WMIQualifierSet() noexcept;
			WMIQualifierSet(COMPtr<IWbemQualifierSet> qualifierSet) noexcept;
			WMIQualifierSet(const WMIQualifierSet&) noexcept;
			WMIQualifierSet(WMIQualifierSet&&) noexcept;
			~WMIQualifierSet();

		public:
			bool IsNull() const noexcept;

			Enumerator<std::pair<String, Any>> EnumQualifiers() const;
			Enumerator<String> EnumNames(FlagSet<WMIClassObjectFlag> flags = {}) const;
			Any GetValue(const kxf::String& name) const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			WMIQualifierSet& operator=(WMIQualifierSet&& other) noexcept;
			WMIQualifierSet& operator=(const WMIQualifierSet&) noexcept;
	};
}
