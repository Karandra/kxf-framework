#pragma once
#include "../Common.h"
#include <wx/menu.h>

namespace kxf::Widgets
{
	class MenuWidget;
}

namespace kxf::WXUI
{
	class KXF_API Menu: public wxMenu
	{
		public:
			static constexpr uint16_t WxIDToWin(int menuWxID) noexcept
			{
				return static_cast<uint32_t>(menuWxID);
			}
			static constexpr kxf::WidgetID WinIDToWx(uint16_t menuWinID) noexcept
			{
				return static_cast<int16_t>(menuWinID);
			}
			static constexpr kxf::WidgetID WinMenuRetToWx(int id) noexcept
			{
				if (id != 0)
				{
					return WinIDToWx(static_cast<uint16_t>(id));
				}
				return {};
			}

		private:
			Widgets::MenuWidget& m_Widget;

		public:
			Menu(Widgets::MenuWidget& widget) noexcept;
			~Menu();
	};
}
