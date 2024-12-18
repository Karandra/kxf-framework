#include "kxf-pch.h"
#include "JSONDocument.h"
#include "kxf/Network/URI.h"
#include "kxf/IO/StreamReaderWriter.h"
#include "kxf/Utility/SoftwareLicenseDB.h"
#include <wx/string.h>

namespace
{
	constexpr char g_Copyright[] = "Copyright© 2013-2023 Niels Lohmann";
}

namespace kxf
{
	String JSONDocument::Save() const
	{
		try
		{
			return String::FromUTF8(AsBase().dump(1, '\t'));
		}
		catch (...)
		{
			return {};
		}
	}
	bool JSONDocument::Save(IOutputStream& stream) const
	{
		try
		{
			std::string string = AsBase().dump(1, '\t');
			return stream.WriteAll(string.data(), string.length());
		}
		catch (...)
		{
			return false;
		}
	}

	bool JSONDocument::Load(const String& json)
	{
		try
		{
			AsBase() = nlohmann::json::parse(json.ToUTF8(), nullptr, false);
			return this->empty();
		}
		catch (...)
		{
			this->clear();
			return false;
		}
	}
	bool JSONDocument::Load(IInputStream& stream)
	{
		if (auto size = stream.GetSize())
		{
			try
			{
				IO::InputStreamReader reader(stream);

				AsBase() = nlohmann::json::parse(reader.ReadStdString(size.ToBytes()), nullptr, false);
				return this->empty();
			}
			catch (...)
			{
				this->clear();
			}
		}
		else
		{
			this->clear();
		}
		return false;
	}

	// ILibraryInfo
	String JSONDocument::GetName() const
	{
		return "JSON for Modern C++";
	}
	Version JSONDocument::GetVersion() const
	{
		return {NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH};
	}
	URI JSONDocument::GetHomePage() const
	{
		return "https://github.com/nlohmann/json";
	}
	uint32_t JSONDocument::GetAPILevel() const
	{
		return NLOHMANN_JSON_VERSION_MAJOR * 1000 + NLOHMANN_JSON_VERSION_MINOR * 100 + NLOHMANN_JSON_VERSION_PATCH * 10;
	}

	String JSONDocument::GetLicense() const
	{
		return SoftwareLicenseDB::Get().GetText(SoftwareLicenseType::MIT, g_Copyright);
	}
	String JSONDocument::GetLicenseName() const
	{
		return SoftwareLicenseDB::Get().GetName(SoftwareLicenseType::MIT);
	}
	String JSONDocument::GetCopyright() const
	{
		return g_Copyright;
	}
}
