#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class KXF_API IEncodingConverter: public RTTI::Interface<IEncodingConverter>
	{
		kxf_RTTI_DeclareIID(IEncodingConverter, {0x31d9a1b0, 0x446e, 0x4569, {0x9f, 0x90, 0x79, 0xe1, 0xef, 0x23, 0xc3, 0x25}});

		private:
			template<class T>
			static constexpr bool IsValidMBType() noexcept
			{
				return std::is_trivially_copyable_v<T> && sizeof(T) == sizeof(std::byte);
			}

			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		protected:
			virtual size_t ToMultiByteBuffer(std::span<const wchar_t> source, std::span<std::byte> destination) = 0;
			virtual size_t ToWideCharBuffer(std::span<const std::byte> source, std::span<wchar_t> destination) = 0;

		public:
			~IEncodingConverter() = default;

		public:
			virtual String GetEncodingName() const = 0;

		public:
			size_t ToMultiByte(std::span<const wchar_t> source, std::span<std::byte> destination)
			{
				if (!source.empty())
				{
					return ToMultiByteBuffer(source, destination);
				}
				return 0;
			}

			template<class T = char> requires(IsValidMBType<T>())
			std::basic_string<T> ToMultiByte(std::span<const wchar_t> source)
			{
				if (!source.empty())
				{
					std::basic_string<T> destination;
					destination.resize(ToMultiByteBuffer(source, {}));
					ToMultiByteBuffer(source, {reinterpret_cast<std::byte*>(destination.data()), destination.length()});

					return destination;
				}
				return {};
			}

			template<class T = char> requires(IsValidMBType<T>())
			std::basic_string<T> ToMultiByte(const wchar_t* source, size_t length = npos)
			{
				if (source && length == npos)
				{
					length = std::char_traits<wchar_t>::length(source);
				}
				return ToMultiByte<T>(std::span(source, length));
			}

		public:
			size_t ToWideChar(std::span<const std::byte> source, std::span<wchar_t> destination)
			{
				if (!source.empty())
				{
					return ToWideCharBuffer(source, destination);
				}
				return 0;
			}

			template<class T> requires(IsValidMBType<T>())
			std::wstring ToWideChar(std::span<const T> source)
			{
				if (!source.empty())
				{
					std::wstring destination;
					destination.resize(ToWideCharBuffer(std::as_bytes(source), {}));
					ToWideCharBuffer(std::as_bytes(source), destination);

					return destination;
				}
				return {};
			}

			template<class T> requires(IsValidMBType<T>())
			std::wstring ToWideChar(std::basic_string_view<T> source)
			{
				return ToWideChar<T>(std::span(source));
			}

			template<class T> requires(IsValidMBType<T>())
			std::wstring ToWideChar(const std::basic_string<T>& source)
			{
				return ToWideChar<T>(std::span(source));
			}

			template<class T> requires(IsValidMBType<T>())
			std::wstring ToWideChar(const T* source, size_t length = npos)
			{
				if (source && length == npos)
				{
					length = std::char_traits<T>::length(source);
				}
				return ToWideChar<T>(std::span(source, length));
			}
	};
}

namespace kxf
{
	extern IEncodingConverter& EncodingConverter_WhateverWorks;

	extern IEncodingConverter& EncodingConverter_Local;
	extern IEncodingConverter& EncodingConverter_ASCII;
	extern IEncodingConverter& EncodingConverter_UTF8;

	extern IEncodingConverter& EncodingConverter_UTF16LE;
	extern IEncodingConverter& EncodingConverter_UTF16BE;

	extern IEncodingConverter& EncodingConverter_UTF32LE;
	extern IEncodingConverter& EncodingConverter_UTF32BE;

	KXF_API std::unique_ptr<IEncodingConverter> GetNativeEncodingConverter(int codePage, FlagSet<uint32_t> flags = {});
}
