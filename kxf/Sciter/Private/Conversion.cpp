#include "kxf-pch.h"
#include "Conversion.h"

namespace kxf::Sciter
{
	std::vector<uint8_t> ToSciterUTF8(const String& text)
	{
		if (!text.IsEmpty())
		{
			auto utf8 = text.ToUTF8();
			constexpr size_t bomLength = 3;

			// Copy to buffer and add BOM sequence to force Sciter to recognize the text as UTF-8
			std::vector<uint8_t> buffer;
			buffer.resize(utf8.length() + bomLength);
			buffer[0] = 0xEF;
			buffer[1] = 0xBB;
			buffer[2] = 0xBF;
			std::memcpy(buffer.data() + bomLength, utf8.data(), utf8.length());

			return buffer;
		}
		return {};
	}

	String FSPathToSciterAddress(const FSPath& path)
	{
		if (path)
		{
			return String("file://").Append(path.GetFullPath(FSPathFormat::TrailingSeparator));
		}
		else
		{
			return "about:blank";
		}
	}
	String URIToSciterAddress(const URI& uri)
	{
		if (!uri.HasScheme())
		{
			return String("file://").Append(uri.BuildURI());
		}
		return uri.BuildURI();
	}

	SC_COLOR CreateSciterColor(kxf::PackedRGBA<uint8_t> color) noexcept
	{
		return GetGrapchicsAPI()->RGBA(color.Red, color.Green, color.Blue, color.Alpha);
	}
	SC_COLOR CreateSciterColor(const Color& color) noexcept
	{
		if (color)
		{
			return CreateSciterColor(color.GetFixed8());
		}
		else
		{
			return CreateSciterColor({0, 0, 0, 255});
		}
	}
}
