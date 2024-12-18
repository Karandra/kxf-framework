#include "kxf-pch.h"
#include "Archive.h"
#include "Library.h"
#include "Common.h"
#include "Private/Utility.h"
#include "Private/GUIDs.h"
#include "Private/ArchiveOpenCallback.h"
#include "Private/ArchiveExtractCallback.h"
#include "Private/ArchiveUpdateCallback.h"
#include "Private/InStreamWrapper.h"
#include "Private/OutStreamWrapper.h"
#include "kxf/Core/ErrorCode.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/System/VariantProperty.h"
#include "kxf/FileSystem/NativeFileSystem.h"
#include "kxf/Utility/ScopeGuard.h"
#include "kxf/Utility/TypeTraits.h"
#include "kxf/Utility/Literals.h"
#include "kxf/Utility/RecursiveCollectionEnumerator.h"

namespace
{
	using namespace kxf;
	using namespace kxf::SevenZip;
	using ArchiveProperty = decltype(kpidNoProperty);

	NativeFileSystem g_NativeFS;

	String GetMethodString(CompressionMethod method) noexcept
	{
		switch (method)
		{
			case CompressionMethod::LZMA:
			{
				return "LZMA";
			}
			case CompressionMethod::BZIP2:
			{
				return "BZip2";
			}
			case CompressionMethod::PPMD:
			{
				return "PPMd";
			}
		};

		// LZMA2 is the default
		return "LZMA2";
	}
	String FormatMethodString(int dictionarySize, SevenZip::CompressionMethod method)
	{
		const int64_t dictSizeMB = static_cast<int64_t>(dictionarySize) + 20;

		switch (method)
		{
			case CompressionMethod::LZMA:
			case CompressionMethod::LZMA2:
			{
				return Format("{}:d={}", GetMethodString(method), dictSizeMB);
			}
			case CompressionMethod::BZIP2:
			{
				// Makes no sense for BZip2 as its max dictionary size is so tiny
				return "BZip2:d=900000b";
			}
			case CompressionMethod::PPMD:
			{
				return Format("{}:mem={}", GetMethodString(method), dictSizeMB);
			}
		};
		return {};
	}
	bool SetCompressionProperties(IUnknown& archive, bool multithreaded, bool solidArchive, int compressionLevel, int dictionarySize, SevenZip::CompressionMethod method)
	{
		String methodString = FormatMethodString(dictionarySize, method);
		constexpr const wchar_t* names[] = {L"x", L"s", L"mt", L"m"};
		const VariantProperty values[] =
		{
			static_cast<uint32_t>(compressionLevel),
			solidArchive,
			multithreaded,
			methodString
		};

		PROPVARIANT variantValues[std::size(values)] = {};
		for (size_t i = 0; i < std::size(values); i++)
		{
			values[i].CopyToNative(variantValues[i]);
		}

		COMPtr<ISetProperties> propertiesSet;
		archive.QueryInterface(COM::ToGUID(SevenZip::GUID::IID_ISetProperties), reinterpret_cast<void**>(&propertiesSet));
		if (!propertiesSet)
		{
			// Archive does not support setting compression properties
			return false;
		}
		return HResult(propertiesSet->SetProperties(names, variantValues, std::size(variantValues))).IsSuccess();
	}

	template<class T>
	std::optional<T> DoGetIntProperty(const IInArchive& archive, size_t fileIndex, ArchiveProperty type)
	{
		VariantProperty property;
		if (HResult(const_cast<IInArchive&>(archive).GetProperty(static_cast<uint32_t>(fileIndex), type, &property)))
		{
			return property.ToInt<T>();
		}
		return {};
	}

	FlagSet<uint32_t> GetItemAttributes(const IInArchive& archive, size_t itemCount, const UniversallyUniqueID& id) noexcept
	{
		auto luid = id.ToLocallyUniqueID();
		if (luid < itemCount)
		{
			VariantProperty property;
			if (HResult(const_cast<IInArchive&>(archive).GetProperty(static_cast<size_t>(luid.ToInt()), kpidAttrib, &property)))
			{
				return property.ToInt<uint32_t>().value_or(INVALID_FILE_ATTRIBUTES);
			}
		}
		return INVALID_FILE_ATTRIBUTES;
	}

	size_t FindItemByName(const IInArchive& stream, size_t itemCount, const FSPath& path)
	{
		for (size_t i = 0; i < itemCount; i++)
		{
			VariantProperty property;

			const_cast<IInArchive&>(stream).GetProperty(i, kpidPath, &property);
			if (auto itemPath = property.ToString())
			{
				if (path.IsSameAs(*itemPath))
				{
					return i;
				}
			}
		}
		return std::numeric_limits<size_t>::max();
	}
}
namespace
{
	class ArchiveDirectoryEnumerator final: public RecursiveCollectionEnumerator<FileItem, FSPath>
	{
		private:
			const Archive& m_Archive;
			Enumerator<FileItem> m_Items;
			Enumerator<FileItem>::iterator m_ItemsIterator;

			FSPath m_Query;
			FlagSet<FSActionFlag> m_Flags;
			FlagSet<StringActionFlag> m_MatchFlags;

		private:
			std::optional<FileItem> DoItem(IEnumerator& enumerator, FileItem item, const FSPath& directory, std::vector<FSPath>& childDirectories)
			{
				FSPath fullPath = item.GetFullPath();
				FSPath containingDirectory = fullPath.GetBefore(directory);
				if (containingDirectory == directory)
				{
					FSPath relativePath = fullPath.GetAfter(directory);
					bool hasChildItems = relativePath.GetComponentCount() > 1;

					if (hasChildItems && m_Flags.Contains(FSActionFlag::Recursive))
					{
						childDirectories.emplace_back(std::move(fullPath));
					}
					if (relativePath.MatchesWildcards(m_Query, m_MatchFlags))
					{
						return item;
					}
				}

				enumerator.SkipCurrent();
				return {};
			};

		protected:
			std::optional<FileItem> SearchDirectory(IEnumerator& enumerator, const FSPath& directory, std::vector<FSPath>& childDirectories, bool& isSubTreeDone) override
			{
				if (directory)
				{
					if (!m_Items)
					{
						m_Items = m_Archive.EnumItems(std::numeric_limits<UniversallyUniqueID>::max(), m_Flags);
						m_ItemsIterator = m_Items.begin();

						if (m_Items && m_ItemsIterator != m_Items.end())
						{
							return DoItem(enumerator, std::move(*m_ItemsIterator), directory, childDirectories);
						}
					}
					else if (++m_ItemsIterator; m_ItemsIterator != m_Items.end())
					{
						return DoItem(enumerator, std::move(*m_ItemsIterator), directory, childDirectories);
					}
					else
					{
						m_Items = {};
						m_ItemsIterator = {};
						isSubTreeDone = true;

						// Skip this step if we need to scan subdirectories because otherwise we'd terminate the process
						if (m_Flags.Contains(FSActionFlag::Recursive))
						{
							enumerator.SkipCurrent();
						}
					}
				}
				return {};
			};

		public:
			ArchiveDirectoryEnumerator() = default;
			ArchiveDirectoryEnumerator(const Archive& archive, FSPath rootPath, FSPath query, FlagSet<FSActionFlag> flags)
				:RecursiveCollectionEnumerator(std::move(rootPath)), m_Archive(archive), m_Query(std::move(query)), m_Flags(flags)
			{
				m_MatchFlags.Add(StringActionFlag::IgnoreCase, !flags.Contains(FSActionFlag::CaseSensitive));
			}
	};
}

namespace kxf::SevenZip
{
	void Archive::InvalidateCache()
	{
		m_Data.IsLoaded = false;
	}
	bool Archive::InitCompressionFormat()
	{
		m_Data.OverrideCompressionFormat = false;
		m_Data.Properties.CompressionFormat = Private::IdentifyCompressionFormat(*m_Data.Stream, {}, m_EvtHandler.Get());

		return m_Data.Properties.CompressionFormat != CompressionFormat::Unknown;
	}
	bool Archive::InitMetadata()
	{
		if (!m_Data.IsLoaded)
		{
			RewindArchiveStreams();
			return !m_Data.OverrideCompressionFormat ? InitCompressionFormat() : true;
		}
		return m_Data.IsLoaded;
	}
	bool Archive::InitArchiveStreams()
	{
		m_Data.ItemCount = 0;
		m_Data.InArchive = Private::GetArchiveReader(m_Data.Properties.CompressionFormat);

		if (m_Data.InArchive)
		{
			RewindArchiveStreams();

			auto openCallback = COM::CreateLocalInstance<Private::Callback::OpenArchive>(m_EvtHandler.Get());
			auto streamWrapper = COM::CreateLocalInstance<Private::InStreamWrapper_IInputStream>(*m_Data.Stream, nullptr);
			if (HResult(m_Data.InArchive->Open(streamWrapper, nullptr, openCallback)))
			{
				if (auto count = Private::GetNumberOfItems(*m_Data.InArchive))
				{
					m_Data.ItemCount = *count;
					return true;
				}
			}
		}
		return false;
	}
	void Archive::RewindArchiveStreams()
	{
		if (m_Data.Stream)
		{
			m_Data.Stream->SeekI(0, IOStreamSeek::FromStart);
		}
	}

	bool Archive::DoOpen(InputStreamDelegate stream)
	{
		DoClose();
		m_Data.Stream = std::move(stream);
		m_Data.IsLoaded = m_Data.Stream && InitMetadata() && InitArchiveStreams();

		return m_Data.IsLoaded;
	}
	void Archive::DoClose()
	{
		if (m_Data.InArchive)
		{
			m_Data.InArchive->Close();
		}
		m_Data = {};
	}
	bool Archive::DoExtract(COMPtr<Private::Callback::ExtractArchive> extractor, Compression::FileIndexView* files) const
	{
		if (files && files->empty())
		{
			return false;
		}

		if (m_Data.Stream)
		{
			m_Data.Stream->SeekI(0, IOStreamSeek::FromStart);
			extractor->SetArchive(m_Data.InArchive);
			extractor->SetEvtHandler(m_EvtHandler.Get());

			HResult result = HResult::Fail();
			if (files)
			{
				auto IsInvalidIndex = [this](size_t index)
				{
					return index >= m_Data.ItemCount;
				};

				// Process only specified files
				if (files->size() == 1)
				{
					// No need to sort single index
					if (IsInvalidIndex(files->front()))
					{
						result = HResult::InvalidArgument();
						return false;
					}

					uint32_t index = files->front();
					result = m_Data.InArchive->Extract(&index, 1, false, extractor);
				}
				else
				{
					auto temp = files->ToVector<uint32_t>();

					// Remove invalid items
					temp.erase(std::remove_if(temp.begin(), temp.end(), IsInvalidIndex), temp.end());

					// IInArchive::Extract requires sorted array
					std::sort(temp.begin(), temp.end());

					result = HResult::InvalidArgument();
					if (!temp.empty())
					{
						result = m_Data.InArchive->Extract(temp.data(), static_cast<uint32_t>(temp.size()), false, extractor);
					}
				}
			}
			else
			{
				// Process all files, the callback will decide which files are needed
				result = m_Data.InArchive->Extract(nullptr, std::numeric_limits<uint32_t>::max(), false, extractor);
			}
			return result.IsSuccess();
		}
		return false;
	}
	bool Archive::DoUpdate(IOutputStream& stream, COMPtr<Private::Callback::UpdateArchive> updater, size_t itemCount)
	{
		auto archiveWriter = Private::GetArchiveWriter(m_Data.Properties.CompressionFormat);
		if (archiveWriter)
		{
			SetCompressionProperties(*archiveWriter,
									 m_Data.Properties.MultiThreaded,
									 m_Data.Properties.Solid,
									 static_cast<int>(m_Data.Properties.CompressionLevel),
									 m_Data.Properties.DictionarySize,
									 m_Data.Properties.CompressionMethod);

			updater->SetArchive(m_Data.InArchive);
			updater->SetEvtHandler(m_EvtHandler.Get());

			auto streamWrapper = COM::CreateLocalInstance<Private::OutStreamWrapper_IOutputStream>(stream, nullptr);
			return HResult(archiveWriter->UpdateItems(streamWrapper, static_cast<uint32_t>(itemCount), updater)).IsSuccess();
		}
		return false;
	}

	Archive::Archive() = default;
	Archive::~Archive()
	{
		Close();
	}

	// IArchive
	FileItem Archive::GetItem(size_t index) const
	{
		if (index < m_Data.ItemCount)
		{
			return Private::GetArchiveItem(*m_Data.InArchive, index);
		}
		return {};
	}
	DataSize Archive::GetOriginalSize() const
	{
		if (m_Data.OriginalSize < 0)
		{
			int64_t total = 0;
			for (size_t i = 0; i < m_Data.ItemCount; i++)
			{
				total += DoGetIntProperty<int64_t>(*m_Data.InArchive, i, ArchiveProperty::kpidSize).value_or(0);
			}
			m_Data.OriginalSize = total;
		}
		return m_Data.OriginalSize;
	}
	DataSize Archive::GetCompressedSize() const
	{
		if (m_Data.CompressedSize < 0)
		{
			int64_t total = 0;
			for (size_t i = 0; i < m_Data.ItemCount; i++)
			{
				total += DoGetIntProperty<int64_t>(*m_Data.InArchive, i, ArchiveProperty::kpidPackSize).value_or(0);
			}
			m_Data.CompressedSize = total;
		}
		return m_Data.CompressedSize;
	}

	// IArchiveExtraction
	bool Archive::Extract(Compression::IExtractCallback& callback) const
	{
		return DoExtract(COM::CreateLocalInstance<Private::Callback::ExtractArchiveWrapper>(*this, callback), nullptr);
	}
	bool Archive::Extract(Compression::IExtractCallback& callback, Compression::FileIndexView files) const
	{
		return DoExtract(COM::CreateLocalInstance<Private::Callback::ExtractArchiveWrapper>(*this, callback), &files);
	}

	bool Archive::ExtractToFS(IFileSystem& fileSystem, const FSPath& directory) const
	{
		return DoExtract(COM::CreateLocalInstance<Private::Callback::ExtractArchiveToFS>(*this, fileSystem, directory), nullptr);
	}
	bool Archive::ExtractToFS(IFileSystem& fileSystem, const FSPath& directory, Compression::FileIndexView files) const
	{
		return DoExtract(COM::CreateLocalInstance<Private::Callback::ExtractArchiveToFS>(*this, fileSystem, directory), &files);
	}

	bool Archive::ExtractToStream(size_t index, IOutputStream& stream) const
	{
		Compression::FileIndexView files = index;
		return DoExtract(COM::CreateLocalInstance<Private::Callback::ExtractArchiveToStream>(*this, stream), &files);
	}

	// IArchiveCompression
	bool Archive::Update(IOutputStream& stream, Compression::IUpdateCallback& callback, size_t itemCount)
	{
		return DoUpdate(stream, COM::CreateLocalInstance<Private::Callback::UpdateArchiveWrapper>(*this, callback), itemCount);
	}
	bool Archive::UpdateFromFS(IOutputStream& stream, const IFileSystem& fileSystem, const FSPath& directory, const FSPath& query, FlagSet<FSActionFlag> flags)
	{
		std::vector<FileItem> files;
		for (FileItem& item: fileSystem.EnumItems(directory, query, flags.Remove(FSActionFlag::LimitToDirectories).Add(FSActionFlag::LimitToFiles)))
		{
			files.emplace_back(std::move(item));
		}

		if (!files.empty())
		{
			const size_t count = files.size();
			return DoUpdate(stream, COM::CreateLocalInstance<Private::Callback::UpdateArchiveFromFS>(*this, fileSystem, std::move(files), directory), count);
		}
		return false;
	}

	// IFileSystem
	bool Archive::IsValidPathName(const FSPath& path) const
	{
		return NativeFileSystem().IsValidPathName(path);
	}
	String Archive::GetForbiddenPathNameCharacters(const String& except) const
	{
		return NativeFileSystem().GetForbiddenPathNameCharacters(except);
	}

	FileItem Archive::GetItem(const FSPath& path) const
	{
		size_t index = FindItemByName(*m_Data.InArchive, m_Data.ItemCount, path);
		if (index != std::numeric_limits<size_t>::max())
		{
			return Private::GetArchiveItem(*m_Data.InArchive, index);
		}
		return {};
	}
	Enumerator<FileItem> Archive::EnumItems(const FSPath& directory, const FSPath& query, FlagSet<FSActionFlag> flags) const
	{
		if (IsOpened())
		{
			return ArchiveDirectoryEnumerator(*this, directory, query, flags);
		}
		return {};
	}

	// IFileIDSystem
	FileItem Archive::GetItem(const UniversallyUniqueID& id) const
	{
		auto luid = id.ToLocallyUniqueID();
		if (size_t index = static_cast<size_t>(luid.ToInt()); index < m_Data.ItemCount)
		{
			return Private::GetArchiveItem(*m_Data.InArchive, index);
		}
		return {};
	}
	Enumerator<FileItem> Archive::EnumItems(const UniversallyUniqueID& id, FlagSet<FSActionFlag> flags) const
	{
		if (IsOpened())
		{
			if (id == std::numeric_limits<UniversallyUniqueID>::max())
			{
				return {[this, flags, index = 0_uz] (IEnumerator& enumerator) mutable -> std::optional<FileItem>
				{
					const size_t fileIndex = index++;
					const auto attributes = GetItemAttributes(*m_Data.InArchive, m_Data.ItemCount, LocallyUniqueID(fileIndex));
					const bool isDirectory = attributes.Contains(FILE_ATTRIBUTE_DIRECTORY);

					if ((flags & FSActionFlag::LimitToFiles && isDirectory) || (flags & FSActionFlag::LimitToDirectories && !isDirectory))
					{
						enumerator.SkipCurrent();
						return {};
					}
					else
					{
						return Private::GetArchiveItem(*m_Data.InArchive, fileIndex);
					}
				}, m_Data.ItemCount};
			}
			else if (auto luid = id.ToLocallyUniqueID(); luid < m_Data.ItemCount)
			{
				if (FileItem item = Private::GetArchiveItem(*m_Data.InArchive, luid.ToInt()))
				{
					return EnumItems(item.GetFullPath(), {}, flags);
				}
			}
		}
		return {};
	}

	Archive& Archive::operator=(Archive&& other)
	{
		m_EvtHandler = std::move(other.m_EvtHandler);
		m_Data = std::move(other.m_Data);

		return *this;
	}
}
