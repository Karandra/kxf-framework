#pragma once
#include "../Common.h"
#include "kxf/Core/IndexedEnum.h"
#include "kxf/System/KnownDirectoryID.h"

namespace kxf::Shell::Private
{
	#define kxf_SHDItem(id)	{KnownDirectoryID::##id, kxfS(#id)}

	struct KnownDirectoryDefinition final: IndexedEnumDefinition<KnownDirectoryDefinition, KnownDirectoryID, StringView, true>
	{
		static inline constexpr TItem Items[] =
		{
			kxf_SHDItem(Music),
			kxf_SHDItem(MusicCommon),
			kxf_SHDItem(MusicLibrary),
			kxf_SHDItem(MusicSample),
			kxf_SHDItem(MusicPlaylists),
			kxf_SHDItem(MusicSamplePlaylists),

			kxf_SHDItem(Pictures),
			kxf_SHDItem(PicturesCommon),
			kxf_SHDItem(PicturesLibrary),
			kxf_SHDItem(PicturesPhotoalbums),
			kxf_SHDItem(PicturesSkydrive),
			kxf_SHDItem(PicturesScreenshots),
			kxf_SHDItem(PicturesSampleCommon),
			kxf_SHDItem(PicturesAccounts),
			kxf_SHDItem(PicturesAccountsCommon),
			kxf_SHDItem(PicturesSaved),
			kxf_SHDItem(PicturesSavedLibrary),
			kxf_SHDItem(PicturesCameraRoll),
			kxf_SHDItem(PicturesCameraRollOneDrive),

			kxf_SHDItem(Videos),
			kxf_SHDItem(VideosCommon),
			kxf_SHDItem(VideosLibrary),
			kxf_SHDItem(VideosSample),

			kxf_SHDItem(Desktop),
			kxf_SHDItem(DesktopCommon),

			kxf_SHDItem(Documents),
			kxf_SHDItem(DocumentsCommon),
			kxf_SHDItem(DocumentsLibrary),
			kxf_SHDItem(DocumentsOneDrive),

			kxf_SHDItem(StartMenu),
			kxf_SHDItem(StartMenuCommon),

			kxf_SHDItem(StartMenuPrograms),
			kxf_SHDItem(StartMenuProgramsCommon),

			kxf_SHDItem(Startup),
			kxf_SHDItem(StartupCommon),

			kxf_SHDItem(CommonFiles),
			kxf_SHDItem(CommonFilesX86),
			kxf_SHDItem(CommonFilesX64),

			kxf_SHDItem(ProgramFiles),
			kxf_SHDItem(ProgramFilesX86),
			kxf_SHDItem(ProgramFilesX64),

			kxf_SHDItem(SystemDrive),
			kxf_SHDItem(Windows),
			kxf_SHDItem(Fonts),
			kxf_SHDItem(Temp),

			kxf_SHDItem(System),
			kxf_SHDItem(SystemX86),

			kxf_SHDItem(ApplicationData),
			kxf_SHDItem(ApplicationDataLocal),
			kxf_SHDItem(ApplicationDataLocalLow),

			kxf_SHDItem(AdminTools),
			kxf_SHDItem(AdminToolsCommon),
			kxf_SHDItem(ApplicationShortcuts),
			kxf_SHDItem(CDBurning),
			kxf_SHDItem(OEMLinksCommon),
			kxf_SHDItem(Templates),
			kxf_SHDItem(TemplatesCommon),
			kxf_SHDItem(Contacts),
			kxf_SHDItem(Cookies),
			kxf_SHDItem(DeviceMetaDataStore),
			kxf_SHDItem(Downloads),
			kxf_SHDItem(Favorites),
			kxf_SHDItem(GameTasks),
			kxf_SHDItem(GameTasksCommon),
			kxf_SHDItem(History),
			kxf_SHDItem(ImplicitAppShortcuts),
			kxf_SHDItem(InternetCache),
			kxf_SHDItem(Libraries),
			kxf_SHDItem(LibrariesCommon),
			kxf_SHDItem(Links),
			kxf_SHDItem(LocalizedResources),
			kxf_SHDItem(NetHood),
			kxf_SHDItem(OriginalImages),
			kxf_SHDItem(PrintHood),
			kxf_SHDItem(UserProfile),
			kxf_SHDItem(ProgramData),
			kxf_SHDItem(ProgramDataCommon),
			kxf_SHDItem(Ringtones),
			kxf_SHDItem(RingtonesCommon),
			kxf_SHDItem(QuickLaunch),
			kxf_SHDItem(Recent),
			kxf_SHDItem(RecordedtvLibrary),
			kxf_SHDItem(ResourceDir),
			kxf_SHDItem(RoamedTileImages),
			kxf_SHDItem(RoamingTiles),
			kxf_SHDItem(SavedGames),
			kxf_SHDItem(SearchHistory),
			kxf_SHDItem(SearchTemplates),
			kxf_SHDItem(SendTo),
			kxf_SHDItem(SidebarParts),
			kxf_SHDItem(SidebarPartsDefault),
			kxf_SHDItem(OneDrive),
			kxf_SHDItem(UserPinned),
			kxf_SHDItem(UserProfiles),
			kxf_SHDItem(UserProgramFiles),
			kxf_SHDItem(UserProgramFilesCommon)
		};
	};

	#undef kxf_SHDItem
}
