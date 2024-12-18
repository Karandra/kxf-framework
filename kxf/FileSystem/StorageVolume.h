#pragma once
#include "Common.h"
#include "FSPath.h"
#include "kxf/Core/UniversallyUniqueID.h"
#include "kxf/Core/DataSize.h"

namespace kxf
{
	class LegacyVolume;
}

namespace kxf
{
	class KXF_API StorageVolume final
	{
		public:
			struct SpaceLayoutInfo final
			{
				DataSize DataPerSector;
				uint32_t SectorsPerCluster = 0;
				uint32_t FreeClusters = 0;
				uint32_t TotalClusters = 0;
			};

		public:
			static Enumerator<StorageVolume> EnumVolumes();
			static Enumerator<std::pair<StorageVolume, LegacyVolume>> EnumLegacyVolumes();

			static bool RemoveMountPoint(const FSPath& path) noexcept;
			static bool RemoveMountPoint(const LegacyVolume& volume) noexcept;

		private:
			XChar m_Path[64 - (sizeof(size_t) / sizeof(XChar))] = {};
			size_t m_Length = 0;

		private:
			void AssignPath(const XChar* path, size_t length = String::npos)
			{
				if (length == String::npos)
				{
					length = std::char_traits<XChar>::length(path);
				}

				m_Length = std::min(length, std::size(m_Path) - 1);
				std::char_traits<XChar>::copy(m_Path, path, m_Length);
			}
			void AssignPath(const String& path)
			{
				AssignPath(path.wc_str(), path.length());
			}

		public:
			StorageVolume() noexcept = default;
			StorageVolume(StorageVolume&&) noexcept = default;
			StorageVolume(const StorageVolume&) noexcept = default;
			StorageVolume(const UniversallyUniqueID& id);
			StorageVolume(const LegacyVolume& legacyVolume);
			StorageVolume(const FSPath& path);

		public:
			bool IsNull() const noexcept;
			bool DoesExist() const noexcept;

			UniversallyUniqueID GetUniqueID() const noexcept;
			FSPath GetPath() const;
			FSPath GetDevicePath() const;

			String GetLabel() const;
			bool SetLabel(const String& label);

			DriveType GetType() const noexcept;
			uint32_t GetSerialNumber() const noexcept;
			String GetFileSystem() const noexcept;
			FlagSet<FileSystemFeature> GetFileSystemFeatures() const noexcept;

			std::optional<SpaceLayoutInfo> GetSpaceLayoutInfo() const noexcept;
			DataSize GetTotalSpace() const noexcept;
			DataSize GetUsedSpace() const noexcept;
			DataSize GetFreeSpace() const noexcept;

			LegacyVolume GetLegacyVolume() const;
			Enumerator<FSPath> EnumMountPoints() const;
			bool SetMountPoint(const FSPath& path);
			bool SetMountPoint(const LegacyVolume& volume) noexcept;

			bool EjectMedia() noexcept;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
			
			bool operator==(const StorageVolume& other) const
			{
				if (this != &other)
				{
					return m_Length == other.m_Length && std::char_traits<XChar>::compare(m_Path, other.m_Path, m_Length) == 0;
				}
				return true;
			}
			bool operator!=(const StorageVolume& other) const
			{
				return !(*this == other);
			}

			StorageVolume& operator=(StorageVolume&&) noexcept = default;
			StorageVolume& operator=(const StorageVolume&) noexcept = default;
	};
}
