#include "kxf-pch.h"
#include "PasswordHandler.h"
#include "Utility.h"

#include "kxf/System/Private/BeginIncludeCOM.h"
namespace
{
	kxf_MakeWinUnicodeCallWrapper(FormatMessage);
}
#include "kxf/System/Private/EndIncludeCOM.h"

namespace kxf::SevenZip::Private
{
	HResult PasswordHandler::OnPasswordRequest(BSTR* password, Int32* passwordIsDefined)
	{
		if (!password)
		{
			return HResult::InvalidPointer();
		}
		if (m_EvtHandler && m_EventID != IEvent::EvtNull)
		{
			ArchiveEvent event = CreateEvent();
			if (!SendEvent(event, m_EventID))
			{
				return HResult::Abort();
			}
				
			String value = std::move(event).GetPassword().ToString();
			if (!value.IsEmpty())
			{
				*password = _bstr_t(value.wc_str()).Detach();
				Utility::SetIfNotNull(passwordIsDefined, 1);
			}
			else
			{
				*password = nullptr;
				Utility::SetIfNotNull(passwordIsDefined, 0);
			}
		}
		return HResult::Success();
	}
}
