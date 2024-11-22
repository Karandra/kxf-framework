#pragma once
#include "../Common.h"
#include "kxf/IO/IStream.h"
#include "kxf/Core/ILibraryInfo.h"
#include <nlohmann/json.hpp>

namespace kxf
{
	class JSONDocument final: public nlohmann::json, public ILibraryInfo
	{
		private:
			nlohmann::json& AsBase() noexcept
			{
				return static_cast<nlohmann::json&>(*this);
			}
			const nlohmann::json& AsBase() const noexcept
			{
				return static_cast<const nlohmann::json&>(*this);
			}

		public:
			using nlohmann::json::json;
			JSONDocument(const String& json)
			{
				Load(json);
			}
			JSONDocument(IInputStream& stream)
			{
				Load(stream);
			}

		public:
			String Save() const;
			bool Save(IOutputStream& stream) const;

			bool Load(const String& json);
			bool Load(IInputStream& stream);

		public:
			// ILibraryInfo
			String GetName() const override;
			Version GetVersion() const override;
			URI GetHomePage() const override;
			uint32_t GetAPILevel() const override;

			String GetLicense() const override;
			String GetLicenseName() const override;
			String GetCopyright() const override;
	};
}

namespace nlohmann
{
	template<>
	struct adl_serializer<kxf::String> final
	{
		static void to_json(json& jsonDocument, const kxf::String& value)
		{
			auto utf8 = value.ToUTF8();
			jsonDocument = std::string_view(utf8.data(), utf8.length());
		}
		static void from_json(const json& jsonDocument, kxf::String& value)
		{
			if (jsonDocument.is_null())
			{
				value.clear();
			}
			else
			{
				const json::string_t& string = jsonDocument.get_ref<const json::string_t&>();
				value = kxf::String::FromUTF8({string.data(), string.length()});
			}
		}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<JSONDocument> final
	{
		uint64_t Serialize(IOutputStream& stream, const JSONDocument& value) const
		{
			return BinarySerializer<String>().Serialize(stream, value.Save());
		}
		uint64_t Deserialize(IInputStream& stream, JSONDocument& value) const
		{
			String buffer;
			auto read = BinarySerializer<String>().Deserialize(stream, buffer);

			value.Load(buffer);
			return read;
		}
	};
}
