#pragma once
#include <Windows.h>
#include "kxf/Win32/UndefMacros.h"

namespace
{
	inline int MessageBox(HWND hwnd, const TCHAR* caption, const TCHAR* message, uint32_t flags)
	{
		return ::MessageBoxW(hwnd, caption, message, flags);
	}
}

#include <sciter-js/sciter-x.h>
#include <sciter-js/sciter-x-api.h>
#include <sciter-js/sciter-x-behavior.h>

namespace kxf::Sciter
{
	ISciterAPI* GetSciterAPI();
	SciterGraphicsAPI* GetGrapchicsAPI();
	SciterRequestAPI* GetRequestAPI();
}
