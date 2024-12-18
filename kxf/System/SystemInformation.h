#pragma once
#include "Common.h"
#include "SystemInformationDefines.h"
#include "UserName.h"
#include "kxf/Core/String.h"
#include "kxf/Core/Version.h"
#include "kxf/Core/DataSize.h"
#include "kxf/Core/LocallyUniqueID.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/DateTime/TimeSpan.h"
#include "kxf/Drawing/Geometry.h"
#include "kxf/Drawing/Color.h"
#include "kxf/Drawing/Font.h"

namespace kxf::System
{
	struct KernelVersion final
	{
		public:
			int Major = -1;
			int Minor = -1;
			int Build = -1;

			int ServicePackMajor = -1;
			int ServicePackMinor = -1;

		public:
			KernelVersion() noexcept = default;
			KernelVersion(int major, int minor = 0, int build = 0) noexcept
				:Major(major), Minor(minor), Build(build)
			{
			}

		public:
			operator Version() const noexcept
			{
				return {Major, Minor, Build, ServicePackMajor, ServicePackMinor};
			}
	};
	struct VersionInfo final
	{
		public:
			KernelVersion Kernel;

			String ServicePack;
			SystemType SystemType = SystemType::Unknown;
			SystemPlatformID PlatformID = SystemPlatformID::Unknown;
			SystemProductType ProductType = SystemProductType::Unknown;
			FlagSet<SystemProductSuite> ProductSuite;
			bool Is64Bit = false;

		public:
			VersionInfo() noexcept = default;
			VersionInfo(int major, int minor = 0, int build = 0) noexcept
				:Kernel(major, minor, build)
			{
			}
	};

	struct MemoryStatus final
	{
		DataSize TotalRAM;
		DataSize TotalVirtual;
		DataSize TotalPageFile;
		DataSize AvailableRAM;
		DataSize AvailableVirtual;
		DataSize AvailablePageFile;
		float MemoryLoad = 0.0f;
	};
	struct UserInfo final
	{
		UserName Name;
		String Organization;
		bool AdminRights = false;
		bool LimitedAdminRights = false;
	};
	struct EnvironmentVariable final
	{
		String Name;
		String Value;
	};

	struct DisplayInfo final
	{
		int Width = 0;
		int Height = 0;
		int BitDepth = 0;
		int RefreshRate = 0;
	};
	struct DisplayDeviceInfo final
	{
		String DeviceName;
		String DeviceDescription;
		FlagSet<DisplayDeviceFlag> Flags;
	};
	struct DisplayAdapterInfo final
	{
		String Name;
		uint32_t Index = 0;
		uint32_t VendorID = 0;
		uint32_t DeviceID = 0;
		uint32_t SubSystemID = 0;
		uint32_t Revision = 0;
		LocallyUniqueID UniqueID;
		FlagSet<DisplayAdapterFlag> Flags;
		DataSize DedicatedVideoMemory;
		DataSize DedicatedSystemMemory;
		DataSize SharedSystemMemory;
	};
}

namespace kxf::System
{
	KXF_API bool Is64Bit() noexcept;
	KXF_API void GetRegistryQuota(DataSize& used, DataSize& allowed) noexcept;

	KXF_API String GetProductName();
	KXF_API String GetProductName(const VersionInfo& versionInfo);

	KXF_API std::optional<KernelVersion> GetKernelVersion() noexcept;
	KXF_API std::optional<VersionInfo> GetVersionInfo() noexcept;
	KXF_API std::optional<MemoryStatus> GetGlobalMemoryStatus() noexcept;
	KXF_API DataSize GetPhysicallyInstalledMemory() noexcept;

	KXF_API std::optional<UserInfo> GetUserInfo();
	KXF_API String GetUserSID();

	KXF_API Color GetColor(SystemColor index) noexcept;
	KXF_API Font GetFont(SystemFont index);
	KXF_API SystemScreenType GetScreenType() noexcept;
	KXF_API int GetMetric(SystemMetric index, const wxWindow* window = nullptr) noexcept;
	KXF_API Size GetMetric(SystemSizeMetric index, const wxWindow* window = nullptr) noexcept;
	KXF_API TimeSpan GetMetric(SystemTimeMetric index, const wxWindow* window = nullptr) noexcept;
	KXF_API bool HasFeature(SystemFeature feature) noexcept;
	KXF_API Enumerator<String> EnumStandardSounds();

	KXF_API std::optional<DisplayInfo> GetDisplayInfo() noexcept;
	KXF_API Enumerator<DisplayInfo> EnumDisplayModes(const String& deviceName = {});
	KXF_API Enumerator<DisplayDeviceInfo> EnumDisplayDevices(const String& deviceName = {});
	KXF_API Enumerator<DisplayAdapterInfo> EnumDisplayAdapters();

	KXF_API String ExpandEnvironmentStrings(const String& strings);
	KXF_API String GetEnvironmentVariable(const String& name);
	KXF_API bool SetEnvironmentVariable(const String& name, const String& value);
	KXF_API Enumerator<EnvironmentVariable> EnumEnvironmentVariables();

	KXF_API bool LockWorkstation(LockWorkstationCommand command) noexcept;
	KXF_API bool ExitWorkstation(FlagSet<ExitWorkstationCommand> command) noexcept;

	KXF_API bool IsWindowsServer() noexcept;
	KXF_API bool IsWindowsVersionOrGreater(int majorVersion, int minorVersion, int servicePackMajor = -1, int buildNumber = -1) noexcept;
	KXF_API bool IsWindowsVersionOrGreater(NamedSystemRelease namedRelease) noexcept;
};
