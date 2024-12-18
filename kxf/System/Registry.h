#pragma once
#include "Common.h"
#include "kxf/Core/String.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/System/Win32Error.h"

namespace kxf
{
	enum class RegistryRootKey
	{
		LocalMachine,
		Users,
		ClassesRoot,
		CurrentUser,
		CurrentUserLocalSettings,
		CurrentConfig,
		PerformanceData,
		PerformanceText,
		PerformanceTextNLS,
		DynData,
	};
	enum class RegistryAccess: uint32_t
	{
		None = 0,

		Read = 1 << 0,
		Write = 1 << 1,
		Create = 1 << 2,
		Delete = 1 << 3,
		Notify = 1 << 4,
		Enumerate = 1 << 5,

		Everything = Create|Notify|Enumerate|Read|Write
	};
	enum class RegistryKeyFlag: uint32_t
	{
		None = 0,
		Volatile = 1 << 0
	};
	enum class RegistryWOW64
	{
		Default = 0,
		Access32,
		Access64
	};
	enum class RegistryValueType
	{
		Any = -1,
		None = 0,

		String,
		StringArray,
		StringExpand,

		SymLink,
		Binary,

		UInt32,
		UInt32_LE = UInt32,
		UInt32_BE,

		UInt64,
		UInt64_LE = UInt64,
		UInt64_BE,

		ResourceList,
		ResourceRequirementsList,
		FullResourceDescriptor,
	};

	kxf_FlagSet_Declare(RegistryAccess);
	kxf_FlagSet_Declare(RegistryKeyFlag);
}

namespace kxf
{
	class KXF_API RegistryKey final
	{
		public:
			static RegistryKey CreateKey(RegistryRootKey baseKey, const FSPath& subKey, FlagSet<RegistryAccess> access, FlagSet<RegistryKeyFlag> flags = {}, RegistryWOW64 wow64 = RegistryWOW64::Default)
			{
				RegistryKey key(baseKey, {}, RegistryAccess::Create, wow64);
				if (key)
				{
					return key.CreateKey(subKey, access, flags, wow64);
				}
				return {};
			}

		private:
			void* m_Handle = nullptr;
			mutable Win32Error m_LastError = Win32Error::Success();

		private:
			void* DoGetBaseKey(RegistryRootKey baseKey) const noexcept;
			bool DoOpenKey(void* rootKey, const FSPath& subKey, FlagSet<RegistryAccess> access, RegistryWOW64 wow64);
			bool DoCreateKey(void* rootKey, const FSPath& subKey, FlagSet<RegistryAccess> access, FlagSet<RegistryKeyFlag> flags, RegistryWOW64 wow64);
			void DoCloseKey(void* handle) noexcept;

		public:
			RegistryKey() noexcept = default;
			RegistryKey(RegistryRootKey baseKey) noexcept
				:m_Handle(DoGetBaseKey(baseKey))
			{
			}
			RegistryKey(RegistryRootKey baseKey, const FSPath& subKey, FlagSet<RegistryAccess> access, RegistryWOW64 wow64 = RegistryWOW64::Default)
			{
				DoOpenKey(DoGetBaseKey(baseKey), subKey, access, wow64);
			}
			RegistryKey(const RegistryKey&) = delete;
			RegistryKey(RegistryKey&& other) noexcept
			{
				*this = std::move(other);
			}
			~RegistryKey() noexcept
			{
				DoCloseKey(m_Handle);
			}

		public:
			bool IsNull() const noexcept
			{
				return m_Handle == nullptr;
			}
			bool IsBaseKey() const noexcept;
			void Close() noexcept
			{
				DoCloseKey(m_Handle);
				m_Handle = nullptr;
			}
			Win32Error GetLastError() const noexcept
			{
				return m_LastError;
			}

			RegistryKey& AttachHandle(void* handle) noexcept
			{
				DoCloseKey(m_Handle);
				m_Handle = handle;

				return *this;
			}
			void* DetachHandle() noexcept
			{
				void* handle = m_Handle;
				m_Handle = nullptr;
				return handle;
			}

			RegistryKey OpenKey(const FSPath& subKey, FlagSet<RegistryAccess> access, RegistryWOW64 wow64 = RegistryWOW64::Default)
			{
				RegistryKey key;
				if (key.DoOpenKey(m_Handle, subKey, access, wow64))
				{
					return key;
				}
				return {};
			}
			RegistryKey CreateKey(const FSPath& subKey, FlagSet<RegistryAccess> access, FlagSet<RegistryKeyFlag> flags = {}, RegistryWOW64 wow64 = RegistryWOW64::Default)
			{
				RegistryKey key;
				if (key.DoCreateKey(m_Handle, subKey, access, flags, wow64))
				{
					return key;
				}
				return {};
			}
			bool RemoveKey(const FSPath& subKey, bool resursive = false);

			bool RemoveValue(const String& valueName);
			bool DoesValueExist(const String& valueName) const;
			RegistryValueType GetValueType(const String& valueName) const;

			Enumerator<String> EnumKeyNames() const;
			Enumerator<String> EnumValueNames() const;

			std::optional<String> GetStringValue(const String& valueName) const;
			bool SetStringValue(const String& valueName, const String& value);

			std::optional<String> GetStringExpandValue(const String& valueName, bool noAutoExpand = false) const;
			bool SetStringExpandValue(const String& valueName, const String& value);

			size_t GetStringArrayValue(const String& valueName, std::function<bool(String)> func) const;
			bool SetStringArrayValue(const String& valueName, std::function<String()> func);

			std::optional<std::vector<uint8_t>> GetBinaryValue(const String& valueName) const;
			bool SetBinaryValue(const String& valueName, const void* data, size_t size);

			std::optional<uint32_t> GetUInt32Value(const String& valueName) const;
			bool SetUInt32Value(const String& valueName, uint32_t value, bool asBigEndian = false);

			std::optional<uint64_t> GetUInt64Value(const String& valueName) const;
			bool SetUInt64Value(const String& valueName, uint64_t value, bool asBigEndian = false);

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			RegistryKey& operator=(RegistryKey&& other) noexcept
			{
				m_Handle = other.m_Handle;
				other.m_Handle = nullptr;

				return *this;
			}
			RegistryKey& operator=(const RegistryKey&) = delete;
	};
}
