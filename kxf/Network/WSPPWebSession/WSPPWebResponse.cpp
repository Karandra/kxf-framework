#include "kxf-pch.h"
#include "WSPPWebResponse.h"
#include "WSPPWebRequest.h"
#include "kxf/IO/IStream.h"

#include "kxf/System/HandlePtr.h"
#include "kxf/Utility/Container.h"
#include "kxf/Utility/Enumerator.h"

namespace kxf
{
	auto WSPPWebResponse::GetClient() const noexcept -> TClient&
	{
		return m_Request.m_Client;
	}
	auto WSPPWebResponse::GetConnection() const noexcept -> TConnection*
	{
		return m_Request.m_Connection.get();
	}

	URI WSPPWebResponse::GetURI() const
	{
		if (auto connection = GetConnection())
		{
			return String::FromUTF8(connection->get_uri()->str());
		}
		return {};
	}
	String WSPPWebResponse::GetMethod() const
	{
		if (auto connection = GetConnection())
		{
			return String::FromUTF8(connection->get_request().get_method());
		}
		return {};
	}
	String WSPPWebResponse::GetPrimaryIP() const
	{
		return {};
	}
	std::optional<uint16_t> WSPPWebResponse::GetPrimaryPort() const
	{
		if (auto connection = GetConnection())
		{
			return connection->get_port();
		}
		return {};
	}
	WebRequestProtocol WSPPWebResponse::GetProtocol() const
	{
		if (auto connection = GetConnection())
		{
			return connection->is_secure() ? WebRequestProtocol::WSS : WebRequestProtocol::WS;
		}
		return WebRequestProtocol::None;
	}
	WebRequestHTTPVersion WSPPWebResponse::GetHTTPVersion() const
	{
		return WebRequestHTTPVersion::None;
	}
	DataSize WSPPWebResponse::GetContentLength() const
	{
		if (auto value = GetHeader("Content-Length").ToInteger<int64_t>())
		{
			return DataSize::FromBytes(*value);
		}
		return {};
	}
	String WSPPWebResponse::GetContentType() const
	{
		return GetHeader("Content-Type");
	}

	String WSPPWebResponse::GetHeader(const String& name) const
	{
		auto it = Utility::Container::FindIf(m_Request.m_ResponseHeaders, [&](const WebRequestHeader& header)
		{
			return header.GetName() == name;
		});
		if (it != m_Request.m_ResponseHeaders.end())
		{
			return it->GetValue();
		}
		return {};
	}
	Enumerator<WebRequestHeader> WSPPWebResponse::EnumHeaders() const
	{
		return Utility::EnumerateIterableContainer<WebRequestHeader>(m_Request.m_ResponseHeaders);
	}
	Enumerator<String> WSPPWebResponse::EnumCookies() const
	{
		return {};
	}

	std::unique_ptr<IInputStream> WSPPWebResponse::GetStream() const
	{
		if (m_Request.m_ReceiveStream)
		{
			switch (m_Request.m_ReceiveStorage)
			{
				case WebRequestStorage::Memory:
				case WebRequestStorage::Stream:
				case WebRequestStorage::FileSystem:
				{
					if (auto readableStream = m_Request.m_ReceiveStream->QueryInterface<IReadableOutputStream>())
					{
						return readableStream->CreateInputStream();
					}
					
					break;
				}
			};
		}
		return nullptr;
	}
}
