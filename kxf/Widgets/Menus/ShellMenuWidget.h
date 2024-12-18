#pragma once
#include "Common.h"
#include "MenuWidget.h"
#include "../IShellMenuWidget.h"
#include "kxf/System/COM.h"
struct IContextMenu;
struct IShellFolder;
struct _ITEMIDLIST;

namespace kxf::Widgets
{
	class ShellMenuWidget: public RTTI::Implementation<ShellMenuWidget, MenuWidget, IShellMenuWidget>
	{
		friend class ShellMenuWidgetItem;

		private:
			COMInitGuard m_Initializer;

			COMPtr<::IContextMenu> m_ShellMenu;
			COMPtr<IShellFolder> m_ShellFolder;
			COMMemoryPtr<_ITEMIDLIST> m_ShellItemList;
			COMMemoryPtr<const _ITEMIDLIST> m_ShellChildItemList;

		private:
			void CopyItems(IMenuWidget& menu, void* menuHandle, size_t itemCount);

		protected:
			// MenuWidget
			std::shared_ptr<MenuWidgetItem> DoCreateItem() override;

		public:
			ShellMenuWidget();
			~ShellMenuWidget();

		public:
			// IWidget
			bool DestroyWidget() override;

			// IShellMenuWidget
			HResult InitializeFromFSObject(const FSPath& path) override;
	};
}
