#pragma once
#include "kxf-gui/Widgets/Common.h"
#include "StdDialog.h"
#include "kxf/Crypto/SecretValue.h"

namespace kxf::UI
{
	class KXF_API CredentialsDialog: public Dialog
	{
		private:
			wxWindow* m_Parent = nullptr;
			String m_Caption;
			String m_Message;

			String m_UserName;
			SecretValue m_Password;

			bool m_SaveCredentials = false;
			bool m_EnableSaveCredentialsCheckBox = false;

		public:
			CredentialsDialog() = default;
			CredentialsDialog(wxWindow* parent,
							  wxWindowID id,
							  const String& caption,
							  const String& message
			)
			{
				Create(parent, id, caption, message);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& caption,
						const String& message
			);

		public:
			String GetCaption() const
			{
				return m_Caption;
			}
			void SetCaption(const String& value)
			{
				m_Caption = value;
			}
		
			String GetMessage() const
			{
				return m_Message;
			}
			void SetMessage(const String& value)
			{
				m_Message = value;
			}

			wxString GetTitle() const override
			{
				return GetCaption();
			}
			void SetTitle(const wxString& value) override
			{
				SetCaption(value);
			}
		
			wxString GetLabel() const override
			{
				return GetCaption();
			}
			void SetLabel(const wxString& value) override
			{
				SetCaption(value);
			}

			bool ShouldSaveCredentials() const
			{
				return m_SaveCredentials;
			}
			void SaveCredentials(bool save = true)
			{
				m_SaveCredentials = save;
			}

			bool IsSaveCredentialsCheckBoxEnabled() const
			{
				return m_EnableSaveCredentialsCheckBox;
			}
			void EnableSaveCredentialsCheckBox(bool enable = true)
			{
				m_EnableSaveCredentialsCheckBox = enable;
			}

			int ShowModal() override;

			String GetUserName() const
			{
				return m_UserName;
			}
			SecretValue GetPassword()
			{
				return std::move(m_Password);
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(CredentialsDialog);
	};
}
