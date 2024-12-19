#include "kxf-pch.h"
#include "AnonymousNativeWindow.h"
#include <wx/nativewin.h>

namespace kxf::Private
{
	void AnonymousNativeWindow::CleanUp()
	{
		m_WxWindow = nullptr;
		AnonymousSystemWindow::CleanUp();
	}

	wxWindow* AnonymousNativeWindow::GetWxWindow()
	{
		if (!m_WxWindow && m_Handle)
		{
			m_WxWindow = std::make_unique<wxNativeContainerWindow>(static_cast<HWND>(m_Handle));
		}
		return m_WxWindow.get();
	}

	AnonymousNativeWindow::~AnonymousNativeWindow() = default;
}
