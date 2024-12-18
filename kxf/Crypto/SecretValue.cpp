#include "kxf-pch.h"
#include "SecretValue.h"
#include "kxf/Utility/Memory.h"

namespace kxf
{
	String SecretValue::ToString(IEncodingConverter& encodingConverter) const
	{
		return encodingConverter.ToWideChar<std::byte>(m_Storage);
	}
	void SecretValue::Wipe() noexcept
	{
		if (!m_Storage.empty())
		{
			Utility::SecureZeroMemory(m_Storage.data(), m_Storage.size());
			m_Storage.clear();
		}
	}
}
