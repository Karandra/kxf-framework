#pragma once
#include "Common.h"
#include "HResult.h"
#include "SystemWindow.h"
#include "KnownDirectoryID.h"
#include "kxf/Core/String.h"
#include "kxf/Core/CallbackFunction.h"
#include "kxf/FileSystem/FSPath.h"

namespace kxf
{
	class Any;
	class URI;
	class FileItem;
	class LegacyVolume;
	class UniversallyUniqueID;
	class BitmapImage;
}

namespace kxf
{
	enum class SHOperationType
	{
		Copy,
		Move,
		Rename,
		Delete
	};
	enum class SHOperationFlags: uint32_t
	{
		None = 0,
		Recursive = 1 << 1,
		LimitToFiles = 1 << 2,
		AllowUndo = 1 << 3,
		NoConfirmation = 1 << 4,
	};
	enum class SHPinShortcutCommand
	{
		PinStartMenu,
		UnpinStartMenu,

		PinTaskbar,
		UnpinTaskbar,
	};
	enum class SHQueryAssociation
	{
		None = -1,

		Command,
		Executable,
		FriendlyDocName,
		FriendlyAppName,
		InfoTip,
		QuickTip,
		TileInfo,
		ContentType,
		DefaultIcon,
		ShellExtension,
		SupportedURIProtocols,
		ProgID,
		AppID,
		AppPublisher,
		AppIconReference,
	};
	enum class SHGetKnownDirectoryFlag: uint32_t
	{
		None = 0,

		UseDefaultLocation = 1 << 0,
		CreateIfDoesNotExist = 1 << 1,
	};
	enum class SHGetFileIconFlag: uint32_t
	{
		None = 0,

		Small = 1 << 0,
		Large = 1 << 1,
		ShellSized = 1 << 2,
		Selected = 1 << 3,
		Open = 1 << 4,
		AddOverlays = 1 << 5,
		LinkOverlay = 1 << 6,
	};
	enum class SHExexuteFlag: uint32_t
	{
		None = 0,

		Async = 1 << 0,
		HideUI = 1 << 1,
		InheritConsole = 1 << 2,
	};

	kxf_FlagSet_Declare(SHOperationFlags);
	kxf_FlagSet_Declare(SHGetKnownDirectoryFlag);
	kxf_FlagSet_Declare(SHGetFileIconFlag);
	kxf_FlagSet_Declare(SHExexuteFlag);
}

namespace kxf::Shell
{
	KXF_API bool FileOperation(SHOperationType opType, const FSPath& source, const FSPath& destination, SystemWindow window = {}, FlagSet<SHOperationFlags> flags = {});
	KXF_API bool FormatVolume(SystemWindow window, const LegacyVolume& volume, bool quickFormat = false) noexcept;
	KXF_API bool PinShortcut(const FSPath& filePath, SHPinShortcutCommand command);

	KXF_API bool Execute(SystemWindow window,
						 const FSPath& path,
						 const String& command = {},
						 const String& parameters = {},
						 const FSPath& workingDirectory = {},
						 FlagSet<SHWindowCommand> showWindow = SHWindowCommand::Show,
						 FlagSet<SHExexuteFlag> flags = {}
	);
	KXF_API bool OpenURI(SystemWindow window, const URI& uri, FlagSet<SHWindowCommand> showWindow = SHWindowCommand::Show, FlagSet<SHExexuteFlag> flags = {});
	KXF_API HResult ExploreToItem(const FSPath& path);

	KXF_API BitmapImage GetFileIcon(const FSPath& path, FlagSet<SHGetFileIconFlag> flags);
	KXF_API BitmapImage GetFileIcon(const FileItem& item, FlagSet<SHGetFileIconFlag> flags);

	KXF_API String QueryAssociation(const FSPath& filePath, SHQueryAssociation option, Any* extraData = nullptr);
	KXF_API String QueryAssociation(const UniversallyUniqueID& classID, SHQueryAssociation option, Any* extraData = nullptr);

	KXF_API String GetLocalizedName(const FSPath& path, int* resourceID = nullptr);
	KXF_API HResult SetLocalizedName(const FSPath& path, const String& resourse, int resourceID);

	KXF_API FSPath GetKnownDirectory(KnownDirectoryID id, FlagSet<SHGetKnownDirectoryFlag> flags = {});
	KXF_API CallbackResult<void> EnumKnownDirectories(CallbackFunction<KnownDirectoryID, String> func);
}
