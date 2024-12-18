#pragma once
#include "Common.h"
#include "SecretValue.h"
#include "kxf/System/UserName.h"

namespace kxf
{
	class KXF_API UserCredentials: public UserName
	{
		private:
			SecretValue m_Secret;

		public:
			UserCredentials() = default;
			UserCredentials(String name, SecretValue secret = {})
				:UserName(std::move(name)), m_Secret(std::move(secret))
			{
			}
			UserCredentials(UserCredentials&&) noexcept = default;
			UserCredentials(const UserCredentials&) = delete;

		public:
			bool HasSecret() const noexcept
			{
				return !m_Secret.IsEmpty();
			}
			SecretValue GetSecret() noexcept
			{
				return std::move(m_Secret);
			}
			const SecretValue& GetSecret() const noexcept
			{
				return m_Secret;
			}
			void SetSecret(SecretValue secret) noexcept
			{
				m_Secret = std::move(secret);
			}
			void WipeSecret()
			{
				m_Secret.Wipe();
			}

		public:
			UserCredentials& operator=(UserCredentials&&) noexcept = default;
			UserCredentials& operator=(const UserCredentials&) = delete;
	};
}
