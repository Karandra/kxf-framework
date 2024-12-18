#pragma once
#include "Common.h"
#include "Options.h"

namespace kxf
{
	class WebRequestHeader;
	class JSONDocument;
	class XMLDocument;
}

namespace kxf
{
	class IWebResponse: public RTTI::Interface<IWebResponse>
	{
		kxf_RTTI_DeclareIID(IWebResponse, {0x24f29008, 0x76c7, 0x42e7, {0x9f, 0x6b, 0x3, 0xcc, 0x4, 0xf3, 0xda, 0x33}});

		public:
			virtual URI GetURI() const = 0;
			virtual String GetMethod() const = 0;
			virtual String GetPrimaryIP() const = 0;
			virtual std::optional<uint16_t> GetPrimaryPort() const = 0;
			virtual WebRequestHTTPVersion GetHTTPVersion() const = 0;
			virtual WebRequestProtocol GetProtocol() const = 0;
			virtual DataSize GetContentLength() const = 0;
			virtual String GetContentType() const = 0;

			virtual std::optional<int> GetStatusCode() const = 0;
			virtual String GetStatusText() const = 0;

			virtual String GetHeader(const String& name) const = 0;
			virtual Enumerator<WebRequestHeader> EnumHeaders() const = 0;
			virtual Enumerator<String> EnumCookies() const = 0;

			virtual FSPath GetSuggestedFilePath() const;
			virtual std::unique_ptr<IInputStream> GetStream() const = 0;

			virtual String GetAsString() const;
			virtual JSONDocument GetAsJSON() const;
			virtual XMLDocument GetAsXML() const;
	};
}

namespace kxf
{
	class NullWebResponse final: public IWebResponse
	{
		public:
			static IWebResponse& Get();

		public:
			URI GetURI() const override
			{
				return {};
			}
			String GetMethod() const override
			{
				return {};
			}
			String GetPrimaryIP() const override
			{
				return {};
			}
			std::optional<uint16_t> GetPrimaryPort() const override
			{
				return {};
			}
			WebRequestProtocol GetProtocol() const override
			{
				return WebRequestProtocol::None;
			}
			WebRequestHTTPVersion GetHTTPVersion() const override
			{
				return WebRequestHTTPVersion::None;
			}
			DataSize GetContentLength() const override
			{
				return {};
			}
			String GetContentType() const override
			{
				return {};
			}

			std::optional<int> GetStatusCode() const override
			{
				return {};
			}
			String GetStatusText() const override
			{
				return {};
			}

			String GetHeader(const String& name) const override
			{
				return {};
			}
			Enumerator<WebRequestHeader> EnumHeaders() const override;
			Enumerator<String> EnumCookies() const override;

			FSPath GetSuggestedFilePath() const override;
			std::unique_ptr<IInputStream> GetStream() const override;
	};
}
