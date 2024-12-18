#pragma once
#include <wx/wx.h>

namespace kxf::FileSystem::Private::NamespacePrefix
{
	constexpr XChar NT[] = kxfS("\\");
	constexpr XChar Win32File[] = kxfS("\\\\?\\");
	constexpr XChar Win32FileUNC[] = kxfS("\\\\?\\UNC\\");
	constexpr XChar Win32Device[] = kxfS("\\\\.\\");
	constexpr XChar Network[] = kxfS("\\\\");
	constexpr XChar NetworkUNC[] = kxfS("\\\\?\\UNC\\");

	// Not strictly namespaces, but useful
	constexpr XChar DotRelative1[] = kxfS(".\\");
	constexpr XChar DotRelative2[] = kxfS("..\\");
	constexpr XChar DotRelative3[] = kxfS("./");
	constexpr XChar DotRelative4[] = kxfS("..//");
	constexpr XChar DotRelative5[] = kxfS("\\.");
	constexpr XChar DotRelative6[] = kxfS("..");
}
