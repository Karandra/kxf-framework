#include "kxf-pch.h"
#include "ShellOperations.h"
#include "SystemInformation.h"
#include "COM.h"
#include "Private/Shell.h"
#include "Private/KnownDirectoryGUID.h"
#include "Private/KnownDirectoryDefinition.h"
#include "kxf/FileSystem/FileItem.h"
#include "kxf/FileSystem/LegacyVolume.h"
#include "kxf/FileSystem/Private/NativeFSUtility.h"
#include "kxf/Core/UniversallyUniqueID.h"
#include "kxf/Core/Any.h"
#include "kxf/Network/URI.h"
#include "kxf/Drawing/GDIRenderer/GDIIcon.h"
#include "kxf/UI/Common.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/String.h"
#include "kxf/Utility/ScopeGuard.h"

#include <Windows.h>
#include <winnls.h>
#include <shobjidl.h>
#include <Shlobj.h>
#include <objbase.h>
#include <objidl.h>
#include <shlwapi.h>
#include <shlguid.h>
#include <Shldisp.h>
#include <KnownFolders.h>
#include "kxf/Win32/UndefMacros.h"

namespace
{
	using namespace kxf;

	constexpr FlagSet<UINT> MapSHGetFileIconFlag(FlagSet<SHGetFileIconFlag> flags) noexcept
	{
		FlagSet<UINT> nativeFlags = SHGFI_ICON;
		nativeFlags.Add(SHGFI_SMALLICON, flags & SHGetFileIconFlag::Small);
		nativeFlags.Add(SHGFI_LARGEICON, flags & SHGetFileIconFlag::Large);
		nativeFlags.Add(SHGFI_SHELLICONSIZE, flags & SHGetFileIconFlag::ShellSized);
		nativeFlags.Add(SHGFI_SELECTED, flags & SHGetFileIconFlag::Selected);
		nativeFlags.Add(SHGFI_OPENICON, flags & SHGetFileIconFlag::Open);
		nativeFlags.Add(SHGFI_ADDOVERLAYS, flags & SHGetFileIconFlag::AddOverlays);
		nativeFlags.Add(SHGFI_LINKOVERLAY, flags & SHGetFileIconFlag::LinkOverlay);

		return nativeFlags;
	}

	String DoQueryAssociation(const String& value, SHQueryAssociation option, Any* extraData)
	{
		auto Query = [&](ASSOCSTR option) -> String
		{
			constexpr ASSOCF flags = ASSOCF_INIT_DEFAULTTOSTAR|ASSOCF_NOTRUNCATE|ASSOCF_REMAPRUNDLL;

			DWORD length = 0;
			HResult hr = ::AssocQueryStringW(flags, option, value.wc_str(), nullptr, nullptr, &length);
			if (hr.IsFalse() && length != 0)
			{
				String result;
				if (hr = ::AssocQueryStringW(flags, option, value.wc_str(), nullptr, Utility::StringBuffer(result, length), &length))
				{
					return result;
				}
			}
			return {};
		};
		switch (option)
		{
			case SHQueryAssociation::Command:
			{
				return Query(ASSOCSTR::ASSOCSTR_COMMAND);
			}
			case SHQueryAssociation::Executable:
			{
				return Query(ASSOCSTR::ASSOCSTR_EXECUTABLE);
			}
			case SHQueryAssociation::FriendlyDocName:
			{
				return Query(ASSOCSTR::ASSOCSTR_FRIENDLYDOCNAME);
			}
			case SHQueryAssociation::FriendlyAppName:
			{
				return Query(ASSOCSTR::ASSOCSTR_FRIENDLYAPPNAME);
			}
			case SHQueryAssociation::InfoTip:
			{
				return Query(ASSOCSTR::ASSOCSTR_INFOTIP);
			}
			case SHQueryAssociation::QuickTip:
			{
				return Query(ASSOCSTR::ASSOCSTR_QUICKTIP);
			}
			case SHQueryAssociation::TileInfo:
			{
				return Query(ASSOCSTR::ASSOCSTR_TILEINFO);
			}
			case SHQueryAssociation::ContentType:
			{
				return Query(ASSOCSTR::ASSOCSTR_CONTENTTYPE);
			}
			case SHQueryAssociation::DefaultIcon:
			{
				const String icon = Query(ASSOCSTR::ASSOCSTR_DEFAULTICON);
				if (icon != "%1")
				{
					if (extraData)
					{
						if (auto index = icon.AfterLast(',').ToInteger<int>())
						{
							*extraData = std::abs(*index);
						}
					}

					String path = icon.BeforeLast(',');
					return !path.IsEmpty() ? path : icon;
				}
				break;
			}
			case SHQueryAssociation::ShellExtension:
			{
				return Query(ASSOCSTR::ASSOCSTR_SHELLEXTENSION);
			}
			case SHQueryAssociation::SupportedURIProtocols:
			{
				return Query(ASSOCSTR::ASSOCSTR_SUPPORTED_URI_PROTOCOLS);
			}
			case SHQueryAssociation::ProgID:
			{
				return Query(ASSOCSTR::ASSOCSTR_PROGID);
			}
			case SHQueryAssociation::AppID:
			{
				return Query(ASSOCSTR::ASSOCSTR_APPID);
			}
			case SHQueryAssociation::AppPublisher:
			{
				return Query(ASSOCSTR::ASSOCSTR_APPPUBLISHER);
			}
			case SHQueryAssociation::AppIconReference:
			{
				return Query(ASSOCSTR::ASSOCSTR_APPICONREFERENCE);
			}
		};
		return {};
	}
}

namespace kxf::Shell
{
	bool FileOperation(SHOperationType opType, const FSPath& source, const FSPath& destination, wxWindow* window, FlagSet<SHOperationFlags> flags)
	{
		// SHFileOperation doesn't work for paths longer than 'MAX_PATH'
		if (source.GetLength() >= MAX_PATH || destination.GetLength() >= MAX_PATH)
		{
			return false;
		}

		SHFILEOPSTRUCTW operationInfo = {};

		// Set operation
		switch (opType)
		{
			case SHOperationType::Copy:
			{
				operationInfo.wFunc = FO_COPY;
				break;
			}
			case SHOperationType::Move:
			{
				operationInfo.wFunc = FO_MOVE;
				break;
			}
			case SHOperationType::Rename:
			{
				operationInfo.wFunc = FO_RENAME;
				break;
			}
			case SHOperationType::Delete:
			{
				operationInfo.wFunc = FO_DELETE;
				break;
			}
			default:
			{
				return false;
			}
		};

		// Parent window
		window = window ? wxGetTopLevelParent(window) : nullptr;
		operationInfo.hwnd = window ? window->GetHandle() : nullptr;

		// Set flags
		FlagSet<decltype(operationInfo.fFlags)> operationInfoFlags = FOF_NOCONFIRMMKDIR;
		operationInfoFlags.Mod(FOF_NO_UI, !operationInfo.hwnd);
		operationInfoFlags.Mod(FOF_FILESONLY|FOF_NORECURSION, flags & SHOperationFlags::LimitToFiles);
		operationInfoFlags.Mod(FOF_ALLOWUNDO, flags & SHOperationFlags::AllowUndo);
		operationInfoFlags.Mod(FOF_NOCONFIRMATION, flags & SHOperationFlags::NoConfirmation);
		operationInfoFlags.Mod(FOF_NORECURSION, !(flags & SHOperationFlags::Recursive));

		operationInfo.fFlags = *operationInfoFlags;

		// Paths
		auto CreateZZString = [](const String& s) -> std::wstring
		{
			if (!s.IsEmpty())
			{
				std::wstring stringZZ;
				stringZZ.reserve(s.length() + 2);
				stringZZ.append(s.view());
				stringZZ.append(2, L'\0');

				return stringZZ;
			}
			return {};
		};
		std::wstring sourceZZ = CreateZZString(source.GetFullPath());
		std::wstring destinationZZ = CreateZZString(destination.GetFullPath());

		operationInfo.pFrom = sourceZZ.data();
		operationInfo.pTo = destinationZZ.data();

		// Disable parent window if UI actions is allowed
		bool disabled = false;
		if (window && window->IsThisEnabled())
		{
			window->Disable();
			disabled = true;
		}
		Utility::ScopeGuard atExit([&]()
		{
			// Re-enable parent window
			if (disabled)
			{
				window->Enable();
			}
		});

		// Perform the operation, zero means function succeeded.
		return ::SHFileOperationW(&operationInfo) == 0 && !operationInfo.fAnyOperationsAborted;
	}
	bool FormatVolume(const wxWindow* window, const LegacyVolume& volume, bool quickFormat) noexcept
	{
		if (volume && volume.DoesExist())
		{
			const HWND handle = reinterpret_cast<HWND>(UI::GetOwnerWindowHandle(window));
			switch (::SHFormatDrive(handle, static_cast<UINT>(volume.GetIndex()), SHFMT_ID_DEFAULT, quickFormat ? SHFMT_OPT_FULL : 0))
			{
				case SHFMT_ERROR:
				case SHFMT_CANCEL:
				case SHFMT_NOFORMAT:
				{
					return false;
				}
			};
			return true;
		}
		return false;
	}
	bool PinShortcut(const FSPath& filePath, SHPinShortcutCommand command)
	{
		#pragma warning(push, 0)
		#pragma warning(disable: 4311)
		#pragma warning(disable: 4302)

		String path = filePath.GetFullPath();
		HINSTANCE result = nullptr;

		switch (command)
		{
			case SHPinShortcutCommand::PinStartMenu:
			{
				result = ::ShellExecuteW(nullptr, L"startpin", path.wc_str(), nullptr, nullptr, SW_SHOWNORMAL);
				break;
			}
			case SHPinShortcutCommand::UnpinStartMenu:
			{
				result = ::ShellExecuteW(nullptr, L"startunpin", path.wc_str(), nullptr, nullptr, SW_SHOWNORMAL);
				break;
			}
			case SHPinShortcutCommand::PinTaskbar:
			{
				result = ::ShellExecuteW(nullptr, L"taskbarpin", path.wc_str(), nullptr, nullptr, SW_SHOWNORMAL);
				break;
			}
			case SHPinShortcutCommand::UnpinTaskbar:
			{
				result = ::ShellExecuteW(nullptr, L"taskbarunpin", path.wc_str(), nullptr, nullptr, SW_SHOWNORMAL);
				break;
			}
		}
		return reinterpret_cast<int>(result) > 32;

		#pragma warning(pop)
	}

	bool Execute(const wxWindow* window,
				 const FSPath& path,
				 const String& command,
				 const String& parameters,
				 const FSPath& workingDirectory,
				 FlagSet<SHWindowCommand> showWindow,
				 FlagSet<SHExexuteFlag> flags
	)
	{
		SHELLEXECUTEINFOW executeInfo = {};
		executeInfo.cbSize = sizeof(executeInfo);

		FlagSet<decltype(executeInfo.fMask)> executeInfoMask = SEE_MASK_DEFAULT|SEE_MASK_INVOKEIDLIST|SEE_MASK_UNICODE;
		executeInfoMask.Add(SEE_MASK_FLAG_NO_UI, flags & SHExexuteFlag::HideUI);
		executeInfoMask.Add(SEE_MASK_NOASYNC, !(flags & SHExexuteFlag::Async));
		executeInfoMask.Add(SEE_MASK_NO_CONSOLE, flags & SHExexuteFlag::InheritConsole);
		executeInfo.fMask = *executeInfoMask;

		executeInfo.hwnd = reinterpret_cast<HWND>(UI::GetOwnerWindowHandle(window));
		executeInfo.lpVerb = command.wc_str();
		executeInfo.nShow = Private::MapSHWindowCommand(showWindow).value_or(SW_SHOWNORMAL);

		const String pathString = path.GetFullPath();
		executeInfo.lpFile = pathString.wc_str();

		const String workingDirectoryString = workingDirectory.GetFullPath();
		executeInfo.lpDirectory = workingDirectoryString.IsEmpty() ? nullptr : workingDirectoryString.wc_str();

		executeInfo.lpParameters = parameters.IsEmpty() ? nullptr : parameters.wc_str();

		COMInitGuard comInit(COMThreadingModel::Apartment, COMInitFlag::DisableOLE1DDE);
		return ::ShellExecuteExW(&executeInfo);
	}
	bool OpenURI(const wxWindow* window, const URI& uri, FlagSet<SHWindowCommand> showWindow, FlagSet<SHExexuteFlag> flags)
	{
		return Execute(window, uri.BuildUnescapedURI(), {}, {}, {}, showWindow, flags);
	}
	HResult ExploreToItem(const FSPath& path)
	{
		HResult hr = E_FAIL;

		SFGAOF attributes = 0;
		String pathString = path.GetFullPath();
		COMMemoryPtr<ITEMIDLIST> item;
		if (hr = ::SHParseDisplayName(pathString.wc_str(), nullptr, &item, 0, &attributes))
		{
			COMInitGuard comInit(COMThreadingModel::Apartment);
			return ::SHOpenFolderAndSelectItems(item, 0, nullptr, 0);
		}
		return hr;
	}

	BitmapImage GetFileIcon(const FSPath& path, FlagSet<SHGetFileIconFlag> flags)
	{
		SHFILEINFOW shellInfo = {};

		const String pathString = path.GetFullPath();
		if (::SHGetFileInfoW(pathString.wc_str(), 0, &shellInfo, sizeof(shellInfo), *MapSHGetFileIconFlag(flags)) != 0)
		{
			GDIIcon icon;
			icon.AttachHandle(shellInfo.hIcon);
			return icon.ToBitmapImage();
		}
		return {};
	}
	BitmapImage GetFileIcon(const FileItem& item, FlagSet<SHGetFileIconFlag> flags)
	{
		SHFILEINFOW shellInfo = {};

		const String pathString = item.GetName();
		const auto attributes = FileSystem::Private::MapFileAttributes(item.GetAttributes());
		if (::SHGetFileInfoW(pathString.wc_str(), *attributes, &shellInfo, sizeof(shellInfo), SHGFI_USEFILEATTRIBUTES|*MapSHGetFileIconFlag(flags)) != 0)
		{
			GDIIcon icon;
			icon.AttachHandle(shellInfo.hIcon);
			return icon.ToBitmapImage();
		}
		return {};
	}

	String QueryAssociation(const FSPath& filePath, SHQueryAssociation option, Any* extraData)
	{
		if (filePath)
		{
			// AssocQueryString requires extension with the dot
			return DoQueryAssociation('.' + filePath.GetExtension(), option, extraData);
		}
		return {};
	}
	String QueryAssociation(const UniversallyUniqueID& classID, SHQueryAssociation option, Any* extraData)
	{
		return DoQueryAssociation(classID.ToString(UUIDFormat::CurlyBraces), option, extraData);
	}

	String GetLocalizedName(const FSPath& path, int* resourceID)
	{
		int resID = 0;
		XChar buffer[INT16_MAX] = {};
		String pathString = path.GetFullPath();

		if (HResult(::SHGetLocalizedName(pathString.wc_str(), buffer, std::size(buffer), &resID)).IsOK())
		{
			Utility::SetIfNotNull(resourceID, resID);
			return buffer;
		}
		return {};
	}
	HResult SetLocalizedName(const FSPath& path, const String& resourse, int resourceID)
	{
		String pathString = path.GetFullPath();
		return ::SHSetLocalizedName(pathString.wc_str(), resourse.wc_str(), resourceID);
	}

	FSPath GetKnownDirectory(KnownDirectoryID id, FlagSet<SHGetKnownDirectoryFlag> flags)
	{
		if (id == KnownDirectoryID::None)
		{
			return {};
		}

		FSPath result;
		bool isEnvUsed = false;
		bool isCustomMethodUsed = false;

		// For non 64-bit systems this function will retrieve 32-bit versions of these paths
		if (!System::Is64Bit())
		{
			switch (id)
			{
				case KnownDirectoryID::ProgramFilesX64:
				{
					id = KnownDirectoryID::ProgramFiles;
					break;
				}
				case KnownDirectoryID::CommonFilesX64:
				{
					id = KnownDirectoryID::CommonFiles;
					break;
				}
			};
		}
		else
		{
			String envVariable;
			switch (id)
			{
				case KnownDirectoryID::ProgramFilesX64:
				{
					isEnvUsed = true;
					envVariable = "%ProgramW6432%";
					break;
				}
				case KnownDirectoryID::CommonFilesX64:
				{
					isEnvUsed = true;
					envVariable = "%CommonProgramW6432%";
					break;
				}
			};

			if (isEnvUsed)
			{
				result = System::ExpandEnvironmentStrings(envVariable);
			}
		}

		if (!isEnvUsed)
		{
			switch (id)
			{
				case KnownDirectoryID::Temp:
				{
					wchar_t buffer[INT16_MAX] = {};
					if (::GetTempPathW(std::size(buffer), buffer) != 0)
					{
						result = buffer;
					}

					isCustomMethodUsed = true;
					break;
				}
				case KnownDirectoryID::SystemDrive:
				{
					wchar_t buffer[INT16_MAX] = {};
					if (::GetSystemDirectoryW(buffer, std::size(buffer)) >= 2)
					{
						result = LegacyVolume::FromChar(buffer).GetPath();
					}

					isCustomMethodUsed = true;
					break;
				}
			};
		}

		Utility::ScopeGuard atExit = [&]()
		{
			if ((isEnvUsed || isCustomMethodUsed) && flags & SHGetKnownDirectoryFlag::CreateIfDoesNotExist)
			{
				NativeFileSystem().CreateDirectory(result);
			}
		};
		if (!isEnvUsed && !isCustomMethodUsed)
		{
			auto begin = std::begin(Private::KnownDirectoryGUID::Items);
			auto end = std::end(Private::KnownDirectoryGUID::Items);
			auto it = std::find_if(begin, end, [&](const auto& item)
			{
				return item.first == id;
			});
			if (it != end && it->second != GUID_NULL)
			{
				FlagSet<DWORD> nativeFlags = KF_FLAG_DONT_VERIFY;
				nativeFlags.Add(KF_FLAG_DEFAULT_PATH, flags & SHGetKnownDirectoryFlag::UseDefaultLocation);
				nativeFlags.Add(KF_FLAG_CREATE|KF_FLAG_INIT, flags & SHGetKnownDirectoryFlag::CreateIfDoesNotExist);

				COMMemoryPtr<wchar_t> knownPath;
				if (HResult(::SHGetKnownFolderPath(it->second, *nativeFlags, nullptr, &knownPath)))
				{
					result = knownPath.Get();
				}
			}
		}
		return result;
	}
	size_t EnumKnownDirectories(std::function<bool(KnownDirectoryID, String)> func)
	{
		using namespace Private;

		return KnownDirectoryDefinition::EnumItems([&](const KnownDirectoryDefinition::TItem& item)
		{
			return std::invoke(func, item.GetValue(), String(item.GetName()));
		});
	}
}
