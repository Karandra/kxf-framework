#include "kxf-pch.h"
#include "UniversallyUniqueID.h"
#include "LocallyUniqueID.h"
#include "RegEx.h"
#include "kxf/System/HResult.h"
#include "kxf/System/Win32Error.h"
#include "kxf/Utility/Memory.h"

#include <Windows.h>
#include <rpcdce.h>
#include "kxf/Win32/LinkLibs-System.h"
#include "kxf/Win32/UndefMacros.h"

namespace
{
	constexpr kxf::XChar g_DefaultSeparator[] = kxS("-");
	constexpr kxf::XChar g_RFC_URN[] = kxS("urn:uuid:");

	template<class T, class TUUID>
	auto CastAs(TUUID&& uuid)
	{
		if constexpr(std::is_const_v<std::remove_reference_t<TUUID>>)
		{
			return reinterpret_cast<const T*>(&uuid);
		}
		else
		{
			return reinterpret_cast<T*>(&uuid);
		}
	}

	template<class T>
	auto AsGUID(T&& uuid)
	{
		return CastAs<::GUID>(std::forward<T>(uuid));
	}

	template<class T>
	auto AsUUID(T&& uuid)
	{
		return CastAs<::UUID>(std::forward<T>(uuid));
	}

	kxf::NativeUUID DoCreateFromString(const kxf::String& value) noexcept
	{
		using namespace kxf;

		// Try system native parsing functions first. They're probably faster and more correct.
		NativeUUID uuid;
		if (::UuidFromStringW(reinterpret_cast<RPC_WSTR>(value.wc_str().unsafe_intptr()), AsUUID(uuid)) == RPC_S_OK)
		{
			return uuid;
		}
		else if (HResult(::CLSIDFromString(value.wc_str(), AsUUID(uuid))))
		{
			return uuid;
		}
		else
		{
			// Try to parse on our own with RegEx
			constexpr size_t shortVariant = 5;
			constexpr size_t longVariant = 11;

			String current = value;
			if (current.StartsWith(g_RFC_URN))
			{
				current.Remove(0, std::size(g_RFC_URN) - 1);
			}

			// Construct a regex that can match a single group and apply it as many times as we need to
			// find all required groups but no more than the maximum we can have.
			RegEx regEx("(?:0?x?)([\\dabcdef]+)", RegExFlag::IgnoreCase);

			std::vector<String> items;
			for (size_t i = 0; i < std::max(shortVariant, longVariant); i++)
			{
				if (regEx.Matches(current))
				{
					const String& item = items.emplace_back(regEx.GetMatch(current, 1));
					if (!item.IsEmptyOrWhitespace())
					{
						// Remove the full match from the current string
						size_t start = 0;
						size_t length = 0;
						regEx.GetMatch(start, length, 0);

						current.Remove(0, start + length);
					}
					else
					{
						items.pop_back();
					}
				}
			}

			if (items.size() == shortVariant || items.size() == longVariant)
			{
				uuid.Data1 = items[0].ToInteger<uint32_t>(16).value_or(0);
				uuid.Data2 = items[1].ToInteger<uint16_t>(16).value_or(0);
				uuid.Data3 = items[2].ToInteger<uint16_t>(16).value_or(0);

				if (items.size() == shortVariant)
				{
					const String& data4_01 = items[3];
					uuid.Data4[0] = data4_01.SubLeft(2).ToInteger<uint8_t>(16).value_or(0);
					uuid.Data4[1] = data4_01.SubRight(2).ToInteger<uint8_t>(16).value_or(0);

					const String& data4_27 = items[4];
					for (size_t i = 2; i < std::size(uuid.Data4); i++)
					{
						uuid.Data4[i] = data4_27.SubMid((i - 2) * 2, 2).ToInteger<uint8_t>(16).value_or(0);
					}
				}
				else if (items.size() == longVariant)
				{
					for (size_t i = 0; i < std::size(uuid.Data4); i++)
					{
						uuid.Data4[i] = items[3 + i].ToInteger<uint8_t>(16).value_or(0);
					}
				}
				return uuid;
			}
			else if (items.size() == 11)
			{
				return uuid;
			}
		}
		return {};
	}
}

namespace kxf
{
	UniversallyUniqueID UniversallyUniqueID::Create() noexcept
	{
		NativeUUID uuid;
		if (HResult(::CoCreateGuid(AsGUID(uuid))))
		{
			return uuid;
		}
		return {};
	}
	UniversallyUniqueID UniversallyUniqueID::CreateSequential() noexcept
	{
		NativeUUID uuid;
		Win32Error error = ::UuidCreateSequential(AsGUID(uuid));
		if (error.IsSuccess() || *error == RPC_S_UUID_LOCAL_ONLY)
		{
			return uuid;
		}
		return {};
	}

	UniversallyUniqueID UniversallyUniqueID::CreateFromInt128(const uint8_t(&bytes)[16]) noexcept
	{
		NativeUUID uuid;
		std::memcpy(&uuid, bytes, std::size(bytes));

		return uuid;
	}
	UniversallyUniqueID UniversallyUniqueID::CreateFromInt128(uint64_t low, uint64_t high) noexcept
	{
		std::array<uint8_t, 16> bytes;
		std::memcpy(&bytes, &low, sizeof(low));
		std::memcpy(&bytes[sizeof(low)], &high, sizeof(high));

		return CreateFromInt128(bytes);
	}
	UniversallyUniqueID UniversallyUniqueID::CreateFromInt128(const std::array<uint8_t, 16> bytes) noexcept
	{
		NativeUUID uuid;
		std::memcpy(&uuid, bytes.data(), bytes.size());

		return uuid;
	}
	UniversallyUniqueID UniversallyUniqueID::CreateFromString(const char* value) noexcept
	{
		return DoCreateFromString(value);
	}
	UniversallyUniqueID UniversallyUniqueID::CreateFromString(const wchar_t* value) noexcept
	{
		return DoCreateFromString(value);
	}
	UniversallyUniqueID UniversallyUniqueID::CreateFromString(const String& value) noexcept
	{
		return DoCreateFromString(value);
	}

	UniversallyUniqueID::UniversallyUniqueID(LocallyUniqueID other) noexcept
	{
		uint64_t vlaue = other.ToInt();
		std::memset(&m_ID, 0, sizeof(m_ID));
		std::memcpy(&m_ID, &vlaue, sizeof(vlaue));
	}

	String UniversallyUniqueID::ToString(FlagSet<UUIDFormat> format, const String& separator) const
	{
		// We don't allow characters that can occur in the UUID itself to be used as a separator
		if (!separator.IsEmptyOrWhitespace() && separator != g_DefaultSeparator && separator.ContainsAnyOfCharacters("0x123456789abcdef", StringActionFlag::IgnoreCase))
		{
			return {};
		}

		String uuid;
		bool shouldReplaceDefaultSeparator = false;

		// Decide on brace character
		std::pair<XChar, XChar> braces = {0, 0};
		if (format & UUIDFormat::CurlyBraces)
		{
			braces = {'{', '}'};
		}
		else if (format & UUIDFormat::SquareBraces)
		{
			braces = {'[', ']'};
		}
		else if (format & UUIDFormat::AngleBraces)
		{
			braces = {'<', '>'};
		}
		else if (format & UUIDFormat::Parentheses)
		{
			braces = {'(', ')'};
		}

		if (format & UUIDFormat::HexPrefix || format & UUIDFormat::Grouped)
		{
			auto DoPart = [&](auto value, size_t width, bool suppressSeparator = false)
			{
				String part = Format("{:0{}x}", value, width);
				part.Truncate(width);

				if (format & UUIDFormat::HexPrefix)
				{
					uuid += "0x";
				}
				uuid += std::move(part);

				if (!suppressSeparator)
				{
					uuid += separator.IsEmpty() ? g_DefaultSeparator : separator;
				}
			};

			if (format & UUIDFormat::Grouped)
			{
				DoPart(m_ID.Data1, 8);
				DoPart(m_ID.Data2, 4);
				DoPart(m_ID.Data3, 4);

				if (braces.first != 0)
				{
					uuid += braces.first;
				}
				for (size_t i = 0; i < std::size(m_ID.Data4); i++)
				{
					DoPart(m_ID.Data4[i], 2, i + 1 == std::size(m_ID.Data4));
				}
				if (braces.second != 0)
				{
					uuid += braces.second;
				}
			}
			else
			{
				DoPart(m_ID.Data1, 8);
				DoPart(m_ID.Data2, 4);
				DoPart(m_ID.Data3, 4);

				// Combine the first 2 bytes of the 8 byte array into an 'uint16_t' to print it
				DoPart(Utility::CompositeInteger<uint8_t>(m_ID.Data4[0], m_ID.Data4[1]).GetFull(), 4);

				// Combine the last 6 bytes into an 'uint64_t' and print it
				auto d4_23 = *Utility::CompositeInteger<uint8_t>(m_ID.Data4[2], m_ID.Data4[3]);
				auto d4_45 = *Utility::CompositeInteger<uint8_t>(m_ID.Data4[4], m_ID.Data4[5]);
				auto d4_67 = *Utility::CompositeInteger<uint8_t>(m_ID.Data4[6], m_ID.Data4[7]);
				auto d4_x0 = *Utility::CompositeInteger<uint16_t>(d4_23, d4_45);
				auto d4_x1 = *Utility::CompositeInteger<uint16_t>(d4_67, 0);
				DoPart(*Utility::CompositeInteger<uint32_t>(d4_x0, d4_x1), 12, true);
			}
		}
		else
		{
			// Use system native to string conversion
			uuid = [&]() -> String
			{
				wchar_t* stringUUID = nullptr;
				if (::UuidToStringW(AsUUID(m_ID), reinterpret_cast<RPC_WSTR*>(&stringUUID)) == RPC_S_OK && stringUUID)
				{
					String temp = stringUUID;
					::RpcStringFreeW(reinterpret_cast<RPC_WSTR*>(&stringUUID));
					return temp;
				}
				return {};
			}();
			shouldReplaceDefaultSeparator = true;
		}

		// Make uppercase
		if (format & UUIDFormat::UpperCase)
		{
			uuid.MakeUpper();
		}

		// Replace the default separator with a user supplied one if we were using system native function to convert UUID to string
		// because it doesn't allow to customize the separator.
		if (shouldReplaceDefaultSeparator && !separator.IsEmpty() && separator != g_DefaultSeparator)
		{
			uuid.Replace(g_DefaultSeparator, separator);
		}

		// Add braces or parentheses if required
		if (braces.first != 0 && braces.second != 0)
		{
			uuid.Prepend(braces.first);
			uuid.Append(braces.second);
		}

		// Prepend URN format prefix if asked to
		if (format & UUIDFormat::URN)
		{
			uuid.Prepend(g_RFC_URN);
		}

		return uuid;
	}
	LocallyUniqueID UniversallyUniqueID::ToLocallyUniqueID() const noexcept
	{
		for (uint8_t d4: m_ID.Data4)
		{
			if (d4 != 0)
			{
				return {};
			}
		}

		uint64_t value = 0;
		std::memcpy(&value, &m_ID, sizeof(value));
		return value;
	}
	std::array<uint8_t, 16> UniversallyUniqueID::ToInt128() const noexcept
	{
		std::array<uint8_t, 16> bytes;
		std::memcpy(bytes.data(), &m_ID, std::size(bytes));
		return bytes;
	}
}
