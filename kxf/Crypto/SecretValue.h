#pragma once
#include "Common.h"
#include "kxf/Core/String.h"
#include "kxf/Core/IEncodingConverter.h"
#include <vector>

namespace kxf
{
	class KX_API SecretValue final
	{
		private:
			std::vector<std::byte> m_Storage;

		private:
			void AssignBytes(void* ptr, size_t size)
			{
				m_Storage.resize(size);
				std::memcpy(m_Storage.data(), ptr, size);
			}

		public:
			SecretValue() noexcept = default;
			SecretValue(void* ptr, size_t size)
			{
				AssignBytes(ptr, size);
			}
			SecretValue(const String& string)
			{
				auto utf8 = string.ToUTF8();
				AssignBytes(utf8.data(), utf8.size());
			}
			SecretValue(const SecretValue&) = delete;
			SecretValue(SecretValue&& other) noexcept
			{
				*this = std::move(other);
			}
			~SecretValue() noexcept
			{
				Wipe();
			}

		public:
			bool IsEmpty() const
			{
				return m_Storage.empty();
			}
			size_t GetSize() const noexcept
			{
				return m_Storage.size();
			}
			const void* GetData() const noexcept
			{
				return m_Storage.data();
			}
			
			String ToString(IEncodingConverter& encodingConverter = EncodingConverter_WhateverWorks) const;
			void Wipe() noexcept;

		public:
			SecretValue& operator=(const SecretValue&) = delete;
			SecretValue& operator=(SecretValue&& other) noexcept
			{
				m_Storage = std::move(other.m_Storage);
				other.Wipe();
				return *this;
			}

			bool operator==(const SecretValue& other) const noexcept
			{
				return m_Storage == other.m_Storage;
			}
			bool operator!=(const SecretValue& other) const noexcept
			{
				return !(*this == other);
			}

			explicit operator bool() const noexcept
			{
				return !IsEmpty();
			}
			bool operator!() const noexcept
			{
				return IsEmpty();
			}
	};
}
