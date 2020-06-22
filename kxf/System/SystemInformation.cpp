#include "stdafx.h"
#include "SystemInformation.h"
#include "NativeAPI.h"
#include "Registry.h"
#include "COM.h"
#include "Private/SystemInformationDefinesMapping.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/CallAtScopeExit.h"
#include <wx/settings.h>

#include <Windows.h>
#include <SDDL.h>

#include <d3d11.h>
#include <dxgi1_3.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <d2d1_2helper.h>

#include "UndefWindows.h"

namespace
{
	constexpr bool CheckKernerlVersion(const kxf::System::KernelVersion& kernelVersion) noexcept
	{
		// Major version should always be > 0, others are allowed to be zero.
		return kernelVersion.Major > 0 && kernelVersion.Minor >= 0 && kernelVersion.Build >= 0 && kernelVersion.ServicePackMajor >= 0 && kernelVersion.ServicePackMinor >= 0;
	}
	constexpr bool CheckVersionInfo(const kxf::System::VersionInfo& versionInfo) noexcept
	{
		using namespace kxf;

		return versionInfo.PlatformID != SystemPlatformID::Unknown &&
			versionInfo.SystemType != SystemType::Unknown &&
			versionInfo.ProductType != SystemProductType::Unknown &&
			versionInfo.ProductSuite != SystemProductSuite::None;
	}
}

namespace kxf::System
{
	bool Is64Bit() noexcept
	{
		#if defined(_WIN64)
		return true;
		#else
		BOOL isWow64 = FALSE;
		if (NativeAPI::Kernel32::IsWow64Process)
		{
			NativeAPI::Kernel32::IsWow64Process(::GetCurrentProcess(), &isWow64);
		}
		return isWow64 != FALSE;
		#endif
	}
	void GetRegistryQuota(BinarySize& used, BinarySize& allowed) noexcept
	{
		DWORD usedBytes = 0;
		DWORD allowedBytes = 0;
		if (::GetSystemRegistryQuota(&allowedBytes, &usedBytes))
		{
			used = BinarySize::FromBytes(usedBytes);
			allowed = BinarySize::FromBytes(allowedBytes);
		}
	}

	String GetProductName()
	{
		if (const auto versionInfo = GetVersionInfo())
		{
			return GetProductName(*versionInfo, Is64Bit());
		}
		return {};
	}
	String GetProductName(const VersionInfo& versionInfo, bool is64Bit)
	{
		// For reference: https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-osversioninfoexa
		const int versionMajor = versionInfo.Kernel.Major;
		const int versionMinor = versionInfo.Kernel.Minor;
		const int buildNumber = versionInfo.Kernel.Build;
		const bool isHomeServer = versionInfo.ProductSuite & SystemProductSuite::HomeServer;
		const bool isWorkstation = versionInfo.SystemType == SystemType::Workstation;

		switch (versionMajor)
		{
			case 10:
			{
				switch (versionMinor)
				{
					case 0:
					{
						if (isWorkstation)
						{
							return "Windows 10";
						}
						else
						{
							// https://techcommunity.microsoft.com/t5/windows-server-insiders/windows-server-2019-version-info/m-p/234472
							if (buildNumber >= 17623)
							{
								return "Windows Server 2019";
							}
							else
							{
								return "Windows Server 2016";
							}
						}
					}
				};
				break;
			}
			case 6:
			{
				switch (versionMinor)
				{
					case 0:
					{
						if (isWorkstation)
						{
							return "Windows Vista";
						}
						else
						{
							return "Windows Server 2008";
						}
					}
					case 1:
					{
						if (isWorkstation)
						{
							return "Windows 7";
						}
						else
						{
							return "Windows Server 2008 R2";
						}
					}
					case 2:
					{
						if (isWorkstation)
						{
							return "Windows 8";
						}
						else
						{
							return "Windows Server 2012";
						}
					}
					case 3:
					{
						if (isWorkstation)
						{
							return "Windows 8.1";
						}
						else
						{
							return "Windows Server 2012 R2";
						}
					}
				};
				break;
			}
			case 5:
			{
				switch (versionMinor)
				{
					case 0:
					{
						return "Windows 2000";
					}
					case 1:
					{
						return "Windows XP";
					}
					case 2:
					{
						if (isWorkstation)
						{
							if (is64Bit)
							{
								return "Windows XP Professional x64 Edition";
							}
							else
							{
								// This branch shouldn't really be executes since there are no regular Windows XP with 6.2 kernel version
								return "Windows XP";
							}
						}
						else if (isHomeServer)
						{
							return "Windows Home Server";
						}
						else
						{
							return "Windows Server 2003";
						}
					}
				};
				break;
			}
		};
		return {};
	}

	std::optional<KernelVersion> GetKernelVersion() noexcept
	{
		static std::optional<KernelVersion> kernelCached;
		if (!kernelCached && NativeAPI::NtDLL::RtlGetVersion)
		{
			RTL_OSVERSIONINFOEXW infoEx = {};
			infoEx.dwOSVersionInfoSize = sizeof(infoEx);
			NativeAPI::NtDLL::RtlGetVersion(&infoEx);

			KernelVersion kernel(infoEx.dwMajorVersion, infoEx.dwMinorVersion, infoEx.dwBuildNumber);
			kernel.ServicePackMajor = infoEx.wServicePackMajor;
			kernel.ServicePackMinor = infoEx.wServicePackMinor;

			if (CheckKernerlVersion(kernel))
			{
				kernelCached = std::move(kernel);
			}
		}
		return kernelCached;
	}
	std::optional<VersionInfo> GetVersionInfo() noexcept
	{
		static std::optional<VersionInfo> versionInfoCached;
		if (!versionInfoCached && NativeAPI::NtDLL::RtlGetVersion)
		{
			RTL_OSVERSIONINFOEXW infoEx = {};
			infoEx.dwOSVersionInfoSize = sizeof(infoEx);
			NativeAPI::NtDLL::RtlGetVersion(&infoEx);

			VersionInfo versionInfo(infoEx.dwMajorVersion, infoEx.dwMinorVersion, infoEx.dwBuildNumber);
			versionInfo.Kernel.ServicePackMajor = infoEx.wServicePackMajor;
			versionInfo.Kernel.ServicePackMinor = infoEx.wServicePackMinor;

			if (CheckKernerlVersion(versionInfo.Kernel))
			{
				versionInfo.PlatformID = Private::MapSystemPlatformID(infoEx.dwPlatformId);
				versionInfo.SystemType = Private::MapSystemType(infoEx.wProductType);
				versionInfo.ProductSuite = Private::MapSystemProductSuite(infoEx.wSuiteMask);
				try
				{
					versionInfo.ServicePack = infoEx.szCSDVersion;
				}
				catch (...)
				{
					return {};
				}

				DWORD productType = 0;
				if (::GetProductInfo(infoEx.dwMajorVersion, infoEx.dwMinorVersion, infoEx.wServicePackMajor, infoEx.wServicePackMinor, &productType))
				{
					versionInfo.ProductType = Private::MapSystemProductType(productType);
				}

				if (CheckVersionInfo(versionInfo))
				{
					versionInfoCached = std::move(versionInfo);
				}
			}
		}
		return versionInfoCached;
	}
	std::optional<MemoryStatus> GetGlobalMemoryStatus() noexcept
	{
		MEMORYSTATUSEX info = {};
		info.dwLength = sizeof(MEMORYSTATUSEX);
		if (::GlobalMemoryStatusEx(&info))
		{
			MemoryStatus memoryStatus;
			memoryStatus.TotalRAM = BinarySize::FromBytes(info.ullTotalPhys);
			memoryStatus.TotalVirtual = BinarySize::FromBytes(info.ullTotalVirtual);
			memoryStatus.TotalPageFile = BinarySize::FromBytes(info.ullTotalPageFile);
			memoryStatus.AvailableRAM = BinarySize::FromBytes(info.ullAvailPhys);
			memoryStatus.AvailableVirtual = BinarySize::FromBytes(info.ullAvailVirtual);
			memoryStatus.AvailablePageFile = BinarySize::FromBytes(info.ullAvailPageFile);
			memoryStatus.MemoryLoad = info.dwMemoryLoad / 100.0f;

			return memoryStatus;
		}
		return {};
	}
	BinarySize GetPhysicallyInstalledMemory() noexcept
	{
		ULONGLONG totalMemoryInKilobytes = 0;
		if (::GetPhysicallyInstalledSystemMemory(&totalMemoryInKilobytes))
		{
			return BinarySize::FromKB(totalMemoryInKilobytes);
		}
		return {};
	}
	
	std::optional<UserInfo> GetUserInfo()
	{
		UserInfo userInfo;

		// User name
		String userName;
		DWORD userNameLength = 0;
		::GetUserNameW(nullptr, &userNameLength);
		if (!::GetUserNameW(wxStringBuffer(userName, userNameLength), &userNameLength))
		{
			return {};
		}
		userInfo.Name = std::move(userName);

		// Organization
		RegistryKey key(RegistryBaseKey::LocalMachine, wxS("Software\\Microsoft\\Windows NT\\CurrentVersion"), RegistryAccess::Read);
		if (key)
		{
			if (auto value = key.GetStringValue(wxS("RegisteredOrganization")))
			{
				userInfo.Organization = std::move(*value);
			}
		}

		// Is administrator
		HANDLE tokenHandle = nullptr;
		Utility::CallAtScopeExit atExit([&]()
		{
			if (tokenHandle)
			{
				::CloseHandle(tokenHandle);
			}
		});

		if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &tokenHandle))
		{
			TOKEN_ELEVATION elevationInfo;
			DWORD size = sizeof(TOKEN_ELEVATION);
			if (::GetTokenInformation(tokenHandle, TokenElevation, &elevationInfo, sizeof(elevationInfo), &size))
			{
				userInfo.AdminRights = elevationInfo.TokenIsElevated;
			}

			TOKEN_ELEVATION_TYPE elevationType;
			size = sizeof(TOKEN_ELEVATION_TYPE);
			if (::GetTokenInformation(tokenHandle, TokenElevationType, &elevationType, sizeof(elevationType), &size))
			{
				userInfo.LimitedAdminRights = elevationType == TOKEN_ELEVATION_TYPE::TokenElevationTypeLimited;
			}
		}
		return userInfo;
	}
	String GetUserSID()
	{
		HANDLE tokenHandle = nullptr;
		wchar_t* userSID = nullptr;
		Utility::CallAtScopeExit atExit([&]()
		{
			if (tokenHandle)
			{
				::CloseHandle(tokenHandle);
			}
			if (userSID)
			{
				::LocalFree(userSID);
			}
		});

		if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &tokenHandle))
		{
			DWORD size = 0;
			::GetTokenInformation(tokenHandle, TokenUser, nullptr, 0, &size);

			std::vector<uint8_t> userToken(size, 0);
			if (::GetTokenInformation(tokenHandle, TokenUser, userToken.data(), size, &size))
			{
				::ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(userToken.data())->User.Sid, &userSID);
				return userSID;
			}
		}
		return {};
	}

	Color GetColor(SystemColor index) noexcept
	{
		return wxSystemSettings::GetColour(static_cast<wxSystemColour>(index));
	}
	SystemScreenType GetScreenType() noexcept
	{
		return static_cast<SystemScreenType>(wxSystemSettings::GetScreenType());
	}
	wxFont GetFont(SystemFont index)
	{
		return wxSystemSettings::GetFont(static_cast<wxSystemFont>(index));
	}
	int GetMetric(SystemMetric index, const wxWindow* window) noexcept
	{
		return wxSystemSettings::GetMetric(static_cast<wxSystemMetric>(index), window);
	}
	Size GetMetric(SystemSizeMetric index, const wxWindow* window) noexcept
	{
		auto GetValue = [&](wxSystemMetric x, wxSystemMetric y)
		{
			return Size(wxSystemSettings::GetMetric(x, window), wxSystemSettings::GetMetric(y, window));
		};

		switch (index)
		{
			case SystemSizeMetric::Border:
			{
				return GetValue(wxSYS_BORDER_X, wxSYS_BORDER_Y);
			}
			case SystemSizeMetric::Cursor:
			{
				return GetValue(wxSYS_CURSOR_X, wxSYS_CURSOR_Y);
			}
			case SystemSizeMetric::DragThreshold:
			{
				return GetValue(wxSYS_DRAG_X, wxSYS_DRAG_Y);
			}
			case SystemSizeMetric::DClickThreshold:
			{
				return GetValue(wxSYS_DCLICK_X, wxSYS_DCLICK_Y);
			}
			case SystemSizeMetric::Edge:
			{
				return GetValue(wxSYS_EDGE_X, wxSYS_EDGE_Y);
			}
			case SystemSizeMetric::ScrollBar:
			{
				return GetValue(wxSYS_VSCROLL_X, wxSYS_HSCROLL_Y);
			}
			case SystemSizeMetric::ScrollThumb:
			{
				return GetValue(wxSYS_HTHUMB_X, wxSYS_VTHUMB_Y);
			}
			case SystemSizeMetric::ScrollArrowVertical:
			{
				return GetValue(wxSYS_VSCROLL_ARROW_X, wxSYS_VSCROLL_ARROW_Y);
			}
			case SystemSizeMetric::ScrollArrowHorizontal:
			{
				return GetValue(wxSYS_HSCROLL_ARROW_X, wxSYS_HSCROLL_ARROW_Y);
			}
			case SystemSizeMetric::Icon:
			{
				return GetValue(wxSYS_ICON_X, wxSYS_ICON_Y);
			}
			case SystemSizeMetric::IconSmall:
			{
				return GetValue(wxSYS_SMALLICON_X, wxSYS_SMALLICON_Y);
			}
			case SystemSizeMetric::IconSpacing:
			{
				return GetValue(wxSYS_ICONSPACING_X, wxSYS_ICONSPACING_Y);
			}
			case SystemSizeMetric::WindowMin:
			{
				return GetValue(wxSYS_WINDOWMIN_X, wxSYS_WINDOWMIN_Y);
			}
			case SystemSizeMetric::ThickFrame:
			{
				return GetValue(wxSYS_FRAMESIZE_X, wxSYS_FRAMESIZE_Y);
			}
		};
		return Size::UnspecifiedSize();
	}
	TimeSpan GetMetric(SystemTimeMetric index, const wxWindow* window) noexcept
	{
		return TimeSpan::Milliseconds(wxSystemSettings::GetMetric(static_cast<wxSystemMetric>(index), window));
	}
	bool HasFeature(SystemFeature feature) noexcept
	{
		return wxSystemSettings::HasFeature(static_cast<wxSystemFeature>(feature));
	}
	size_t EnumStandardSounds(std::function<bool(String)> func)
	{
		RegistryKey key(RegistryBaseKey::CurrentUser, wxS("AppEvents\\EventLabels"), RegistryAccess::Read|RegistryAccess::Enumerate);
		if (key)
		{
			return key.EnumKeyNames(std::move(func));
		}
		return 0;
	}

	std::optional<DisplayInfo> GetDisplayInfo() noexcept
	{
		HDC desktopDC = ::GetDC(nullptr);
		if (desktopDC)
		{
			Utility::CallAtScopeExit atExit([&]()
			{
				::ReleaseDC(nullptr, desktopDC);
			});

			DisplayInfo displayInfo;
			displayInfo.Width = ::GetDeviceCaps(desktopDC, DESKTOPHORZRES);
			displayInfo.Height = ::GetDeviceCaps(desktopDC, DESKTOPVERTRES);
			displayInfo.BitDepth = ::GetDeviceCaps(desktopDC, BITSPIXEL);
			displayInfo.RefreshRate = ::GetDeviceCaps(desktopDC, VREFRESH);
			return displayInfo;
		}
		return {};
	}
	size_t EnumDisplayModes(std::function<bool(DisplayInfo)> func, const String& deviceName)
	{
		size_t count = 0;

		DEVMODEW deviceMode = {};
		deviceMode.dmSize = sizeof(deviceMode);
		while (::EnumDisplaySettingsW(deviceName.IsEmpty() ? nullptr : deviceName.wc_str(), count, &deviceMode))
		{
			DisplayInfo displayInfo;
			displayInfo.Width = deviceMode.dmPelsWidth;
			displayInfo.Height = deviceMode.dmPelsHeight;
			displayInfo.BitDepth = deviceMode.dmBitsPerPel;
			displayInfo.RefreshRate = deviceMode.dmDisplayFrequency;

			count++;
			if (!std::invoke(func, std::move(displayInfo)))
			{
				break;
			}
		}
		return count;
	}
	size_t EnumDisplayDevices(std::function<bool(DisplayDeviceInfo)> func)
	{
		DWORD index = 0;
		size_t count = 0;
		bool isSuccess = false;

		do
		{
			DISPLAY_DEVICEW displayDevice = {};
			displayDevice.cb = sizeof(displayDevice);
			isSuccess = ::EnumDisplayDevicesW(nullptr, index, &displayDevice, 0);

			if (!std::wstring_view(displayDevice.DeviceString).empty())
			{
				DisplayDeviceInfo deviceInfo;
				deviceInfo.DeviceName = displayDevice.DeviceName;
				deviceInfo.DeviceDescription = displayDevice.DeviceString;

				deviceInfo.Flags.Add(DisplayDeviceFlag::Active, displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP);
				deviceInfo.Flags.Add(DisplayDeviceFlag::Primary, displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE);
				deviceInfo.Flags.Add(DisplayDeviceFlag::Removable, displayDevice.StateFlags & DISPLAY_DEVICE_REMOVABLE);
				deviceInfo.Flags.Add(DisplayDeviceFlag::RDPUnifiedDriver, displayDevice.StateFlags & DISPLAY_DEVICE_RDPUDD);
				deviceInfo.Flags.Add(DisplayDeviceFlag::MirroringDriver, displayDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER);
				deviceInfo.Flags.Add(DisplayDeviceFlag::MultiDriver, displayDevice.StateFlags & DISPLAY_DEVICE_MULTI_DRIVER);
				deviceInfo.Flags.Add(DisplayDeviceFlag::AccDriver, displayDevice.StateFlags & DISPLAY_DEVICE_ACC_DRIVER);
				deviceInfo.Flags.Add(DisplayDeviceFlag::VGACompatible, displayDevice.StateFlags & DISPLAY_DEVICE_VGA_COMPATIBLE);
				deviceInfo.Flags.Add(DisplayDeviceFlag::TSCompatible, displayDevice.StateFlags & DISPLAY_DEVICE_TS_COMPATIBLE);
				deviceInfo.Flags.Add(DisplayDeviceFlag::ModesPruned, displayDevice.StateFlags & DISPLAY_DEVICE_MODESPRUNED);
				deviceInfo.Flags.Add(DisplayDeviceFlag::UnsafeModesEnabled, displayDevice.StateFlags & DISPLAY_DEVICE_UNSAFE_MODES_ON);
				deviceInfo.Flags.Add(DisplayDeviceFlag::Disconnect, displayDevice.StateFlags & DISPLAY_DEVICE_DISCONNECT);
				deviceInfo.Flags.Add(DisplayDeviceFlag::Remote, displayDevice.StateFlags & DISPLAY_DEVICE_REMOTE);

				count++;
				if (!std::invoke(func, std::move(deviceInfo)))
				{
					break;
				}
			}
			index++;
		}
		while (isSuccess);

		return count;
	}
	size_t EnumDisplayAdapters(std::function<bool(DisplayAdapterInfo)> func)
	{
		HResult hr = HResult::Fail();

		// Create DXGI factory
		COMPtr<IDXGIFactory2> dxgiFactory;
		if (NativeAPI::DXGI::CreateDXGIFactory2)
		{
			hr = NativeAPI::DXGI::CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(IDXGIFactory2), dxgiFactory.GetAddress());
		}
		else
		{
			hr = ::CreateDXGIFactory1(__uuidof(IDXGIFactory2), dxgiFactory.GetAddress());
		}

		if (dxgiFactory && hr)
		{
			UINT index = 0;
			COMPtr<IDXGIAdapter1> adapter;

			size_t count = 0;
			while (dxgiFactory->EnumAdapters1(index, &adapter) != DXGI_ERROR_NOT_FOUND)
			{
				DXGI_ADAPTER_DESC1 description = {};
				if (HResult(adapter->GetDesc1(&description)))
				{
					DisplayAdapterInfo info = {};
					info.Name = description.Description;
					info.Index = index;
					info.VendorID = description.VendorId;
					info.DeviceID = description.DeviceId;
					info.SubSystemID = description.SubSysId;
					info.Revision = description.Revision;
					info.UniqueID = Utility::IntFromLowHigh<uint64_t>(description.AdapterLuid.LowPart, description.AdapterLuid.HighPart);
					info.DedicatedVideoMemory = BinarySize::FromBytes(description.DedicatedVideoMemory);
					info.DedicatedSystemMemory = BinarySize::FromBytes(description.DedicatedSystemMemory);
					info.SharedSystemMemory = BinarySize::FromBytes(description.SharedSystemMemory);
					info.Flags.Add(DisplayAdapterFlag::Software, description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE);

					count++;
					if (!std::invoke(func, std::move(info)))
					{
						break;
					}
				}
				index++;
			}
		}
		return 0;
	}

	String ExpandEnvironmentStrings(const String& strings)
	{
		DWORD length = ::ExpandEnvironmentStringsW(strings.wc_str(), nullptr, 0);
		if (length != 0)
		{
			String result;
			if (::ExpandEnvironmentStringsW(strings.wc_str(), wxStringBuffer(result, length - 1), length) != 0)
			{
				return result;
			}
		}
		return {};
	}
	String GetEnvironmentVariable(const String& name)
	{
		DWORD length = ::GetEnvironmentVariableW(name.wc_str(), nullptr, 0);
		if (length != 0)
		{
			String result;
			::GetEnvironmentVariableW(name.wc_str(), wxStringBuffer(result, length), length);
			return result;
		}
		return {};
	}
	bool SetEnvironmentVariable(const String& name, const String& value)
	{
		return ::SetEnvironmentVariableW(name.wc_str(), value.wc_str());
	}
	size_t EnumEnvironmentVariables(std::function<bool(String, String)> func)
	{
		size_t count = 0;
		for (const wchar_t* item = ::GetEnvironmentStringsW(); *item; item += std::wcslen(item) + 1)
		{
			const wchar_t* separator = std::wcschr(item, L'=');
			if (separator && separator != item)
			{
				StringView name(item, separator - item);
				StringView value(separator + 1);

				count++;
				if (!std::invoke(func, String::FromView(name), String::FromView(value)))
				{
					break;
				}
			}
		}
		return count;
	}

	bool LockWorkstation(LockWorkstationCommand command) noexcept
	{
		switch (command)
		{
			case LockWorkstationCommand::LockScreen:
			{
				return ::LockWorkStation();
			}
			case LockWorkstationCommand::ScreenOff:
			{
				return !::SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
			}
			case LockWorkstationCommand::ScreenOn:
			{
				return !::SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)-1);
			}
			case LockWorkstationCommand::ScreenSleep:
			{
				return !::SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)1);
			}
			case LockWorkstationCommand::ScreenWait:
			{
				return !::SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)0);
			}
			case LockWorkstationCommand::ScreenSaver:
			{
				return !::SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_SCREENSAVE, 0);
			}
		};
		return false;
	}
	bool ExitWorkstation(FlagSet<ExitWorkstationCommand> command) noexcept
	{
		UINT modeEWX = 0;
		Utility::AddFlagRef(modeEWX, EWX_LOGOFF, command & ExitWorkstationCommand::LogOff);
		Utility::AddFlagRef(modeEWX, EWX_POWEROFF, command & ExitWorkstationCommand::PowerOff);
		Utility::AddFlagRef(modeEWX, EWX_SHUTDOWN, command & ExitWorkstationCommand::Shutdown);
		Utility::AddFlagRef(modeEWX, EWX_REBOOT, command & ExitWorkstationCommand::Reboot);
		Utility::AddFlagRef(modeEWX, EWX_QUICKRESOLVE, command & ExitWorkstationCommand::QuickResolve);
		Utility::AddFlagRef(modeEWX, EWX_RESTARTAPPS, command & ExitWorkstationCommand::RestartApps);
		Utility::AddFlagRef(modeEWX, EWX_HYBRID_SHUTDOWN, command & ExitWorkstationCommand::HybridShutdown);
		Utility::AddFlagRef(modeEWX, EWX_BOOTOPTIONS, command & ExitWorkstationCommand::BootOptions);
		Utility::AddFlagRef(modeEWX, EWX_FORCE, command & ExitWorkstationCommand::Force);
		Utility::AddFlagRef(modeEWX, EWX_FORCEIFHUNG, command & ExitWorkstationCommand::ForceHung);

		return ::ExitWindowsEx(modeEWX, SHTDN_REASON_MAJOR_OTHER|SHTDN_REASON_MINOR_OTHER|SHTDN_REASON_FLAG_PLANNED);
	}

	bool IsWindowsServer() noexcept
	{
		auto version = GetVersionInfo();
		return version && version->SystemType == SystemType::Server;
	}
	bool IsWindowsVersionOrGreater(int majorVersion, int minorVersion, int servicePackMajor) noexcept
	{
		if (auto version = GetKernelVersion())
		{
			auto CheckServicePack = [&]()
			{
				if (servicePackMajor != -1)
				{
					return version->ServicePackMajor >= servicePackMajor;
				}
				return true;
			};

			if (version->Major > majorVersion)
			{
				// No sense in checking service pack if it's completely different version
				return true;
			}
			else if (version->Major == majorVersion)
			{
				if (version->Minor >= minorVersion)
				{
					return CheckServicePack();
				}
			}
		}
		return false;
	}
}