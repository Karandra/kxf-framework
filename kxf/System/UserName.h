#pragma once
#include "Common.h"

namespace kxf
{
	class KXF_API UserName
	{
		private:
			String m_Name;
			String m_Domain;
			UserNameFormat m_NameFormat = UserNameFormat::None;

		private:
			void AssignName(String name);
			void AssignDomain(String domain);

		public:
			UserName() = default;
			UserName(String name)
			{
				AssignName(std::move(name));
			}
			UserName(UserName&&) noexcept = default;
			UserName(const UserName&) = default;
			virtual ~UserName() = default;

		public:
			bool IsEmpty() const noexcept
			{
				return m_Name.IsEmpty();
			}
			bool IsSameAs(const UserName& other) const noexcept;

			String GetName() const
			{
				return m_Name;
			}
			void SetName(String name)
			{
				AssignName(std::move(name));
			}

			bool HasDomain() const noexcept
			{
				return !m_Domain.IsEmpty();
			}
			String GetDomain() const
			{
				return m_Name;
			}
			void SetDomain(String domain)
			{
				AssignDomain(std::move(domain));
			}

			UserNameFormat GetNameFormat() const noexcept
			{
				return m_NameFormat;
			}
			String GetFullName(UserNameFormat withFormat = UserNameFormat::None) const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsEmpty();
			}
			bool operator!() const noexcept
			{
				return IsEmpty();
			}

			UserName& operator=(UserName&&) noexcept = default;
			UserName& operator=(const UserName&) = default;
	};
}
