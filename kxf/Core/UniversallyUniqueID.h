#pragma once
#include "Common.h"
#include "NativeUUID.h"
#include "String.h"
#include <array>

namespace kxf
{
	class LocallyUniqueID;

	enum class UUIDFormat: uint32_t
	{
		None = 0, // 123e4567-e89b-12d3-a456-426655440000
		URN = 1 << 1, // RFC 4122 format: urn:uuid:123e4567-e89b-12d3-a456-426655440000
		CurlyBraces = 1 << 2, // {123e4567-e89b-12d3-a456-426655440000}
		SquareBraces = 1 << 4, // [123e4567-e89b-12d3-a456-426655440000]
		AngleBraces = 1 << 5, // <123e4567-e89b-12d3-a456-426655440000>
		Parentheses = 1 << 6, // (123e4567-e89b-12d3-a456-426655440000)
		UpperCase = 1 << 7, // 123E4567-E89B-12D3-A456-426655440000
		HexPrefix = 1 << 8, // 0x123e4567-0xe89b-0x12d3-0xa456-0x426655440000
		Grouped = 1 << 9 // {0x123e4567, 0xe89b, 0x12d3, {0xa4, 0x56, 0x42, 0x66, 0x55, 0x44, 0x00, 0x00}}
	};
	KxFlagSet_Declare(UUIDFormat);
}

namespace kxf
{
	class KX_API UniversallyUniqueID final
	{
		friend struct std::hash<UniversallyUniqueID>;
		friend struct BinarySerializer<UniversallyUniqueID>;

		public:
			static UniversallyUniqueID Create() noexcept;
			static UniversallyUniqueID CreateSequential() noexcept;

			static UniversallyUniqueID CreateFromInt128(const uint8_t (&bytes)[16]) noexcept;
			static UniversallyUniqueID CreateFromInt128(const std::array<uint8_t, 16> bytes) noexcept;
			static UniversallyUniqueID CreateFromInt128(uint64_t low, uint64_t high) noexcept;

			static UniversallyUniqueID CreateFromString(const char* value) noexcept;
			static UniversallyUniqueID CreateFromString(const wchar_t* value) noexcept;
			static UniversallyUniqueID CreateFromString(const String& value) noexcept;

		private:
			NativeUUID m_ID;

		public:
			constexpr UniversallyUniqueID() noexcept = default;
			constexpr UniversallyUniqueID(UniversallyUniqueID&&) noexcept = default;
			constexpr UniversallyUniqueID(const UniversallyUniqueID&) noexcept = default;
			constexpr UniversallyUniqueID(NativeUUID other) noexcept
				:m_ID(std::move(other))
			{
			}
			UniversallyUniqueID(LocallyUniqueID other) noexcept;

		public:
			constexpr bool IsNull() const noexcept
			{
				return m_ID.IsNull();
			}

			constexpr NativeUUID ToNativeUUID() const noexcept
			{
				return m_ID;
			}
			String ToString(FlagSet<UUIDFormat> format = {}, const String& separator = {}) const;

			LocallyUniqueID ToLocallyUniqueID() const noexcept;
			std::array<uint8_t, 16> ToInt128() const noexcept;

		public:
			constexpr UniversallyUniqueID& operator=(UniversallyUniqueID&&) noexcept = default;
			constexpr UniversallyUniqueID& operator=(const UniversallyUniqueID&) noexcept = default;

			constexpr operator NativeUUID() const noexcept
			{
				return ToNativeUUID();
			}
			operator String() const
			{
				return ToString();
			}

			constexpr explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			constexpr bool operator!() const noexcept
			{
				return IsNull();
			}

		public:
			constexpr auto operator<=>(const UniversallyUniqueID&) const noexcept = default;
			constexpr bool operator==(const UniversallyUniqueID& other) const noexcept
			{
				return m_ID == other.m_ID;
			}

			constexpr auto operator<=>(const NativeUUID& other) const noexcept
			{
				return m_ID <=> other;
			}
			constexpr bool operator==(const NativeUUID& other) const noexcept
			{
				return m_ID == other;
			}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<UniversallyUniqueID> final
	{
		uint64_t Serialize(IOutputStream& stream, const UniversallyUniqueID& value) const
		{
			return Serialization::WriteObject(stream, value.m_ID);
		}
		uint64_t Deserialize(IInputStream& stream, UniversallyUniqueID& value) const
		{
			return Serialization::ReadObject(stream, value.m_ID);
		}
	};
}

namespace std
{
	template<>
	struct hash<kxf::UniversallyUniqueID> final
	{
		constexpr size_t operator()(const kxf::UniversallyUniqueID& uuid) const noexcept
		{
			return std::hash<kxf::NativeUUID>()(uuid.m_ID);
		}
	};

	template<>
	struct numeric_limits<kxf::UniversallyUniqueID>: public numeric_limits<kxf::NativeUUID>
	{
		using Base = numeric_limits<kxf::NativeUUID>;

		static constexpr kxf::UniversallyUniqueID min() noexcept
		{
			return Base::min();
		}
		static constexpr kxf::UniversallyUniqueID lowest() noexcept
		{
			return Base::lowest();
		}
		static constexpr kxf::UniversallyUniqueID max() noexcept
		{
			return Base::max();
		}
	};

	template<>
	struct formatter<kxf::UniversallyUniqueID, char>: std::formatter<std::string_view, char>
	{
		template<class TFormatContext>
		auto format(const kxf::UniversallyUniqueID& value, TFormatContext& formatContext) const
		{
			auto formatted = value.ToString(kxf::UUIDFormat::CurlyBraces).ToUTF8();
			return std::formatter<std::string_view, char>::format(formatted, formatContext);
		}
	};

	template<>
	struct formatter<kxf::UniversallyUniqueID, wchar_t>: std::formatter<std::wstring_view, wchar_t>
	{
		template<class TFormatContext>
		auto format(const kxf::UniversallyUniqueID& value, TFormatContext& formatContext) const
		{
			auto formatted = value.ToString(kxf::UUIDFormat::CurlyBraces);
			return std::formatter<std::wstring_view, wchar_t>::format(formatted.view(), formatContext);
		}
	};
}
