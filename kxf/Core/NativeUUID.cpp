#include "kxf-pch.h"
#include "NativeUUID.h"
#include "kxf/IO/IStream.h"
#include "kxf/Core/UniversallyUniqueID.h"
#include <guiddef.h>

namespace kxf
{
	UniversallyUniqueID NativeUUID::ToUniversallyUniqueID() const noexcept
	{
		return *this;
	}

	void NativeUUID::FromPlatformUUID(const void* uuid) noexcept
	{
		static_assert(sizeof(GUID) == sizeof(NativeUUID), "sizeof(GUID) must be equal to sizeof(NativeUUID)");

		std::memcpy(this, uuid, sizeof(NativeUUID));
	}
	void NativeUUID::ToPlatformUUID(void* uuid) const noexcept
	{
		static_assert(sizeof(GUID) == sizeof(NativeUUID), "sizeof(GUID) must be equal to sizeof(NativeUUID)");

		std::memcpy(uuid, this, sizeof(GUID));
	}
}

namespace kxf
{
	uint64_t BinarySerializer<NativeUUID>::Serialize(IOutputStream& stream, const NativeUUID& value) const
	{
		uint64_t written = Serialization::WriteObject(stream, value.Data1);
		written += Serialization::WriteObject(stream, value.Data2);
		written += Serialization::WriteObject(stream, value.Data3);
		for (const auto& d4: value.Data4)
		{
			written += Serialization::WriteObject(stream, d4);
		}

		return written;
	}
	uint64_t BinarySerializer<NativeUUID>::Deserialize(IInputStream& stream, NativeUUID& value) const
	{
		uint64_t read = Serialization::ReadObject(stream, value.Data1);
		read += Serialization::ReadObject(stream, value.Data2);
		read += Serialization::ReadObject(stream, value.Data3);
		for (auto& d4: value.Data4)
		{
			read += Serialization::ReadObject(stream, d4);
		}

		return read;
	}
}
