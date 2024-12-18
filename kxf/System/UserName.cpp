#include "kxf-pch.h"
#include "UserName.h"
#include "kxf/Core/UniversallyUniqueID.h"

namespace
{
	constexpr char g_SAMSeparator = '\\';
	constexpr char g_UPNSeparator = '@';
}

namespace kxf
{
	void UserName::AssignName(String name)
	{
		if (name.Contains(g_SAMSeparator))
		{
			name.BeforeFirst(g_SAMSeparator, &m_Domain);
			name.AfterFirst(g_SAMSeparator, &m_Name);
			m_NameFormat = UserNameFormat::DownLevel;
		}
		else if (name.Contains(g_UPNSeparator))
		{
			name.BeforeFirst(g_SAMSeparator, &m_Name);
			name.AfterFirst(g_SAMSeparator, &m_Domain);
			m_NameFormat = UserNameFormat::UserPrincipal;
		}
		else if (auto uuid = UniversallyUniqueID::CreateFromString(name); !uuid.IsNull())
		{
			m_Name = uuid.ToString(UUIDFormat::CurlyBraces);
			m_NameFormat = UserNameFormat::UniqueID;
		}
		else
		{
			m_Name = std::move(name);
			m_NameFormat = UserNameFormat::DownLevel;
		}
	}
	void UserName::AssignDomain(String domain)
	{
		if (!domain.Contains(g_SAMSeparator) && !domain.Contains(g_UPNSeparator))
		{
			m_Domain = std::move(domain);
		}
		else
		{
			m_Domain.clear();
		}
	}

	bool UserName::IsSameAs(const UserName& other) const noexcept
	{
		const bool namesSame = m_NameFormat == other.m_NameFormat && m_Name.IsSameAs(other.m_Name, StringActionFlag::IgnoreCase);
		if (!m_Domain.IsEmpty())
		{
			return namesSame && m_Domain.IsSameAs(other.m_Domain, StringActionFlag::IgnoreCase);
		}
		return namesSame;
	}
	String UserName::GetFullName(UserNameFormat withFormat) const
	{
		if (m_Domain.IsEmpty())
		{
			return m_Name;
		}
		else
		{
			switch (withFormat != UserNameFormat::None ? withFormat : m_NameFormat)
			{
				case UserNameFormat::Display:
				case UserNameFormat::UniqueID:
				{
					return m_Name;
				}
				case UserNameFormat::DownLevel:
				{
					return m_Domain + g_SAMSeparator + m_Name;
				}
				case UserNameFormat::UserPrincipal:
				{
					return m_Name + g_UPNSeparator + m_Domain;
				}
			};
		}
		return {};
	}
}
