#include "kxf-pch.h"
#include "TopLevelWindow.h"
#include "kxf/System/NativeAPI.h"
#include "kxf/System/SystemWindow.h" 
#include "kxf/Drawing/BitmapImage.h"
#include <DWMAPI.h>
#include "kxf/System/UndefWindows.h"
#undef DWMAPI

namespace kxf::UI::Private
{
	bool TopLevelWindowBase::DWMIsCompositionEnabled()
	{
		BOOL result = FALSE;
		if (NativeAPI::DWMAPI::DwmIsCompositionEnabled)
		{
			NativeAPI::DWMAPI::DwmIsCompositionEnabled(&result);
		}
		return result;
	}
	bool TopLevelWindowBase::DWMIsGlassOpaque()
	{
		uint32_t dwmColor = 0;
		BOOL isOpaqueColor = FALSE;
		if (NativeAPI::DWMAPI::DwmGetColorizationColor)
		{
			NativeAPI::DWMAPI::DwmGetColorizationColor(&dwmColor, &isOpaqueColor);
		}
		return isOpaqueColor;
	}
	Color TopLevelWindowBase::DWMGetGlassColor()
	{
		Color color;
		if (NativeAPI::DWMAPI::DwmGetColorizationColor)
		{
			uint32_t dwmColor = 0;
			BOOL isOpaqueColor = FALSE;
			NativeAPI::DWMAPI::DwmGetColorizationColor(&dwmColor, &isOpaqueColor);

			color.SetARGB(dwmColor);
		}
		return color;
	}

	Color TopLevelWindowBase::DWMGetColorKey() const
	{
		wxWindowDC dc(m_Window);
		BitmapImage image = dc.GetAsBitmap().ConvertToImage();
		if (auto color = image.FindFirstUnusedColour())
		{
			return *color;
		}
		return {};
	}
	bool TopLevelWindowBase::DWMExtendFrame()
	{
		if (NativeAPI::DWMAPI::DwmExtendFrameIntoClientArea)
		{
			SystemWindow window = m_Window->GetHandle();
			window.ModWindowStyle(GWL_EXSTYLE, WS_EX_LAYERED, false);

			MARGINS margins = {};
			return NativeAPI::DWMAPI::DwmExtendFrameIntoClientArea(window.GetHandle(), &margins) == S_OK;
		}
		return false;
	}
	bool TopLevelWindowBase::DWMExtendFrame(const Rect& rect, const Color& color)
	{
		if (NativeAPI::DWMAPI::DwmExtendFrameIntoClientArea)
		{
			SystemWindow window = m_Window->GetHandle();
			window.ModWindowStyle(GWL_EXSTYLE, WS_EX_LAYERED, true);

			if (color)
			{
				::SetLayeredWindowAttributes(reinterpret_cast<HWND>(window.GetHandle()), color.GetCOLORREF(), color.GetFixed8().Alpha, LWA_COLORKEY);
			}
			else
			{
				::SetLayeredWindowAttributes(reinterpret_cast<HWND>(window.GetHandle()), color.GetCOLORREF(), 0, 0);
			}

			MARGINS margins = {};
			margins.cxLeftWidth = rect.GetX();
			margins.cxRightWidth = rect.GetY();
			margins.cyTopHeight = rect.GetWidth();
			margins.cyBottomHeight = rect.GetHeight();
			return NativeAPI::DWMAPI::DwmExtendFrameIntoClientArea(window.GetHandle(), &margins) == S_OK;
		}
		return false;
	}
	bool TopLevelWindowBase::DWMBlurBehind(bool enable, const wxRegion& region)
	{
		if (NativeAPI::DWMAPI::DwmEnableBlurBehindWindow)
		{
			DWM_BLURBEHIND blurInfo = {};
			blurInfo.fEnable = enable;
			blurInfo.fTransitionOnMaximized = TRUE;
			blurInfo.dwFlags = DWM_BB_ENABLE|DWM_BB_TRANSITIONONMAXIMIZED;
			if (!region.IsEmpty())
			{
				blurInfo.dwFlags |= DWM_BB_BLURREGION;
				blurInfo.hRgnBlur = region.GetHRGN();
			}

			return NativeAPI::DWMAPI::DwmEnableBlurBehindWindow(m_Window->GetHandle(), &blurInfo);
		}
		return false;
	}

	void TopLevelWindowBase::SetDefaultBackgroundColor()
	{
		// White: wxSYS_COLOUR_WINDOW, Gray: wxSYS_COLOUR_FRAMEBK
		m_Window->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
	}
	bool TopLevelWindowBase::MSWWindowProc(WXLRESULT& result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
	{
		HWND handle = m_Window->GetHandle();

		switch (msg)
		{
			case WM_NCCREATE:
			{
				if (NativeAPI::User32::EnableNonClientDpiScaling)
				{
					NativeAPI::User32::EnableNonClientDpiScaling(handle);
				}
				return false;
			}
			case WM_DWMCOLORIZATIONCOLORCHANGED:
			{
				wxNotifyEvent event(EvtDWMGlassColorChanged->AsInt());
				event.SetEventObject(m_Window);
				m_Window->HandleWindowEvent(event);

				result = 0;
				return true;
			}
			case WM_DWMCOMPOSITIONCHANGED:
			{
				wxNotifyEvent event(EvtDWMCompositionChanged->AsInt());
				event.SetEventObject(m_Window);
				event.SetInt(DWMIsCompositionEnabled());
				m_Window->HandleWindowEvent(event);

				result = 0;
				return true;
			}
		};

		return false;
	}

	GDIIcon TopLevelWindowBase::GetTitleIcon() const
	{
		HWND handle = m_Window->GetHandle();
		HICON iconHandle = reinterpret_cast<HICON>(::SendMessageW(handle, WM_GETICON, ICON_BIG, 0));
		if (!iconHandle)
		{
			iconHandle = reinterpret_cast<HICON>(::SendMessageW(handle, WM_GETICON, ICON_SMALL, 0));
			if (iconHandle)
			{
				GDIIcon icon;
				icon.AttachHandle(iconHandle);
				return icon;
			}
		}
		return {};
	}
	void TopLevelWindowBase::SetTitleIcon(const GDIIcon& icon)
	{
		HWND handle = m_Window->GetHandle();
		if (icon)
		{
			::SendMessageW(handle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon.GetHandle()));
			::SendMessageW(handle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon.GetHandle()));
		}
		else
		{
			::SendMessageW(handle, WM_SETICON, ICON_BIG, 0);
			::SendMessageW(handle, WM_SETICON, ICON_SMALL, 0);
		}
	}

	bool TopLevelWindowBase::EnableCloseButton(bool enable)
	{
		HWND handle = m_Window->GetHandle();

		UINT options = 0;
		if (enable)
		{
			options = MF_ENABLED;
		}
		else
		{
			options = MF_DISABLED|MF_GRAYED;
		}
		return ::EnableMenuItem(::GetSystemMenu(handle, FALSE), SC_CLOSE, MF_BYCOMMAND|options);
	}
	bool TopLevelWindowBase::EnableMinimizeButton(bool enable)
	{
		SystemWindow window = m_Window->GetHandle();
		return window.ModWindowStyle(GWL_STYLE, WS_MINIMIZEBOX, enable).has_value();
	}
	bool TopLevelWindowBase::EnableMaximizeButton(bool enable)
	{
		SystemWindow window = m_Window->GetHandle();
		return window.ModWindowStyle(GWL_STYLE, WS_MAXIMIZEBOX, enable).has_value();
	}

	void* TopLevelWindowBase::GetWindowUserData() const
	{
		HWND handle = m_Window->GetHandle();
		#ifdef _WIN64
		return reinterpret_cast<void*>(GetWindowLongPtrW(handle, GWLP_USERDATA));
		#else
		return reinterpret_cast<void*>(GetWindowLongW(handle, GWL_USERDATA));
		#endif
	}
	void TopLevelWindowBase::SetWindowUserData(const void* data)
	{
		HWND handle = m_Window->GetHandle();
		#ifdef _WIN64
		SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data));
		#else
		SetWindowLongW(handle, GWLP_USERDATA, reinterpret_cast<LONG>(data));
		#endif
	}
}
