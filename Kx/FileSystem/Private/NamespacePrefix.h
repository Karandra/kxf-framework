#pragma once
#include <wx/wx.h>

namespace KxFramework::FileSystem::Private::NamespacePrefix
{
	constexpr wxChar NT[] = wxS("\\");
	constexpr wxChar Win32File[] = wxS("\\\\?\\");
	constexpr wxChar Win32FileUNC[] = wxS("\\\\?\\UNC\\");
	constexpr wxChar Win32Device[] = wxS("\\\\.\\");
	constexpr wxChar Network[] = wxS("\\\\");
	constexpr wxChar NetworkUNC[] = wxS("\\\\?\\UNC\\");
}