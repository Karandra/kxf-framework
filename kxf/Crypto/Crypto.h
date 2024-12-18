#pragma once
#include "Common.h"
#include "HashValue.h"

namespace kxf
{
	class IInputStream;
	class IOutputStream;
}

namespace kxf::Crypto
{
	KXF_API size_t Rot13(String& source) noexcept;
	KXF_API size_t UwUize(String& source) noexcept;

	KXF_API HashValue<32> CRC32(IInputStream& stream, uint32_t initialValue = 0xFFFFFFFFu) noexcept;
	KXF_API HashValue<128> MD5(IInputStream& stream) noexcept;

	KXF_API HashValue<160> SHA1(IInputStream& stream) noexcept;

	KXF_API HashValue<224> SHA2_224(IInputStream& stream) noexcept;
	KXF_API HashValue<256> SHA2_256(IInputStream& stream) noexcept;
	KXF_API HashValue<384> SHA2_384(IInputStream& stream) noexcept;
	KXF_API HashValue<512> SHA2_512(IInputStream& stream) noexcept;

	KXF_API HashValue<224> SHA3_224(IInputStream& stream) noexcept;
	KXF_API HashValue<256> SHA3_256(IInputStream& stream) noexcept;
	KXF_API HashValue<384> SHA3_384(IInputStream& stream) noexcept;
	KXF_API HashValue<512> SHA3_512(IInputStream& stream) noexcept;

	KXF_API HashValue<32> xxHash_32(const void* data, size_t size) noexcept;
	KXF_API HashValue<64> xxHash_64(const void* data, size_t size) noexcept;
	KXF_API HashValue<128> xxHash_128(const void* data, size_t size) noexcept;
	KXF_API HashValue<32> xxHash_32(IInputStream& stream) noexcept;
	KXF_API HashValue<64> xxHash_64(IInputStream& stream) noexcept;

	KXF_API bool Base64Encode(IInputStream& inputStream, IOutputStream& outputStream);
	KXF_API bool Base64Decode(IInputStream& inputStream, IOutputStream& outputStream);
};
