#pragma once
#include "Common.h"
#include "kxf/Crypto/SecretValue.h"
#include "kxf/Crypto/ISecretStore.h"

namespace kxf
{
	class KXF_API CredentialStore: public ISecretStore
	{
		private:
			String m_ServiceName;

		public:
			CredentialStore(String serviceName) noexcept
				:m_ServiceName(std::move(serviceName))
			{
			}
			virtual ~CredentialStore() noexcept = default;

		public:
			String GetServiceName() const override
			{
				return m_ServiceName;
			}

			bool Save(const String& userName, const SecretValue& secret) override = 0;
			bool Load(String& userName, SecretValue& secret) const override = 0;
			bool Delete() override = 0;
	};
}
