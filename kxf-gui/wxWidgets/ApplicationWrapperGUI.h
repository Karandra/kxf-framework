#pragma once
#include "kxf/wxWidgets/ApplicationWrapper.h"
#include "kxf/Application/IGUIApplication.h"

namespace kxf::wxWidgets
{
	class KXF_API ApplicationWrapperGUI final: public ApplicationWrapperCommon<ApplicationWrapperGUI, ApplicationGUI>
	{
		private:
			IGUIApplication& m_GUIApp;

		public:
			ApplicationWrapperGUI(ICoreApplication& app, IGUIApplication& guiApp)
				:ApplicationWrapperCommon(app), m_GUIApp(guiApp)
			{
			}

		public:
			bool IsActive() const override
			{
				return m_GUIApp.IsActive();
			}
			wxWindow* GetTopWindow() const override;
			
			wxLayoutDirection GetLayoutDirection() const override
			{
				switch (m_GUIApp.GetLayoutDirection())
				{
					case LayoutDirection::LeftToRight:
					{
						return wxLayoutDirection::wxLayout_LeftToRight;
					}
					case LayoutDirection::RightToLeft:
					{
						return wxLayoutDirection::wxLayout_RightToLeft;
					}
				};
				return wxLayoutDirection::wxLayout_Default;
			}
			bool SetNativeTheme(const wxString& themeName) override
			{
				return m_GUIApp.SetNativeTheme(themeName);
			}

			bool SafeYield(wxWindow* window, bool onlyIfNeeded) override;
			bool SafeYieldFor(wxWindow* window, long eventsToProcess) override;
	};
}
