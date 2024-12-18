#pragma once
#include "../Common.h"
#include "kxf/Core/NativeUUID.h"
#include "kxf/System/KnownDirectoryID.h"

#include <Windows.h>
#include <KnownFolders.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf::Shell::Private
{
	#define kxf_SHDItem(id, guid)	{KnownDirectoryID::##id, guid}

	struct KnownDirectoryGUID final
	{
		static inline std::pair<KnownDirectoryID, ::_GUID> Items[] =
		{
			kxf_SHDItem(Music, FOLDERID_Music),
			kxf_SHDItem(MusicCommon, FOLDERID_PublicMusic),
			kxf_SHDItem(MusicLibrary, FOLDERID_MusicLibrary),
			kxf_SHDItem(MusicSample, FOLDERID_SampleMusic),
			kxf_SHDItem(MusicPlaylists, FOLDERID_Playlists),
			kxf_SHDItem(MusicSamplePlaylists, FOLDERID_SamplePlaylists),

			kxf_SHDItem(Pictures, FOLDERID_Pictures),
			kxf_SHDItem(PicturesCommon, FOLDERID_PublicPictures),
			kxf_SHDItem(PicturesLibrary, FOLDERID_PicturesLibrary),
			kxf_SHDItem(PicturesPhotoalbums, FOLDERID_PhotoAlbums),
			kxf_SHDItem(PicturesSkydrive, FOLDERID_SkyDrivePictures),
			kxf_SHDItem(PicturesScreenshots, FOLDERID_Screenshots),
			kxf_SHDItem(PicturesSampleCommon, FOLDERID_SamplePictures),
			kxf_SHDItem(PicturesAccounts, FOLDERID_AccountPictures),
			kxf_SHDItem(PicturesAccountsCommon, FOLDERID_PublicUserTiles),
			kxf_SHDItem(PicturesSaved, FOLDERID_SavedPictures),
			kxf_SHDItem(PicturesSavedLibrary, FOLDERID_SavedPicturesLibrary),
			kxf_SHDItem(PicturesCameraRoll, FOLDERID_CameraRoll),
			kxf_SHDItem(PicturesCameraRollOneDrive, FOLDERID_SkyDriveCameraRoll),

			kxf_SHDItem(Videos, FOLDERID_Videos),
			kxf_SHDItem(VideosCommon, FOLDERID_PublicVideos),
			kxf_SHDItem(VideosLibrary, FOLDERID_VideosLibrary),
			kxf_SHDItem(VideosSample, FOLDERID_SampleVideos),

			kxf_SHDItem(Desktop, FOLDERID_Desktop),
			kxf_SHDItem(DesktopCommon, FOLDERID_PublicDesktop),

			kxf_SHDItem(Documents, FOLDERID_Documents),
			kxf_SHDItem(DocumentsCommon, FOLDERID_PublicDocuments),
			kxf_SHDItem(DocumentsLibrary, FOLDERID_DocumentsLibrary),
			kxf_SHDItem(DocumentsOneDrive, FOLDERID_SkyDriveDocuments),

			kxf_SHDItem(StartMenu, FOLDERID_StartMenu),
			kxf_SHDItem(StartMenuCommon, FOLDERID_CommonStartMenu),

			kxf_SHDItem(StartMenuPrograms, FOLDERID_Programs),
			kxf_SHDItem(StartMenuProgramsCommon, FOLDERID_CommonPrograms),

			kxf_SHDItem(Startup, FOLDERID_Startup),
			kxf_SHDItem(StartupCommon, FOLDERID_CommonStartup),

			kxf_SHDItem(CommonFiles, FOLDERID_ProgramFilesCommon),
			kxf_SHDItem(CommonFilesX86, FOLDERID_ProgramFilesCommonX86),
			kxf_SHDItem(CommonFilesX64, FOLDERID_ProgramFilesCommonX64),

			kxf_SHDItem(ProgramFiles, FOLDERID_ProgramFiles),
			kxf_SHDItem(ProgramFilesX86, FOLDERID_ProgramFilesX86),
			kxf_SHDItem(ProgramFilesX64, FOLDERID_ProgramFilesX64),

			kxf_SHDItem(SystemDrive, GUID_NULL),
			kxf_SHDItem(Windows, FOLDERID_Windows),
			kxf_SHDItem(Fonts, FOLDERID_Fonts),
			kxf_SHDItem(Temp, GUID_NULL),

			kxf_SHDItem(System, FOLDERID_System),
			kxf_SHDItem(SystemX86, FOLDERID_SystemX86),

			kxf_SHDItem(ApplicationData, FOLDERID_RoamingAppData),
			kxf_SHDItem(ApplicationDataLocal, FOLDERID_LocalAppData),
			kxf_SHDItem(ApplicationDataLocalLow, FOLDERID_LocalAppDataLow),

			kxf_SHDItem(AdminTools, FOLDERID_AdminTools),
			kxf_SHDItem(AdminToolsCommon, FOLDERID_CommonAdminTools),
			kxf_SHDItem(ApplicationShortcuts, FOLDERID_ApplicationShortcuts),
			kxf_SHDItem(CDBurning, FOLDERID_CDBurning),
			kxf_SHDItem(OEMLinksCommon, FOLDERID_CommonOEMLinks),
			kxf_SHDItem(Templates, FOLDERID_Templates),
			kxf_SHDItem(TemplatesCommon, FOLDERID_CommonTemplates),
			kxf_SHDItem(Contacts, FOLDERID_Contacts),
			kxf_SHDItem(Cookies, FOLDERID_Cookies),
			kxf_SHDItem(DeviceMetaDataStore, FOLDERID_DeviceMetadataStore),
			kxf_SHDItem(Downloads, FOLDERID_Downloads),
			kxf_SHDItem(Favorites, FOLDERID_Favorites),
			kxf_SHDItem(GameTasks, FOLDERID_GameTasks),
			kxf_SHDItem(GameTasksCommon, FOLDERID_PublicGameTasks),
			kxf_SHDItem(History, FOLDERID_History),
			kxf_SHDItem(ImplicitAppShortcuts, FOLDERID_ImplicitAppShortcuts),
			kxf_SHDItem(InternetCache, FOLDERID_InternetCache),
			kxf_SHDItem(Libraries, FOLDERID_Libraries),
			kxf_SHDItem(LibrariesCommon, FOLDERID_PublicLibraries),
			kxf_SHDItem(Links, FOLDERID_Links),
			kxf_SHDItem(LocalizedResources, FOLDERID_LocalizedResourcesDir),
			kxf_SHDItem(NetHood, FOLDERID_NetHood),
			kxf_SHDItem(OriginalImages, FOLDERID_OriginalImages),
			kxf_SHDItem(PrintHood, FOLDERID_PrintHood),
			kxf_SHDItem(UserProfile, FOLDERID_Profile),
			kxf_SHDItem(ProgramData, FOLDERID_ProgramData),
			kxf_SHDItem(ProgramDataCommon, FOLDERID_Public),
			kxf_SHDItem(Ringtones, FOLDERID_Ringtones),
			kxf_SHDItem(RingtonesCommon, FOLDERID_PublicRingtones),
			kxf_SHDItem(QuickLaunch, FOLDERID_QuickLaunch),
			kxf_SHDItem(Recent, FOLDERID_Recent),
			kxf_SHDItem(RecordedtvLibrary, FOLDERID_RecordedTVLibrary),
			kxf_SHDItem(ResourceDir, FOLDERID_ResourceDir),
			kxf_SHDItem(RoamedTileImages, FOLDERID_RoamedTileImages),
			kxf_SHDItem(RoamingTiles, FOLDERID_RoamingTiles),
			kxf_SHDItem(SavedGames, FOLDERID_SavedGames),
			kxf_SHDItem(SearchHistory, FOLDERID_SearchHistory),
			kxf_SHDItem(SearchTemplates, FOLDERID_SearchTemplates),
			kxf_SHDItem(SendTo, FOLDERID_SendTo),
			kxf_SHDItem(SidebarParts, FOLDERID_SidebarParts),
			kxf_SHDItem(SidebarPartsDefault, FOLDERID_SidebarDefaultParts),
			kxf_SHDItem(OneDrive, FOLDERID_SkyDrive),
			kxf_SHDItem(UserPinned, FOLDERID_UserPinned),
			kxf_SHDItem(UserProfiles, FOLDERID_UserProfiles),
			kxf_SHDItem(UserProgramFiles, FOLDERID_UserProgramFiles),
			kxf_SHDItem(UserProgramFilesCommon, FOLDERID_UserProgramFilesCommon)
		};
	};
	#undef kxf_SHDItem
}
