#pragma once
#include "Common.h"
#include "kxf/Core/String.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class SecretValue;
}

namespace kxf
{
	class KXF_API ISecretStore: public RTTI::Interface<ISecretStore>
	{
		kxf_RTTI_DeclareIID(ISecretStore, {0xd09a26e9, 0x60c3, 0x405b, {0x84, 0xcb, 0x64, 0x89, 0x38, 0x61, 0x2d, 0x45}});

		public:
			virtual ~ISecretStore() = default;

		public:
			virtual String GetServiceName() const = 0;

			virtual bool Save(const String& userName, const SecretValue& secret) = 0;
			virtual bool Load(String& userName, SecretValue& secret) const = 0;
			virtual bool Delete() = 0;
	};
}
