#include "KxfPCH.h"
#include "CredentialsDialog.h"
#include "kxf/Utility/Memory.h"
#include "kxf/Utility/ScopeGuard.h"
#include "kxf/System/COM.h"
#include "kxf/System/Win32Error.h"
#include <wincred.h>
#pragma comment(lib, "CredUI.lib")

namespace
{
	const kxf::XChar* GetStringOrNull(const kxf::String& text)
	{
		return text.IsEmpty() ? nullptr : text.wc_str();
	}
}

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(CredentialsDialog, StdDialog);

	bool CredentialsDialog::Create(wxWindow* parent,
								   wxWindowID id,
								   const String& caption,
								   const String& message
	)
	{
		if (Dialog::Create(parent, id, caption, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), Dialog::DefaultStyle))
		{
			m_Parent = parent;
			m_Caption = caption;
			m_Message = message;

			return true;
		}
		return false;
	}

	int CredentialsDialog::ShowModal()
	{
		m_UserName.clear();
		m_Password.Wipe();

		CREDUI_INFOW credentialInfoUI = {};
		credentialInfoUI.cbSize = sizeof(credentialInfoUI);
		credentialInfoUI.hwndParent = m_Parent ? wxGetTopLevelParent(m_Parent)->GetHandle() : nullptr;
		credentialInfoUI.pszCaptionText = m_Caption.wc_str();
		credentialInfoUI.pszMessageText = m_Message.wc_str();
		credentialInfoUI.hbmBanner = nullptr;

		ULONG authPackage = 0;
		void* inAuthBlob = nullptr;
		ULONG inAuthBlobSize = 0;
		COMMemoryPtr<uint8_t> authBlob;
		ULONG authBlobSize = 0;
		BOOL saveCredentials = m_SaveCredentials;
		Utility::ScopeGuard zeroAuthBlob = ([&]()
		{
			if (authBlob)
			{
				Utility::SecureZeroMemory(authBlob, authBlobSize);
			}
		});

		Win32Error status = ::CredUIPromptForWindowsCredentialsW(&credentialInfoUI,
																 0,
																 &authPackage,
																 inAuthBlob,
																 inAuthBlobSize,
																 reinterpret_cast<void**>(&authBlob),
																 &authBlobSize,
																 m_EnableSaveCredentialsCheckBox ? &saveCredentials : nullptr,
																 CREDUIWIN_GENERIC
		);
		if (status)
		{
			// Save the checkbox status
			if (m_EnableSaveCredentialsCheckBox)
			{
				m_SaveCredentials = saveCredentials;
			}

			// Retrieve data
			constexpr DWORD nameLengthMax = CREDUI_MAX_USERNAME_LENGTH + 1;
			constexpr DWORD passwordLengthMax = CREDUI_MAX_PASSWORD_LENGTH + 1;
			constexpr DWORD domainLengthMax = CREDUI_MAX_DOMAIN_TARGET_LENGTH + 1;
			DWORD nameLength = nameLengthMax;
			DWORD passwordLength = passwordLengthMax;
			DWORD domainLength = domainLengthMax;

			wchar_t name[nameLengthMax] = {};
			wchar_t password[passwordLengthMax] = {};
			wchar_t domain[domainLengthMax] = {};
			Utility::ScopeGuard zeroCredentials = ([&]()
			{
				Utility::SecureZeroMemory(name, sizeof(name));
				Utility::SecureZeroMemory(password, sizeof(password));
				Utility::SecureZeroMemory(domain, sizeof(domain));
			});

			BOOL unpacked = ::CredUnPackAuthenticationBufferW(CRED_PACK_PROTECTED_CREDENTIALS,
															  authBlob,
															  authBlobSize,
															  name,
															  &nameLength,
															  domain,
															  &domainLength,
															  password,
															  &passwordLength
			);
			if (unpacked)
			{
				m_UserName = String(name, nameLength);
				m_Password = SecretValue(String(password, passwordLength));
			}
		}
		return status.IsSuccess() ? wxID_OK : wxID_CANCEL;
	}
}
