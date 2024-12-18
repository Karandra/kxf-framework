#pragma once
#include "Common.h"
#include "kxf/System/HResult.h"
#include "kxf/FileSystem/FSPath.h"

namespace kxf
{
	class IWidget;
}

namespace kxf
{
	class KXF_API IShellMenuWidgetItem: public RTTI::Interface<IShellMenuWidgetItem>
	{
		kxf_RTTI_DeclareIID(IShellMenuWidgetItem, {0xf8079dff, 0xa14f, 0x4e98, {0x89, 0x7b, 0xa2, 0x99, 0x5f, 0xa1, 0xce, 0x6c}});

		public:
			virtual String GetIconString() const = 0;
			virtual String GetHelpString() const = 0;
			virtual String GetCommandString() const = 0;
			virtual uint32_t GetShellItemID() const = 0;

			virtual HResult InvokeShellCommand(const IWidget* widget = nullptr, const String& commandLine = {}, const FSPath& workingDirectory = {}) = 0;
	};
}
