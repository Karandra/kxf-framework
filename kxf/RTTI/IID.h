#pragma once
#include "Common.h"
#include "kxf/Core/NativeUUID.h"

namespace kxf
{
	class UniversallyUniqueID;
}

namespace kxf
{
	class KX_API IID final
	{
		friend struct std::hash<IID>;
		friend struct BinarySerializer<IID>;

		private:
			NativeUUID m_ID;

		public:
			constexpr IID() noexcept = default;
			constexpr IID(NativeUUID uuid) noexcept
				:m_ID(std::move(uuid))
			{
			}

		public:
			constexpr bool IsNull() const noexcept
			{
				return m_ID.IsNull();
			}
			constexpr const NativeUUID& ToNativeUUID() const noexcept
			{
				return m_ID;
			}
			UniversallyUniqueID ToUniversallyUniqueID() const noexcept;

			template<class T>
			constexpr bool IsOfType() const noexcept
			{
				return *this == RTTI::GetInterfaceID<T>();
			}

		public:
			explicit constexpr operator bool() const noexcept
			{
				return !m_ID.IsNull();
			}
			constexpr bool operator!() const noexcept
			{
				return m_ID.IsNull();
			}

			constexpr auto operator<=>(const IID&) const noexcept = default;
			constexpr bool operator==(const IID& other) const noexcept
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
	struct BinarySerializer<IID> final
	{
		uint64_t Serialize(IOutputStream& stream, const IID& value) const
		{
			return Serialization::WriteObject(stream, value.m_ID);
		}
		uint64_t Deserialize(IInputStream& stream, IID& value) const
		{
			return Serialization::ReadObject(stream, value.m_ID);
		}
	};
}

namespace std
{
	template<>
	struct hash<kxf::IID> final
	{
		constexpr size_t operator()(const kxf::IID& iid) const noexcept
		{
			return std::hash<kxf::NativeUUID>()(iid.m_ID);
		}
	};
}
