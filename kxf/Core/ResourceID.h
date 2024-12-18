#pragma once
#include "Common.h"
#include "kxf/Network/URI.h"

namespace kxf
{
	class KXF_API ResourceID final
	{
		friend struct std::hash<ResourceID>;

		private:
			URI m_Value;

		public:
			ResourceID() noexcept = default;

			template<class T>
			requires(std::is_integral_v<T> || std::is_enum_v<T>)
			ResourceID(T id) noexcept
			{
				m_Value.Create(kxf::ToString(id));
			}

			ResourceID(URI id) noexcept
				:m_Value(std::move(id))
			{
			}
			ResourceID(const String& id) noexcept
				:ResourceID(URI(id))
			{
			}
			ResourceID(const char* id) noexcept
				:ResourceID(URI(id))
			{
			}
			ResourceID(const wchar_t* id) noexcept
				:ResourceID(URI(id))
			{
			}

			ResourceID(const ResourceID&) = default;
			ResourceID(ResourceID&&) noexcept = default;

		public:
			bool IsNull() const noexcept
			{
				return m_Value.IsNull();
			}

			bool HasScheme() const noexcept
			{
				return m_Value.HasScheme();
			}
			String GetScheme() const
			{
				return m_Value.GetScheme();
			}
			String GetPath() const;

			// Integer
			template<class T = int>
			requires(std::is_integral_v<T> || std::is_enum_v<T>)
			std::optional<T> ToInt() const noexcept
			{
				return GetPath().ToInteger<T>();
			}

			// URI
			const URI& ToURI() const& noexcept
			{
				return m_Value;
			}
			URI ToURI() && noexcept
			{
				return std::move(m_Value);
			}

			// String
			String ToString() const
			{
				return m_Value.BuildURI();
			}

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}

			bool operator==(const ResourceID& other) const noexcept
			{
				return this == &other || m_Value == other.m_Value;
			}

			ResourceID& operator=(const ResourceID&) = default;
			ResourceID& operator=(ResourceID&&) noexcept = default;
	};
}

namespace std
{
	template<>
	struct hash<kxf::ResourceID> final
	{
		size_t operator()(const kxf::ResourceID& id) const noexcept
		{
			return std::hash<kxf::URI>()(id.m_Value);
		}
	};
}
