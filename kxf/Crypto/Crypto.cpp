#include "kxf-pch.h"
#include "Crypto.h"
#include "kxf/IO/IStream.h"
#include "kxf/Core/String.h"
#include "kxf/Core/DataSize.h"
#include "kxf/Utility/ScopeGuard.h"

#include <wx/base64.h> 
#include <wx/regex.h> 
#include "OpenSSL/opensslv.h"
#include "OpenSSL/md5.h"
#include "OpenSSL/sha.h"
#include "OpenSSL/evp.h"

#define XXH_STATIC_LINKING_ONLY
#include <xxhash.h>

namespace
{
	using namespace kxf;
	using namespace kxf::Crypto;

	constexpr size_t g_StreamBlockSize = DataSize::FromKB(64).ToBytes();

	template<class THashContext, size_t hashLength, class TInitFunc, class TUpdateFunc, class TFinalFunc, class TValue = uint8_t>
	HashValue<hashLength * 8> DoCalcHash1(IInputStream& stream, TInitFunc&& initFunc, TUpdateFunc&& updateFunc, TFinalFunc&& finalFunc) noexcept
	{
		THashContext hashContext;
		if (std::invoke(initFunc, &hashContext) != 0)
		{
			TValue buffer[g_StreamBlockSize] = {};
			while (stream.CanRead())
			{
				if (stream.Read(buffer, std::size(buffer)).LastRead() != 0)
				{
					std::invoke(updateFunc, &hashContext, &buffer, stream.LastRead().ToBytes());
				}
				else
				{
					break;
				}
			}

			HashValue<hashLength * 8> hash;
			if (std::invoke(finalFunc, reinterpret_cast<TValue*>(hash.data()), &hashContext) != 0)
			{
				return hash;
			}
		}
		return {};
	}

	template<size_t bitLength, class THashAlgorithm, class TValue = uint8_t>
	HashValue<bitLength> DoCalcHash2(IInputStream& stream, THashAlgorithm&& algorithFunc) noexcept
	{
		if (const EVP_MD* algorithm = std::invoke(algorithFunc))
		{
			EVP_MD_CTX* context = EVP_MD_CTX_create();
			Utility::ScopeGuard atExit([&]()
			{
				if (context)
				{
					EVP_MD_CTX_destroy(context);
				}
			});

			if (context && EVP_DigestInit_ex(context, algorithm, nullptr) == 1)
			{
				TValue buffer[g_StreamBlockSize] = {};
				while (stream.CanRead())
				{
					if (stream.Read(buffer, std::size(buffer)).LastRead() != 0)
					{
						if (EVP_DigestUpdate(context, &buffer, stream.LastRead().ToBytes()) != 1)
						{
							return {};
						}
					}
					else
					{
						break;
					}
				}

				HashValue<bitLength> hash;
				unsigned int length = EVP_MD_size(algorithm);

				if (EVP_DigestFinal_ex(context, reinterpret_cast<unsigned char*>(hash.data()), &length) == 1 && length <= hash.length())
				{
					return hash;
				}
			}
		}
		return {};
	}

	template<size_t hashLength, class TInitFunc, class TFreeFunc, class TResetFunc, class TUpdateFunc, class TFinalFunc, class TValue = uint8_t>
	HashValue<hashLength> DoCalcXXHash(IInputStream& stream,
									   TInitFunc&& initFunc,
									   TFreeFunc&& freeFunc,
									   TResetFunc&& resetFunc,
									   TUpdateFunc&& updateFunc,
									   TFinalFunc&& finalFunc
	) noexcept
	{
		if (auto* state = std::invoke(initFunc))
		{
			Utility::ScopeGuard atExit = [&]()
			{
				// State can be re-used, here it's simply freed
				std::invoke(freeFunc, state);
			};

			// Initialize state with selected seed (or with 0)
			if (std::invoke(resetFunc, state, 0) != XXH_ERROR)
			{
				// Feed the state with input data, any size, any number of times
				TValue buffer[g_StreamBlockSize] = {};
				while (stream.CanRead())
				{
					if (stream.Read(buffer, std::size(buffer)).LastRead() != 0)
					{
						if (std::invoke(updateFunc, state, buffer, stream.LastRead().ToBytes()) == XXH_ERROR)
						{
							break;
						}
					}
					else
					{
						break;
					}
				}

				// Get the hash
				auto hash = std::invoke(finalFunc, state);
				return HashValue<hashLength>(&hash, sizeof(hash));
			}
		}
		return {};
	}
}

namespace kxf::Crypto
{
	size_t Rot13(String& source) noexcept
	{
		size_t count = 0;
		for (XChar& c: source)
		{
			if (UniChar(c).IsASCII())
			{
				if ((c >= 'A' && c <= 'M') || (c >= 'a' && c <= 'm'))
				{
					c += 13;
					count++;
				}
				else if ((c >= 'N' && c <= 'Z') || (c >= 'n' && c <= 'z'))
				{
					c -= 13;
					count++;
				}
			}
		}
		return count;
	}
	size_t UwUize(String& source) noexcept
	{
		size_t count = 0;
		for (XChar& c: source)
		{
			if (c == 'l' || c == 'r')
			{
				c = 'w';
				count++;
			}
			else if (c == 'L' || c == 'R')
			{
				c = 'W';
				count++;
			}
		}
		return count;
	}

	HashValue<32> CRC32(IInputStream& stream, uint32_t initialValue) noexcept
	{
		constexpr uint32_t table[] =
		{
			0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
			0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
			0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
			0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
			0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
			0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
			0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
			0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
			0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
			0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
			0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
			0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
			0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
			0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
			0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
			0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
			0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
			0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
			0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
			0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
			0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
			0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
			0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
			0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
			0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
			0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
			0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
			0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
			0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
			0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
			0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
			0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
			0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
			0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
			0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
			0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
			0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
			0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
			0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
			0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
			0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
			0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
			0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
		};

		uint32_t result = initialValue;
		while (stream.CanRead())
		{
			uint8_t buffer[g_StreamBlockSize] = {};
			const size_t readBytes = stream.Read(buffer, std::size(buffer)).LastRead().ToBytes();

			for (size_t i = 0; i < readBytes; i++)
			{
				result = table[(result ^ buffer[i]) & 0xFFu] ^ (result >> 8);
			}
		}
		return ~result;
	}
	HashValue<128> MD5(IInputStream& stream) noexcept
	{
		return DoCalcHash2<MD5_DIGEST_LENGTH * 8>(stream, EVP_md5);
	}

	HashValue<160> SHA1(IInputStream& stream) noexcept
	{
		return DoCalcHash2<SHA_DIGEST_LENGTH * 8>(stream, EVP_sha1);
	}

	HashValue<224> SHA2_224(IInputStream& stream) noexcept
	{
		return DoCalcHash2<SHA224_DIGEST_LENGTH * 8>(stream, EVP_sha224);
	}
	HashValue<256> SHA2_256(IInputStream& stream) noexcept
	{
		return DoCalcHash2<SHA256_DIGEST_LENGTH * 8>(stream, EVP_sha256);
	}
	HashValue<384> SHA2_384(IInputStream& stream) noexcept
	{
		return DoCalcHash2<SHA384_DIGEST_LENGTH * 8>(stream, EVP_sha384);
	}
	HashValue<512> SHA2_512(IInputStream& stream) noexcept
	{
		return DoCalcHash2<SHA512_DIGEST_LENGTH * 8>(stream, EVP_sha512);
	}

	HashValue<224> SHA3_224(IInputStream& stream) noexcept
	{
		return DoCalcHash2<224>(stream, EVP_sha3_224);
	}
	HashValue<256> SHA3_256(IInputStream& stream) noexcept
	{
		return DoCalcHash2<256>(stream, EVP_sha3_256);
	}
	HashValue<384> SHA3_384(IInputStream& stream) noexcept
	{
		return DoCalcHash2<384>(stream, EVP_sha3_384);
	}
	HashValue<512> SHA3_512(IInputStream& stream) noexcept
	{
		return DoCalcHash2<512>(stream, EVP_sha3_512);
	}

	HashValue<32> xxHash_32(const void* data, size_t size) noexcept
	{
		return XXH32(data, size, 0);
	}
	HashValue<64> xxHash_64(const void* data, size_t size) noexcept
	{
		return XXH64(data, size, 0);
	}
	HashValue<128> xxHash_128(const void* data, size_t size) noexcept
	{
		XXH128_hash_t result = XXH3_128bits(data, size);
		return HashValue<128>(&result, sizeof(result));
	}
	HashValue<32> xxHash_32(IInputStream& stream) noexcept
	{
		return DoCalcXXHash<32>(stream, XXH32_createState, XXH32_freeState, XXH32_reset, XXH32_update, XXH32_digest);
	}
	HashValue<64> xxHash_64(IInputStream& stream) noexcept
	{
		return DoCalcXXHash<64>(stream, XXH64_createState, XXH64_freeState, XXH64_reset, XXH64_update, XXH64_digest);
	}

	bool Base64Encode(IInputStream& inputStream, IOutputStream& outputStream)
	{
		const size_t size = wxBase64EncodedSize(g_StreamBlockSize);
		wxMemoryBuffer buffer(size);

		uint8_t readBuffer[g_StreamBlockSize] = {};
		while (inputStream.CanRead())
		{
			if (inputStream.Read(readBuffer, std::size(readBuffer)).LastRead() != 0)
			{
				buffer.AppendData(readBuffer, inputStream.LastRead().ToBytes());
			}
			else
			{
				break;
			}

			if (buffer.GetDataLen() == size)
			{
				auto utf8 = wxBase64Encode(buffer).ToUTF8();
				if (outputStream.WriteAll(utf8.data(), utf8.length()))
				{
					return true;
				}
				else
				{
					break;
				}
			}
		}
		return false;
	}
	bool Base64Decode(IInputStream& inputStream, IOutputStream& outputStream)
	{
		const size_t size = wxBase64DecodedSize(g_StreamBlockSize);
		wxMemoryBuffer buffer(size);

		uint8_t readBuffer[g_StreamBlockSize] = {};
		while (inputStream.CanRead())
		{
			if (inputStream.Read(readBuffer, std::size(readBuffer)).LastRead() != 0)
			{
				buffer.AppendData(readBuffer, inputStream.LastRead().ToBytes());
			}
			else
			{
				break;
			}

			if (buffer.GetDataLen() == size)
			{
				wxMemoryBuffer outBuffer = wxBase64Decode(reinterpret_cast<const char*>(buffer.GetData()), buffer.GetDataLen(), wxBase64DecodeMode_SkipWS);
				if (outputStream.WriteAll(outBuffer.GetData(), outBuffer.GetDataLen()))
				{
					return true;
				}
				else
				{
					break;
				}
			}
		}
		return false;
	}
}
