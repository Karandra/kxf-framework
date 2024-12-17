#include "KxfPCH.h"
#include "Common.h"
#include "kxf/Core/String.h"
#include "Private/NamespacePrefix.h"

namespace kxf::FileSystem
{
	StringView GetNamespaceString(FSPathNamespace type)
	{
		using namespace Private;

		switch (type)
		{
			case FSPathNamespace::NT:
			{
				return NamespacePrefix::NT;
			}
			case FSPathNamespace::Win32File:
			{
				return NamespacePrefix::Win32File;
			}
			case FSPathNamespace::Win32FileUNC:
			{
				return NamespacePrefix::Win32FileUNC;
			}
			case FSPathNamespace::Win32Device:
			{
				return NamespacePrefix::Win32Device;
			}
			case FSPathNamespace::Network:
			{
				return NamespacePrefix::Network;
			}
			case FSPathNamespace::NetworkUNC:
			{
				return NamespacePrefix::NetworkUNC;
			}
		};
		return {};
	}
}
