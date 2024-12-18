#pragma once
#include "Common.h"
#include "MenuWidgetItem.h"
#include "../IShellMenuWidgetItem.h"

namespace kxf::Widgets
{
	class ShellMenuWidgetItem: public RTTI::Implementation<ShellMenuWidgetItem, MenuWidgetItem, IShellMenuWidgetItem>
	{
		private:
			uint32_t DoGetShellItemID() const;
			String DoGetCommandString(uint32_t stringIndex) const;

		public:
			ShellMenuWidgetItem() = default;
			~ShellMenuWidgetItem() = default;

		public:
			// IWidgetItem
			String GetDescription() const override;

			// IShellMenuWidgetItem
			String GetIconString() const override;
			String GetHelpString() const override;
			String GetCommandString() const override;
			uint32_t GetShellItemID() const override;

			HResult InvokeShellCommand(const IWidget* widget = nullptr, const String& commandLine = {}, const FSPath& workingDirectory = {}) override;
	};
}
