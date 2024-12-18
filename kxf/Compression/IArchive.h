#pragma once
#include "Common.h"
#include "ArchiveEvent.h"
#include "IArchiveCallbacks.h"
#include "kxf/EventSystem/Event.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class KXF_API IArchive: public RTTI::Interface<IArchive>
	{
		kxf_RTTI_DeclareIID(IArchive, {0xb4327a42, 0x17a7, 0x44db, {0x84, 0xb, 0xc3, 0x24, 0x5b, 0x29, 0xca, 0xe8}});

		public:
			kxf_EVENT_MEMBER(ArchiveEvent, OpenBytes);
			kxf_EVENT_MEMBER(ArchiveEvent, OpenItems);
			kxf_EVENT_MEMBER(ArchiveEvent, Password);

		public:
			virtual ~IArchive() = default;

		public:
			virtual bool IsOpened() const = 0;
			virtual bool Open(InputStreamDelegate stream) = 0;
			virtual void Close() = 0;

			virtual size_t GetItemCount() const = 0;
			virtual FileItem GetItem(size_t index) const = 0;

			virtual DataSize GetOriginalSize() const = 0;
			virtual DataSize GetCompressedSize() const = 0;
			double GetCompressionRatio() const
			{
				return GetSizeRatio(GetCompressedSize(), GetOriginalSize());
			}

		public:
			explicit operator bool() const
			{
				return IsOpened();
			}
			bool operator!() const
			{
				return !IsOpened();
			}
	};
}

namespace kxf
{
	class KXF_API IArchiveExtract: public RTTI::Interface<IArchiveExtract>
	{
		kxf_RTTI_DeclareIID(IArchiveExtract, {0x105f744b, 0x904d, 0x4822, {0xb4, 0x7a, 0x57, 0x8b, 0x3e, 0xd, 0x95, 0xe6}});

		public:
			kxf_EVENT_MEMBER(ArchiveEvent, Item);
			kxf_EVENT_MEMBER(ArchiveEvent, ItemDone);
			kxf_EVENT_MEMBER(ArchiveEvent, WriteStream);
			kxf_EVENT_MEMBER(ArchiveEvent, Password);

		public:
			virtual ~IArchiveExtract() = default;

		public:
			// Extracts files using provided callback interface
			virtual bool Extract(Compression::IExtractCallback& callback) const = 0;
			virtual bool Extract(Compression::IExtractCallback& callback, Compression::FileIndexView files) const = 0;

			// Extract entire archive or only specified files into a directory
			virtual bool ExtractToFS(IFileSystem& fileSystem, const FSPath& directory) const = 0;
			virtual bool ExtractToFS(IFileSystem& fileSystem, const FSPath& directory, Compression::FileIndexView files) const = 0;
			
			// Extract specified file into a stream
			virtual bool ExtractToStream(size_t index, IOutputStream& stream) const = 0;

			template<class TOutStream = IOutputStream>
			Compression::ExtractWithOptions<TOutStream> ExtractWith() const
			{
				return *this;
			}
	};
}

namespace kxf
{
	class KXF_API IArchiveUpdate: public RTTI::Interface<IArchiveUpdate>
	{
		kxf_RTTI_DeclareIID(IArchiveUpdate, {0xcf9bb9ac, 0x6519, 0x49d4, {0xa3, 0xb4, 0xcd, 0x63, 0x17, 0x52, 0xe1, 0x55}});

		public:
			kxf_EVENT_MEMBER(ArchiveEvent, Item);
			kxf_EVENT_MEMBER(ArchiveEvent, ItemDone);
			kxf_EVENT_MEMBER(ArchiveEvent, ReadStream);
			kxf_EVENT_MEMBER(ArchiveEvent, Password);

		public:
			virtual ~IArchiveUpdate() = default;

		public:
			// Add files using provided callback interface
			virtual bool Update(IOutputStream& stream, Compression::IUpdateCallback& callback, size_t itemCount) = 0;

			// Add files from the provided file system
			virtual bool UpdateFromFS(IOutputStream& stream, const IFileSystem& fileSystem, const FSPath& directory, const FSPath& query = {}, FlagSet<FSActionFlag> flags = {}) = 0;
	};
}

namespace kxf
{
	class KXF_API IArchiveProperties: public RTTI::Interface<IArchiveProperties>
	{
		kxf_RTTI_DeclareIID(IArchiveProperties, {0x8ecede61, 0x7542, 0x4164, {0x99, 0x7c, 0xd6, 0x72, 0x57, 0x24, 0x94, 0x26}});

		public:
			virtual ~IArchiveProperties() = default;

		public:
			virtual std::optional<bool> GetPropertyBool(StringView property) const = 0;
			virtual bool SetPropertyBool(StringView property, bool value) = 0;

			virtual std::optional<int64_t> GetPropertyInt(StringView property) const = 0;
			virtual bool SetPropertyInt(StringView property, int64_t value) = 0;

			virtual std::optional<double> GetPropertyFloat(StringView property) const = 0;
			virtual bool SetPropertyFloat(StringView property, double value) = 0;

			virtual std::optional<String> GetPropertyString(StringView property) const = 0;
			virtual bool SetPropertyString(StringView property, StringView value) = 0;
	};
}

namespace kxf::Compression
{
	#define kxf_Compression_DeclareUserProperty(section, name)	constexpr XChar section##_##name[] = "User/" wxS(#section) "/" wxS(#name);

	namespace Property
	{
		#define kxf_Compression_DeclareBaseProperty(section, name) constexpr XChar section##_##name[] = "Archive/" wxS(#section) "/" wxS(#name);

		kxf_Compression_DeclareBaseProperty(Common, FilePath);
		kxf_Compression_DeclareBaseProperty(Common, ItemCount);
		kxf_Compression_DeclareBaseProperty(Common, OriginalSize);
		kxf_Compression_DeclareBaseProperty(Common, CompressedSize);

		kxf_Compression_DeclareBaseProperty(Compression, Format);
		kxf_Compression_DeclareBaseProperty(Compression, Method);
		kxf_Compression_DeclareBaseProperty(Compression, Level);
		kxf_Compression_DeclareBaseProperty(Compression, Solid);
		kxf_Compression_DeclareBaseProperty(Compression, MultiThreaded);
		kxf_Compression_DeclareBaseProperty(Compression, DictionarySize);

		#undef Kx_Compression_DeclareBaseProperty
	}
}
