#pragma once
#include "kxf/Common.hpp"
#include "kxf/Core/String.h"
#include "kxf/Core/UniChar.h"
#include <unordered_map>
#include <unordered_set>

namespace kxf
{
	class IEncodingConverter;
}

namespace kxf::Utility
{
	struct StringEqualToNoCase final
	{
		template<class T>
		bool operator()(T&& left, T&& right) const noexcept
		{
			return String::Compare(std::forward<T>(left), std::forward<T>(right), StringActionFlag::IgnoreCase) == 0;
		}
	};

	struct StringHashNoCase final
	{
		// From Boost
		template<class T>
		static void hash_combine(size_t& seed, const T& v) noexcept
		{
			std::hash<T> hasher;
			seed ^= hasher(v) + static_cast<size_t>(0x9e3779b9u) + (seed << 6) + (seed >> 2);
		}

		template<class T>
		size_t operator()(T&& value) const noexcept
		{
			size_t hashValue = 0;
			for (UniChar c: value)
			{
				hash_combine(hashValue, c.ToLowerCase().GetValue());
			}
			return hashValue;
		}
	};

	template<class TKey, class TValue>
	using UnorderedMapNoCase = std::unordered_map<TKey, TValue, StringHashNoCase, StringEqualToNoCase>;
	
	template<class TValue>
	using UnorderedSetNoCase = std::unordered_set<TValue, StringHashNoCase, StringEqualToNoCase>;
}

namespace kxf::Utility
{
	class StringBuffer final
	{
		private:
			enum class Type
			{
				None = -1,

				NarrowChars,
				WideChars
			};

		private:
			String& m_Value;
			size_t m_Length = 0;
			std::vector<std::byte> m_Buffer;
			IEncodingConverter* m_EncodingConverter = nullptr;
			Type m_Type = Type::None;
			bool m_NullTerminated = false;

		private:
			char* PrepareNarrowChars();
			wchar_t* PrepareWideChars();
			void Finalize();

		public:
			StringBuffer(String& value, size_t length, bool nullTerminated = false) noexcept
				:m_Value(value), m_Length(length), m_NullTerminated(nullTerminated)
			{
			}
			StringBuffer(String& value, size_t length, IEncodingConverter& encondigConverter, bool nullTerminated = false) noexcept
				:m_Value(value), m_Length(length), m_EncodingConverter(&encondigConverter), m_NullTerminated(nullTerminated)
			{
			}
			~StringBuffer()
			{
				Finalize();
			}

		public:
			char* nc_str()
			{
				return PrepareNarrowChars();
			}
			wchar_t* wc_str()
			{
				return PrepareWideChars();
			}
			size_t length() const noexcept
			{
				return m_Length;
			}

		public:
			operator char*()
			{
				return PrepareNarrowChars();
			}
			operator wchar_t*()
			{
				return PrepareWideChars();
			}
	};
}

namespace kxf::Utility
{
	KXF_API std::optional<bool> ParseBool(const String& value);
}
