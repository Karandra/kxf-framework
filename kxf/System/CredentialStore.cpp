#include "KxfPCH.h"
#include "CredentialStore.h"
#include "kxf/Utility/ScopeGuard.h"
#include <Windows.h>
#include <wincred.h>
#include "UndefWindows.h"

namespace kxf
{
	bool CredentialStore::Save(const String& userName, const SecretValue& secret)
	{
		if (secret && secret.GetSize() <= CRED_MAX_CREDENTIAL_BLOB_SIZE)
		{
			CREDENTIALW credentialInfo = {0};
			credentialInfo.Type = CRED_TYPE_GENERIC;
			credentialInfo.Persist = CRED_PERSIST_LOCAL_MACHINE;
			credentialInfo.TargetName = m_ServiceName.wc_str().unsafe_data();
			credentialInfo.UserName = userName.wc_str().unsafe_data();
			credentialInfo.CredentialBlob = static_cast<uint8_t*>(const_cast<void*>(secret.GetData()));
			credentialInfo.CredentialBlobSize = secret.GetSize();

			return ::CredWriteW(&credentialInfo, 0);
		}
		return false;
	}
	bool CredentialStore::Load(String& userName, SecretValue& secret) const
	{
		PCREDENTIALW credentialInfo = nullptr;
		Utility::ScopeGuard atExit([&]()
		{
			::CredFree(credentialInfo);
		});

		bool status = false;
		if (::CredReadW(m_ServiceName.wc_str(), CRED_TYPE_GENERIC, 0, &credentialInfo))
		{
			userName = credentialInfo->UserName;
			secret = SecretValue(credentialInfo->CredentialBlob, credentialInfo->CredentialBlobSize);
			::RtlSecureZeroMemory(credentialInfo->CredentialBlob, credentialInfo->CredentialBlobSize);

			return true;
		}
		return false;
	}
	bool CredentialStore::Delete()
	{
		return ::CredDeleteW(m_ServiceName.wc_str(), CRED_TYPE_GENERIC, 0);
	}
}
