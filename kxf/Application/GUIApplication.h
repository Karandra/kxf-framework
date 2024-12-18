#pragma once
#include "Common.h"
#include "IGUIApplication.h"
#include "CoreApplication.h"

namespace kxf
{
	class KXF_API GUIApplication: public RTTI::Implementation<GUIApplication, CoreApplication, IGUIApplication>
	{
		private:
			enum class ExitWhenLastWidgetDestroyed
			{
				Never,
				Always,
				Later
			};

		private:
			std::shared_ptr<ITopLevelWidget> m_TopWidget;
			LayoutDirection m_LayoutDirection = LayoutDirection::Default;
			ExitWhenLastWidgetDestroyed m_ExitWhenLastWidgetDestroyed = ExitWhenLastWidgetDestroyed::Later;
			bool m_IsActive = true;

		protected:
			void DeleteAllTopLevelWindows();

		public:
			GUIApplication() = default;

		public:
			// ICoreApplication
			bool OnCreate() override;
			void OnDestroy() override;
			int OnRun() override;

			// Application::IMainEventLoop
			std::shared_ptr<IEventLoop> CreateMainLoop() override;

			// Application::IActiveEventLoop
			bool DispatchIdle() override;

			// Application::IExceptionHandler
			bool OnMainLoopException() override;

			// IGUIApplication
			std::shared_ptr<ITopLevelWidget> GetTopWidget() const override;
			void SetTopWidget(std::shared_ptr<ITopLevelWidget> widget) override;

			bool ShoudExitWhenLastWidgetDestroyed() const override;
			void ExitWhenLastWidgetDestroyed(bool enable = true) override;

			bool IsActive() const override;
			void SetActive(bool active = true, std::shared_ptr<IWidget> widget = nullptr) override;

			LayoutDirection GetLayoutDirection() const override;
			void SetLayoutDirection(LayoutDirection direction) override;

			String GetNativeTheme() const override;
			bool SetNativeTheme(const String& themeName) override;

			using CoreApplication::Yield;
			bool Yield(IWidget& widget, FlagSet<EventYieldFlag> flags) override;
			bool YieldFor(IWidget& widget, FlagSet<EventCategory> toProcess) override;
	};
}
