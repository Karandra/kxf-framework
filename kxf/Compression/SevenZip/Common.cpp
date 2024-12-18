#include "kxf-pch.h"
#include "Common.h"
#include "Library.h"
#include "kxf/System/ExecutableVersionResource.h"
#include "Private/GUIDs.h"

namespace kxf::SevenZip
{
	String GetLibraryName()
	{
		return "7-Zip";
	}
	Version GetLibraryVersion()
	{
		Library& library = Library::GetInstance();
		if (library.IsLoaded())
		{
			ExecutableVersionResource versionResource(library.GetLibrary().GetFilePath());
			if (versionResource)
			{
				return versionResource.GetAnyVersion();
			}
		}
		return {};
	}

	NativeUUID GetAlgorithmID(CompressionFormat format) noexcept
	{
		switch (format)
		{
			case CompressionFormat::SevenZip:
			{
				return GUID::CLSID_CFormat7z;
			}
			case CompressionFormat::Zip:
			{
				return GUID::CLSID_CFormatZip;
			}
			case CompressionFormat::GZip:
			{
				return GUID::CLSID_CFormatGZip;
			}
			case CompressionFormat::BZip2:
			{
				return GUID::CLSID_CFormatBZip2;
			}
			case CompressionFormat::Rar:
			{
				return GUID::CLSID_CFormatRar;
			}
			case CompressionFormat::Rar5:
			{
				return GUID::CLSID_CFormatRar5;
			}
			case CompressionFormat::Tar:
			{
				return GUID::CLSID_CFormatTar;
			}
			case CompressionFormat::Iso:
			{
				return GUID::CLSID_CFormatIso;
			}
			case CompressionFormat::Cab:
			{
				return GUID::CLSID_CFormatCab;
			}
			case CompressionFormat::Lzma:
			{
				return GUID::CLSID_CFormatLzma;
			}
			case CompressionFormat::Lzma86:
			{
				return GUID::CLSID_CFormatLzma86;
			}
		}
		return {};
	}
	String GetNameByFormat(CompressionFormat format)
	{
		switch (format)
		{
			case CompressionFormat::SevenZip:
			{
				return "7-Zip";
			}
			case CompressionFormat::Zip:
			{
				return "ZIP";
			}
			case CompressionFormat::Rar:
			{
				return "RAR";
			}
			case CompressionFormat::Rar5:
			{
				return "RAR5";
			}
			case CompressionFormat::GZip:
			{
				return "GZip";
			}
			case CompressionFormat::BZip2:
			{
				return "BZip2";
			}
			case CompressionFormat::Tar:
			{
				return "TAR";
			}
			case CompressionFormat::Lzma:
			{
				return "LZMA";
			}
			case CompressionFormat::Lzma86:
			{
				return "LZMA 86";
			}
			case CompressionFormat::Cab:
			{
				return "CAB";
			}
			case CompressionFormat::Iso:
			{
				return "ISO";
			}
		}
		return {};
	}
	String GetExtensionByFormat(CompressionFormat format)
	{
		switch (format)
		{
			case CompressionFormat::SevenZip:
			{
				return "7z";
			}
			case CompressionFormat::Zip:
			{
				return "zip";
			}
			case CompressionFormat::Rar:
			case CompressionFormat::Rar5:
			{
				return "rar";
			}
			case CompressionFormat::GZip:
			{
				return "gz";
			}
			case CompressionFormat::BZip2:
			{
				return "bz2";
			}
			case CompressionFormat::Tar:
			{
				return "tar";
			}
			case CompressionFormat::Lzma:
			{
				return "lzma";
			}
			case CompressionFormat::Lzma86:
			{
				return "lzma86";
			}
			case CompressionFormat::Cab:
			{
				return "cab";
			}
			case CompressionFormat::Iso:
			{
				return "iso";
			}
		}
		return {};
	}
	CompressionFormat GetFormatByExtension(const String& extension)
	{
		auto Test = [&](const char* ext)
		{
			return extension.IsSameAs(ext, StringActionFlag::IgnoreCase);
		};

		if (Test("7z"))
		{
			return CompressionFormat::SevenZip;
		}
		if (Test("zip"))
		{
			return CompressionFormat::Zip;
		}
		if (Test("rar"))
		{
			return CompressionFormat::Rar;
		}
		if (Test("rar5"))
		{
			return CompressionFormat::Rar5;
		}
		if (Test("gz") || Test("gzip"))
		{
			return CompressionFormat::GZip;
		}
		if (Test("bz2") || Test("bzip2"))
		{
			return CompressionFormat::BZip2;
		}
		if (Test("tar"))
		{
			return CompressionFormat::Tar;
		}
		if (Test("lz") || Test("lzma"))
		{
			return CompressionFormat::Lzma;
		}
		if (Test("lz86") || Test("lzma86"))
		{
			return CompressionFormat::Lzma86;
		}
		if (Test("cab"))
		{
			return CompressionFormat::Cab;
		}
		if (Test("iso"))
		{
			return CompressionFormat::Iso;
		}

		return CompressionFormat::Unknown;
	}
}
