#include "kxf-pch.h"
#include "String.h"
#include "kxf/Core/IEncodingConverter.h"

namespace
{
	template<class T>
	T* PrepareBuffer(std::vector<std::byte>& buffer, size_t length)
	{
		buffer.resize(length * sizeof(T));
		return reinterpret_cast<T*>(buffer.data());
	}

	template<class T>
	kxf::String FinalizeBuffer(std::vector<std::byte>& buffer, size_t& length, bool isNullTerminated, kxf::IEncodingConverter* encondigConverter)
	{
		auto ptr = reinterpret_cast<T*>(buffer.data());
		if (isNullTerminated)
		{
			size_t size = buffer.size() / sizeof(T);
			if constexpr(std::is_same_v<T, char>)
			{
				length = ::strnlen(ptr, size);
			}
			else if constexpr(std::is_same_v<T, wchar_t>)
			{
				length = ::wcsnlen(ptr, size);
			}

			buffer.resize(length * sizeof(T));
			ptr = reinterpret_cast<T*>(buffer.data());
		}

		if constexpr(std::is_same_v<T, char>)
		{
			auto& converter = encondigConverter ? *encondigConverter : kxf::EncodingConverter_WhateverWorks;
			return converter.ToWideChar(std::basic_string_view<T>(ptr, length));
		}
		else if constexpr(std::is_same_v<T, wchar_t>)
		{
			return std::basic_string_view<T>(ptr, length);
		}
	}
}

namespace kxf::Utility
{
	char* StringBuffer::PrepareNarrowChars()
	{
		auto ptr = PrepareBuffer<char>(m_Buffer, m_Length);
		m_Type = Type::NarrowChars;

		return ptr;
	}
	wchar_t* StringBuffer::PrepareWideChars()
	{
		auto ptr = PrepareBuffer<wchar_t>(m_Buffer, m_Length);
		m_Type = Type::WideChars;

		return ptr;
	}
	void StringBuffer::Finalize()
	{
		switch (m_Type)
		{
			case Type::NarrowChars:
			{
				m_Value = FinalizeBuffer<char>(m_Buffer, m_Length, m_NullTerminated, m_EncodingConverter);
				break;
			}
			case Type::WideChars:
			{
				m_Value = FinalizeBuffer<wchar_t>(m_Buffer, m_Length, m_NullTerminated, m_EncodingConverter);
				break;
			}
		};
	}
}

namespace kxf::Utility
{
	std::optional<bool> ParseBool(const String& value)
	{
		if (value == kxfS("false") || value == kxfS("FALSE"))
		{
			return false;
		}
		else if (value == kxfS("true") || value == kxfS("TRUE"))
		{
			return true;
		}
		else if (auto iValue = value.ToInteger<int>())
		{
			return *iValue != 0;
		}
		else if (auto fValue = value.ToFloatingPoint())
		{
			return *fValue != 0;
		}
		return {};
	}
}
