#pragma once
#include "Common.h"
#include "kxf/Application/IGUIApplication.h"
#include "kxf/Application/CoreApplication.h"

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
			// GUIApplication
			void DeleteAllTopLevelWindows();

		public:
			GUIApplication() = default;

		public:
			// ICoreApplication -> Main Event Loop
			std::shared_ptr<IEventLoop> CreateMainLoop() override;

			// ICoreApplication -> Active Event Loop
			bool DispatchIdle() override;

			// ICoreApplication -> Exception Handler
			bool OnMainLoopException() override;

			// ICoreApplication -> Application
			bool OnCreate() override;
			void OnDestroy() override;
			int OnRun() override;

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
