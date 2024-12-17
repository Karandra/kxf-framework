#include "KxfPCH.h"
#include "StorageVolume.h"
#include "NativeFileSystem.h"
#include "LegacyVolume.h"
#include "FSPath.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/INativeStream.h"
#include "kxf/System/HandlePtr.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/ScopeGuard.h"

namespace
{
	using namespace kxf;

	constexpr size_t g_GUIDLength = 36;
	constexpr size_t g_VolumePathPrefixLength = 10;
	constexpr size_t g_VolumePathTotalLength = g_VolumePathPrefixLength + g_GUIDLength + 3;

	constexpr size_t g_FirstLegacyVolume = 'A';
	constexpr size_t g_LastLegacyVolume = 'Z';
	constexpr size_t g_LegacyVolumesCount = g_LastLegacyVolume - g_FirstLegacyVolume + 1;

	constexpr DriveType MapDriveType(UINT type) noexcept
	{
		switch (type)
		{
			case DRIVE_UNKNOWN:
			{
				return DriveType::Unknown;
			}
			case DRIVE_NO_ROOT_DIR:
			{
				return DriveType::NotMounted;
			}
			case DRIVE_REMOVABLE:
			{
				return DriveType::Removable;
			}
			case DRIVE_REMOTE:
			{
				return DriveType::Remote;
			}
			case DRIVE_FIXED:
			{
				return DriveType::Fixed;
			}
			case DRIVE_CDROM:
			{
				return DriveType::Optical;
			}
			case DRIVE_RAMDISK:
			{
				return DriveType::Memory;
			}
		};
		return DriveType::Unknown;
	}
	constexpr FlagSet<FileSystemFeature> MapDiskFeatures(DWORD nativeFeatures) noexcept
	{
		FlagSet<FileSystemFeature> features;
		features.Add(FileSystemFeature::CasePreservedNames, nativeFeatures & FILE_CASE_PRESERVED_NAMES);
		features.Add(FileSystemFeature::CaseSensitiveSearch, nativeFeatures & FILE_CASE_SENSITIVE_SEARCH);
		features.Add(FileSystemFeature::FileCompression, nativeFeatures & FILE_FILE_COMPRESSION);
		features.Add(FileSystemFeature::NamedStreams, nativeFeatures & FILE_NAMED_STREAMS);
		features.Add(FileSystemFeature::PersistentACLS, nativeFeatures & FILE_PERSISTENT_ACLS);
		features.Add(FileSystemFeature::ReadOnlyVolume, nativeFeatures & FILE_READ_ONLY_VOLUME);
		features.Add(FileSystemFeature::SequentialWrite, nativeFeatures & FILE_SEQUENTIAL_WRITE_ONCE);
		features.Add(FileSystemFeature::Encryption, nativeFeatures & FILE_SUPPORTS_ENCRYPTION);
		features.Add(FileSystemFeature::ExtendedAttributes, nativeFeatures & FILE_SUPPORTS_EXTENDED_ATTRIBUTES);
		features.Add(FileSystemFeature::HardLinks, nativeFeatures & FILE_SUPPORTS_HARD_LINKS);
		features.Add(FileSystemFeature::ObjectIDs, nativeFeatures & FILE_SUPPORTS_OBJECT_IDS);
		features.Add(FileSystemFeature::OpenFileByID, nativeFeatures & FILE_SUPPORTS_OPEN_BY_FILE_ID);
		features.Add(FileSystemFeature::ReparsePoints, nativeFeatures & FILE_SUPPORTS_REPARSE_POINTS);
		features.Add(FileSystemFeature::SparseFiles, nativeFeatures & FILE_SUPPORTS_SPARSE_FILES);
		features.Add(FileSystemFeature::Transactions, nativeFeatures & FILE_SUPPORTS_TRANSACTIONS);
		features.Add(FileSystemFeature::USNJournal, nativeFeatures & FILE_SUPPORTS_USN_JOURNAL);
		features.Add(FileSystemFeature::Unicode, nativeFeatures & FILE_UNICODE_ON_DISK);
		features.Add(FileSystemFeature::CompressedVolume, nativeFeatures & FILE_VOLUME_IS_COMPRESSED);
		features.Add(FileSystemFeature::VolumeQuotas, nativeFeatures & FILE_VOLUME_QUOTAS);

		return features;
	}
}

namespace kxf
{
	Enumerator<StorageVolume> StorageVolume::EnumVolumes()
	{
		return [handle = make_handle_ptr<::FindVolumeClose, HANDLE, INVALID_HANDLE_VALUE>(nullptr)](IEnumerator& en) mutable -> std::optional<StorageVolume>
		{
			XChar volumeGuidPath[64] = {};
			if (!handle)
			{
				handle = ::FindFirstVolumeW(volumeGuidPath, std::size(volumeGuidPath));
				if (!handle)
				{
					return {};
				}
			}
			else if (!::FindNextVolumeW(*handle, volumeGuidPath, std::size(volumeGuidPath)))
			{
				return {};
			}

			StorageVolume volume;
			volume.AssignPath(volumeGuidPath);

			if (!volume)
			{
				en.SkipCurrent();
				return {};
			}
			return volume;
		};
	}
	Enumerator<std::pair<StorageVolume, LegacyVolume>> StorageVolume::EnumLegacyVolumes()
	{
		return [driveLetter = g_FirstLegacyVolume](IEnumerator& en) mutable -> std::optional<std::pair<StorageVolume, LegacyVolume>>
		{
			if (driveLetter <= g_LastLegacyVolume)
			{
				XChar disk[] = kxS("\0:\\");
				disk[0] = driveLetter++;

				XChar volumeGuidPath[64] = {};
				if (::GetVolumeNameForVolumeMountPointW(disk, volumeGuidPath, std::size(volumeGuidPath)))
				{
					StorageVolume volume;
					volume.AssignPath(volumeGuidPath);

					if (volume)
					{
						return std::make_pair(std::move(volume), LegacyVolume::FromChar(UniChar(disk[0])));
					}
				}
				en.SkipCurrent();
			}
			return {};
		};
	}
	
	bool StorageVolume::RemoveMountPoint(const FSPath& path) noexcept
	{
		if (path)
		{
			String mountPoint = path.GetFullPathTryNS(FSPathNamespace::Win32File);
			return ::DeleteVolumeMountPointW(mountPoint.wc_str());
		}
		return false;
	}
	bool StorageVolume::RemoveMountPoint(const LegacyVolume& volume) noexcept
	{
		if (volume)
		{
			XChar disk[] = kxS("\0:\\");
			disk[0] = volume.GetChar();

			return ::DeleteVolumeMountPointW(disk);
		}
		return false;
	}

	StorageVolume::StorageVolume(const UniversallyUniqueID& id)
	{
		static_assert(Utility::ArraySize<decltype(StorageVolume::m_Path)>::value >= g_VolumePathTotalLength + 1, "insufficient buffer size");

		AssignPath(Format(R"(\\?\Volume{}\)", id.ToString(UUIDFormat::CurlyBraces)));
	}
	StorageVolume::StorageVolume(const LegacyVolume& legacyVolume)
	{
		if (legacyVolume)
		{
			String volumePath = legacyVolume.GetPath().GetFullPath(FSPathFormat::TrailingSeparator);
			XChar volumeGuidPath[64] = {};
			if (::GetVolumeNameForVolumeMountPointW(volumePath.wc_str(), volumeGuidPath, std::size(volumeGuidPath)))
			{
				AssignPath(volumeGuidPath);
			}
		}
	}
	StorageVolume::StorageVolume(const FSPath& path)
	{
		if (path.ContainsStorageVolume())
		{
			// Volume paths should be in following format: '\\?\Volume{66843779-55ae-45c5-9abe-b67ccee14079}\'.
			AssignPath(path.GetFullPathRequireNS(FSPathNamespace::Win32File, FSPathFormat::TrailingSeparator).SubLeft(g_VolumePathTotalLength));
		}
	}

	bool StorageVolume::IsNull() const noexcept
	{
		return m_Path[0] == '\0' || m_Length != g_VolumePathTotalLength;
	}
	bool StorageVolume::DoesExist() const noexcept
	{
		return ::GetDriveTypeW(m_Path) != DRIVE_NO_ROOT_DIR;
	}

	UniversallyUniqueID StorageVolume::GetUniqueID() const noexcept
	{
		if (!IsNull())
		{
			auto guid = StringViewOf(m_Path).substr(g_VolumePathPrefixLength + 1, g_GUIDLength);

			XChar buffer[64] = {};
			std::char_traits<XChar>::copy(buffer, guid.data(), guid.length());
			return UniversallyUniqueID::CreateFromString(buffer);
		}
		return {};
	}
	FSPath StorageVolume::GetPath() const
	{
		// FSPath should parse this into 'Volume{guid}' with Win32 namespace
		return m_Path;
	}
	FSPath StorageVolume::GetDevicePath() const
	{
		auto path = GetPath();
		path.SetNamespace(FSPathNamespace::Win32Device);

		return path;
	}

	String StorageVolume::GetLabel() const
	{
		XChar buffer[MAX_PATH + 1] = {};
		::GetVolumeInformationW(m_Path, buffer, std::size(buffer), nullptr, nullptr, nullptr, nullptr, 0);
		return buffer;
	}
	bool StorageVolume::SetLabel(const String& label)
	{
		return ::SetVolumeLabelW(m_Path, label.IsEmpty() ? nullptr : label.wc_str());
	}

	DriveType StorageVolume::GetType() const noexcept
	{
		if (!IsNull())
		{
			return MapDriveType(::GetDriveTypeW(m_Path));
		}
		return DriveType::Unknown;
	}
	uint32_t StorageVolume::GetSerialNumber() const noexcept
	{
		DWORD serialNumber = std::numeric_limits<uint32_t>::max();
		::GetVolumeInformationW(m_Path, nullptr, 0, &serialNumber, nullptr, nullptr, nullptr, 0);
		return serialNumber;
	}
	String StorageVolume::GetFileSystem() const noexcept
	{
		XChar buffer[MAX_PATH + 1] = {};
		::GetVolumeInformationW(m_Path, nullptr, 0, nullptr, nullptr, nullptr, buffer, std::size(buffer));
		return buffer;
	}
	FlagSet<FileSystemFeature> StorageVolume::GetFileSystemFeatures() const noexcept
	{
		DWORD nativeFeatures = 0;
		DWORD maximumComponentLength = 0;
		if (::GetVolumeInformationW(m_Path, nullptr, 0, nullptr, &maximumComponentLength, &nativeFeatures, nullptr, 0))
		{
			auto features = MapDiskFeatures(nativeFeatures);
			features.Add(FileSystemFeature::LongFileNames, maximumComponentLength == 255);

			return features;
		}
		return {};
	}

	auto StorageVolume::GetSpaceLayoutInfo() const noexcept -> std::optional<SpaceLayoutInfo>
	{
		DWORD bytesPerSector = 0;
		DWORD sectorsPerCluster = 0;
		DWORD numberOfFreeClusters = 0;
		DWORD totalNumberOfClusters = 0;
		if (::GetDiskFreeSpaceW(m_Path, &sectorsPerCluster, &bytesPerSector, &numberOfFreeClusters, &totalNumberOfClusters))
		{
			SpaceLayoutInfo layoutInfo = {};
			layoutInfo.DataPerSector = DataSize::FromBytes(bytesPerSector);
			layoutInfo.SectorsPerCluster = sectorsPerCluster;
			layoutInfo.FreeClusters = numberOfFreeClusters;
			layoutInfo.TotalClusters = totalNumberOfClusters;

			return layoutInfo;
		}
		return {};
	}
	DataSize StorageVolume::GetTotalSpace() const noexcept
	{
		ULARGE_INTEGER value = {};
		if (::GetDiskFreeSpaceExW(m_Path, nullptr, &value, nullptr))
		{
			return DataSize::FromBytes(value.QuadPart);
		}
		return {};
	}
	DataSize StorageVolume::GetUsedSpace() const noexcept
	{
		ULARGE_INTEGER total = {};
		ULARGE_INTEGER free = {};

		if (::GetDiskFreeSpaceExW(m_Path, nullptr, &total, &free))
		{
			return DataSize::FromBytes(total.QuadPart - free.QuadPart);
		}
		return {};
	}
	DataSize StorageVolume::GetFreeSpace() const noexcept
	{
		ULARGE_INTEGER value = {};
		if (::GetDiskFreeSpaceExW(m_Path, nullptr, nullptr, &value))
		{
			return DataSize::FromBytes(value.QuadPart);
		}
		return {};
	}

	LegacyVolume StorageVolume::GetLegacyVolume() const
	{
		for (auto&& [volume, legacyVolume]: EnumLegacyVolumes())
		{
			if (volume == *this)
			{
				return legacyVolume;
			}
		}
		return {};
	}
	Enumerator<FSPath> StorageVolume::EnumMountPoints() const
	{
		return [this, handle = make_handle_ptr<::FindVolumeMountPointClose, HANDLE, INVALID_HANDLE_VALUE>(nullptr)]() mutable -> std::optional<FSPath>
		{
			XChar buffer[std::numeric_limits<int16_t>::max()] = {};
			if (handle)
			{
				if (::FindNextVolumeMountPointW(*handle, buffer, std::size(buffer)))
				{
					return FSPath(buffer);
				}
			}
			else if (handle.reset(::FindFirstVolumeMountPointW(m_Path, buffer, std::size(buffer))); handle)
			{
				return FSPath(buffer);
			}
			return {};
		};
		return {};
	}
	bool StorageVolume::SetMountPoint(const FSPath& path)
	{
		if (path)
		{
			String mountPoint = path.GetFullPathTryNS(FSPathNamespace::Win32File);
			return ::SetVolumeMountPointW(mountPoint.wc_str(), m_Path);
		}
		return false;
	}
	bool StorageVolume::SetMountPoint(const LegacyVolume& volume) noexcept
	{
		if (volume)
		{
			XChar disk[] = kxS("\0:\\");
			disk[0] = volume.GetChar();

			return ::SetVolumeMountPointW(disk, m_Path);
		}
		return false;
	}

	bool StorageVolume::EjectMedia() noexcept
	{
		if (auto stream = NativeFileSystem().OpenToRead(GetDevicePath(), IOStreamDisposition::OpenExisting, IOStreamShare::Everything))
		{
			if (auto nativeStream = stream->QueryInterface<INativeStream>())
			{
				void* handle = nativeStream->GetHandle();

				DWORD bytes = 0;
				::DeviceIoControl(handle, FSCTL_LOCK_VOLUME, nullptr, 0, nullptr, 0, &bytes, nullptr);
				::DeviceIoControl(handle, FSCTL_DISMOUNT_VOLUME, nullptr, 0, nullptr, 0, &bytes, nullptr);
				return ::DeviceIoControl(handle, IOCTL_STORAGE_EJECT_MEDIA, nullptr, 0, nullptr, 0, &bytes, nullptr);
			}
		}
		return false;
	}
}
