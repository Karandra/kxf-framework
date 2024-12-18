#pragma once
#include "Common.h"
class wxURI;

namespace kxf
{
	class FSPath;

	namespace Private
	{
		class URIObject;
	}
}

namespace kxf
{
	enum class URIFlag: uint32_t
	{
		None = 0,

		SpacePlus = 1 << 0,
		NormalizeBreaks = 1 << 1,
		DomainRootRelative = 1 << 2
	};
	kxf_FlagSet_Declare(URIFlag);
}

namespace kxf
{
	class KXF_API URI final
	{
		friend class Private::URIObject;
		friend struct std::hash<URI>;

		public:
			static String Escape(const String& source, FlagSet<URIFlag> flags = {});
			static String Unescape(const String& source, LineBreakFormat lineBreakFormat = LineBreakFormat::None, FlagSet<URIFlag> flags = {});

		private:
			std::unique_ptr<Private::URIObject> m_URI;

		public:
			URI() noexcept;
			URI(const String& uri)
				:URI()
			{
				Create(uri);
			}
			URI(const FSPath& path)
				:URI()
			{
				Create(path);
			}
			URI(const wxURI& uri)
				:URI()
			{
				Create(uri);
			}
			URI(const wxString& uri)
				:URI()
			{
				Create(uri);
			}
			URI(const char* uri)
				:URI()
			{
				Create(uri);
			}
			URI(const wchar_t* uri)
				:URI()
			{
				Create(uri);
			}
			URI(const URI& other)
				:URI()
			{
				*this = other;
			}
			URI(URI&&) noexcept;
			~URI();

		public:
			bool IsNull() const noexcept;
			void Clear() noexcept;

			bool Create(const String& uri);
			bool Create(const FSPath& path);
			bool Create(const wxURI& uri);
			bool Create(const wxString& uri)
			{
				return Create(String(uri));
			}
			bool Create(const char* uri)
			{
				return Create(String(uri));
			}
			bool Create(const wchar_t* uri)
			{
				return Create(String(uri));
			}

			bool IsReference() const noexcept;
			URI& Resolve(const URI& base, FlagSet<URIFlag> flags = {});
			URI& MakeReference(const URI& base, FlagSet<URIFlag> flags = {});
			URI& Normalize() noexcept;

			String BuildURI() const;
			String BuildUnescapedURI(LineBreakFormat lineBreakFormat = LineBreakFormat::None, FlagSet<URIFlag> flags = {}) const;

		public:
			bool HasScheme() const noexcept;
			String GetScheme() const;

			NetworkHostType GetHostType() const noexcept;
			bool HasServer() const noexcept;
			String GetServer() const;

			bool HasPort() const noexcept;
			std::optional<uint16_t> GetPortInt() const;
			String GetPort() const;

			bool HasPath() const noexcept;
			String GetPath() const;

			bool HasQuery() const noexcept;
			String GetQuery() const;

			bool HasFragment() const noexcept;
			String GetFragment() const;

			bool HasUserInfo() const noexcept;
			String GetUserInfo() const;
			String GetUser() const;
			String GetPassword() const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			URI& operator=(const URI& other);
			URI& operator=(URI&&) noexcept;

			bool operator==(const URI& other) const noexcept;
			bool operator==(const wxURI& other) const;

			operator wxURI() const;
	};
}

namespace std
{
	template<>
	struct hash<kxf::URI> final
	{
		size_t operator()(const kxf::URI& uri) const noexcept;
	};
}
