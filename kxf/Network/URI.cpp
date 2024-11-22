#include "KxfPCH.h"
#include "URI.h"
#include "kxf/Core/ResourceID.h"
#include "kxf/FileSystem/FSPath.h"
#include <uriparser/Uri.h>
#include <wx/uri.h>

namespace
{
	UriTextRangeW ToTextRange(const kxf::String& value) noexcept
	{
		UriTextRangeW range = {nullptr, nullptr};

		if (!value.IsEmpty())
		{
			range.first = value.wc_str();
			range.afterLast = value.wc_str() + value.length();
		}
		return range;
	}
	kxf::StringView FromTextRange(const UriTextRangeW& range) noexcept
	{
		return kxf::StringView(range.first, range.afterLast - range.first);
	}
	bool IsTextRangeEmpty(const UriTextRangeW& range) noexcept
	{
		return FromTextRange(range).empty();
	}

	constexpr UriBreakConversion MapLineBreakFormat(kxf::LineBreakFormat format) noexcept
	{
		using kxf::LineBreakFormat;

		switch (format)
		{
			case LineBreakFormat::LF:
			{
				return UriBreakConversion::URI_BR_TO_LF;
			}
			case LineBreakFormat::CRLF:
			{
				return UriBreakConversion::URI_BR_TO_CRLF;
			}
			case LineBreakFormat::CR:
			{
				return UriBreakConversion::URI_BR_TO_CR;
			}
		};
		return UriBreakConversion::URI_BR_DONT_TOUCH;
	}
	constexpr kxf::NetworkHostType MapHostType(wxURIHostType hostType) noexcept
	{
		using kxf::NetworkHostType;

		switch (hostType)
		{
			case wxURIHostType::wxURI_REGNAME:
			{
				return NetworkHostType::RegisteredName;
			}
			case wxURIHostType::wxURI_IPV4ADDRESS:
			{
				return NetworkHostType::IPv4;
			}
			case wxURIHostType::wxURI_IPV6ADDRESS:
			{
				return NetworkHostType::IPv6;
			}
			case wxURIHostType::wxURI_IPVFUTURE:
			{
				return NetworkHostType::IPvFuture;
			}
		};
		return NetworkHostType::None;
	}
}

namespace kxf::Private
{
	class URIObject final
	{
		private:
			UriUriW m_URI;

		private:
			void Initialize() noexcept
			{
				std::memset(&m_URI, 0, sizeof(m_URI));
			}
			void Destroy() noexcept
			{
				if (m_URI.owner)
				{
					::uriFreeUriMembersW(&m_URI);
					m_URI.owner = URI_FALSE;
				}
				std::memset(&m_URI, 0, sizeof(m_URI));
			}

		public:
			URIObject() noexcept
			{
				Initialize();
			}
			URIObject(const URIObject&) = delete;
			~URIObject() noexcept
			{
				Destroy();
			}

		public:
			const UriUriW* Get() const noexcept
			{
				return &m_URI;
			}
			UriUriW* Get() noexcept
			{
				return &m_URI;
			}

		public:
			bool Create(const String& uri, bool makeOwner) noexcept
			{
				Destroy();

				if (::uriParseSingleUriW(&m_URI, uri.wc_str(), nullptr) == URI_SUCCESS)
				{
					if (makeOwner)
					{
						auto result = ::uriMakeOwnerW(&m_URI);
						return result == URI_SUCCESS || result == URI_TRUE;
					}
					return true;
				}
				return false;
			}
			String BuildURI() const
			{
				int requiredSize = 0;
				if (::uriToStringCharsRequiredW(&m_URI, &requiredSize) == URI_SUCCESS)
				{
					std::wstring buffer;
					buffer.resize(requiredSize);

					int written = 0;
					if (::uriToStringW(buffer.data(), &m_URI, requiredSize + 1, &written) == URI_SUCCESS)
					{
						return buffer;
					}
				}
				return {};
			}

			size_t GetHash() const noexcept
			{
				auto Hash = [](const auto& value)
				{
					return std::hash<StringView>()(FromTextRange(value));
				};

				size_t hash = Hash(m_URI.scheme);
				hash ^= Hash(m_URI.userInfo);
				hash ^= Hash(m_URI.hostText);
				hash ^= Hash(m_URI.portText);

				auto pathHead = m_URI.pathHead;
				while (pathHead)
				{
					hash ^= Hash(pathHead->text);
					pathHead = pathHead->next;
				}

				hash ^= Hash(m_URI.query);
				hash ^= Hash(m_URI.fragment);

				return hash;
			}
			bool IsSameAs(const URIObject& other) const noexcept
			{
				return this == &other || ::uriEqualsUriW(&m_URI, &other.m_URI);
			}

			bool Resolve(const URIObject& base, FlagSet<URIFlag> flags) noexcept
			{
				UriUriW result;
				if (::uriAddBaseUriW(&result, &m_URI, &base.m_URI) == URI_SUCCESS)
				{
					Destroy();
					m_URI = result;

					return true;
				}
				return false;
			}
			bool MakeReference(const URIObject& base, FlagSet<URIFlag> flags) noexcept
			{
				UriUriW result;
				if (::uriRemoveBaseUriW(&result, &m_URI, &base.m_URI, flags.Contains(URIFlag::DomainRootRelative)) == URI_SUCCESS)
				{
					Destroy();
					m_URI = result;

					return true;
				}
				return false;
			}
			bool Normalize()
			{
				unsigned int requiresNormalization = 0;
				if (::uriNormalizeSyntaxMaskRequiredExW(&m_URI, &requiresNormalization) == URI_SUCCESS)
				{
					return ::uriNormalizeSyntaxExW(&m_URI, requiresNormalization) == URI_SUCCESS;
				}
				return false;
			}

		public:
			bool HasScheme() const noexcept
			{
				return !IsTextRangeEmpty(m_URI.scheme);
			}
			String GetScheme() const
			{
				return FromTextRange(m_URI.scheme);
			}

			NetworkHostType GetHostType() const noexcept
			{
				if (m_URI.hostData.ip4)
				{
					return NetworkHostType::IPv4;
				}
				else if (m_URI.hostData.ip6)
				{
					return NetworkHostType::IPv6;
				}
				else if (m_URI.hostData.ipFuture.first && m_URI.hostData.ipFuture.afterLast)
				{
					return NetworkHostType::IPvFuture;
				}
				else if (m_URI.hostText.first && m_URI.hostText.afterLast)
				{
					return NetworkHostType::RegisteredName;
				}
				return NetworkHostType::None;
			}
			bool HasServer() const noexcept
			{
				return !IsTextRangeEmpty(m_URI.hostText);
			}
			String GetServer() const
			{
				return FromTextRange(m_URI.hostText);
			}

			bool HasPort() const noexcept
			{
				return !IsTextRangeEmpty(m_URI.portText);
			}
			String GetPort() const
			{
				return FromTextRange(m_URI.portText);
			}

			bool HasPath() const
			{
				return m_URI.pathHead && !IsTextRangeEmpty(m_URI.pathHead->text);
			}
			String GetPath() const
			{
				String result;

				auto pathHead = m_URI.pathHead;
				while (pathHead)
				{
					if (!result.IsEmpty())
					{
						result += '/';
					}
					result += FromTextRange(pathHead->text);

					pathHead = pathHead->next;
				}
				return result;
			}

			bool HasQuery() const noexcept
			{
				return !IsTextRangeEmpty(m_URI.query);
			}
			String GetQuery() const
			{
				return FromTextRange(m_URI.query);
			}

			bool HasFragment() const noexcept
			{
				return !IsTextRangeEmpty(m_URI.fragment);
			}
			String GetFragment() const
			{
				return FromTextRange(m_URI.fragment);
			}

			bool HasUserInfo() const noexcept
			{
				return !IsTextRangeEmpty(m_URI.userInfo);
			}
			String GetUserInfo() const
			{
				return FromTextRange(m_URI.userInfo);
			}
			String GetUser() const
			{
				auto userInfo = FromTextRange(m_URI.userInfo);

				size_t pos = userInfo.find(':');
				if (pos != StringView::npos)
				{
					return userInfo.substr(0, pos - 1);
				}
				return {};
			}
			String GetPassword() const
			{
				auto userInfo = FromTextRange(m_URI.userInfo);

				size_t pos = userInfo.find(':');
				if (pos != StringView::npos)
				{
					return userInfo.substr(pos + 1);
				}
				return {};
			}

		public:
			URIObject& operator=(const URIObject&) = delete;
	};
}

namespace kxf
{
	String URI::Escape(const String& source, FlagSet<URIFlag> flags)
	{
		auto sourceRange = ToTextRange(source);

		std::wstring buffer;
		buffer.resize(source.length() * (flags.Contains(URIFlag::NormalizeBreaks) ? 6 : 3));

		if (const wchar_t* outputTerminator = ::uriEscapeExW(sourceRange.first, sourceRange.afterLast, buffer.data(), flags.Contains(URIFlag::SpacePlus), flags.Contains(URIFlag::NormalizeBreaks)))
		{
			buffer.resize(outputTerminator - buffer.data());
			return buffer;
		}
		return {};
	}
	String URI::Unescape(const String& source, LineBreakFormat lineBreakFormat, FlagSet<URIFlag> flags)
	{
		auto buffer = source.str();
		if (const wchar_t* outputTerminator = ::uriUnescapeInPlaceExW(buffer.data(), flags.Contains(URIFlag::SpacePlus), MapLineBreakFormat(lineBreakFormat)))
		{
			buffer.resize(outputTerminator - buffer.data());
			return buffer;
		}
		return {};
	}

	URI::URI() noexcept = default;
	URI::URI(URI&&) noexcept = default;
	URI::~URI() = default;

	bool URI::IsNull() const noexcept
	{
		return m_URI == nullptr;
	}
	void URI::Clear() noexcept
	{
		m_URI = nullptr;
	}

	bool URI::Create(const String& uri)
	{
		auto uriObject = std::make_unique<Private::URIObject>();
		if (uriObject->Create(uri, true))
		{
			m_URI = std::move(uriObject);
			return true;
		}
		else
		{
			Clear();
			return false;
		}
	}
	bool URI::Create(const FSPath& path)
	{
		String pathString = path.GetFullPath();

		std::wstring buffer;
		if (path.IsAbsolute())
		{
			buffer.resize(8 + 3 * pathString.length() + 1);
		}
		else
		{
			buffer.resize(3 * pathString.length() + 1);
		}

		if (::uriWindowsFilenameToUriStringW(pathString.wc_str(), buffer.data()) == URI_SUCCESS)
		{
			return Create(String(std::move(buffer)));
		}
		return false;
	}
	bool URI::Create(const wxURI& uri)
	{
		return Create(uri.BuildURI());
	}

	bool URI::IsReference() const noexcept
	{
		return !HasScheme() || !HasServer();
	}
	URI& URI::Resolve(const URI& base, FlagSet<URIFlag> flags)
	{
		if (m_URI && base)
		{
			m_URI->Resolve(*base.m_URI, flags);
		}
		return *this;
	}
	URI& URI::MakeReference(const URI& base, FlagSet<URIFlag> flags)
	{
		if (m_URI && base)
		{
			m_URI->MakeReference(*base.m_URI, flags);
		}
		return *this;
	}
	URI& URI::Normalize() noexcept
	{
		if (m_URI)
		{
			m_URI->Normalize();
		}
		return *this;
	}

	String URI::BuildURI() const
	{
		return m_URI ? m_URI->BuildURI() : NullString;
	}
	String URI::BuildUnescapedURI(LineBreakFormat lineBreakFormat, FlagSet<URIFlag> flags) const
	{
		return m_URI ? Unescape(m_URI->BuildURI(), lineBreakFormat, flags) : NullString;
	}

	bool URI::HasScheme() const noexcept
	{
		return m_URI && m_URI->HasScheme();
	}
	String URI::GetScheme() const
	{
		return m_URI ? m_URI->GetScheme() : NullString;
	}

	NetworkHostType URI::GetHostType() const noexcept
	{
		return m_URI ? m_URI->GetHostType() : NetworkHostType::None;
	}
	bool URI::HasServer() const noexcept
	{
		return m_URI && m_URI->HasServer();
	}
	String URI::GetServer() const
	{
		return m_URI ? m_URI->GetServer() : NullString;
	}

	bool URI::HasPort() const noexcept
	{
		return m_URI && m_URI->HasPort();
	}
	std::optional<uint16_t> URI::GetPortInt() const
	{
		return GetPort().ToInteger<uint16_t>();
	}
	String URI::GetPort() const
	{
		return m_URI ? m_URI->GetPort() : NullString;
	}

	bool URI::HasPath() const noexcept
	{
		return m_URI && m_URI->HasPath();
	}
	String URI::GetPath() const
	{
		return m_URI ? m_URI->GetPath() : NullString;
	}

	bool URI::HasQuery() const noexcept
	{
		return m_URI && m_URI->HasQuery();
	}
	String URI::GetQuery() const
	{
		return m_URI ? m_URI->GetQuery() : NullString;
	}

	bool URI::HasFragment() const noexcept
	{
		return m_URI && m_URI->HasFragment();
	}
	String URI::GetFragment() const
	{
		return m_URI ? m_URI->GetFragment() : NullString;
	}

	bool URI::HasUserInfo() const noexcept
	{
		return m_URI && m_URI->HasUserInfo();
	}
	String URI::GetUserInfo() const
	{
		return m_URI ? m_URI->GetUserInfo() : NullString;
	}
	String URI::GetUser() const
	{
		return m_URI ? m_URI->GetUser() : NullString;
	}
	String URI::GetPassword() const
	{
		return m_URI ? m_URI->GetPassword() : NullString;
	}

	URI& URI::operator=(const URI& other)
	{
		Create(other.BuildURI());
		return *this;
	}
	URI& URI::operator=(URI&&) noexcept = default;

	bool URI::operator==(const URI& other) const noexcept
	{
		return m_URI == other.m_URI || (m_URI && other.m_URI && m_URI->IsSameAs(*other.m_URI));
	}
	bool URI::operator==(const wxURI& other) const
	{
		return *this == URI(other);
	}

	URI::operator wxURI() const
	{
		return wxURI(BuildURI());
	}
}

namespace std
{
	size_t std::hash<kxf::URI>::operator()(const kxf::URI& uri) const noexcept
	{
		return uri.m_URI ? uri.m_URI->GetHash() : 0;
	}
}
