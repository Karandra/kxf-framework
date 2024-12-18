#pragma once
#include "Common.h"
#include "ICoreApplication.h"

namespace kxf
{
	class IWidget;
	class ITopLevelWidget;
}

namespace kxf
{
	class KXF_API IGUIApplication: public RTTI::Interface<IGUIApplication>
	{
		kxf_RTTI_DeclareIID(IGUIApplication, {0x9a8d298a, 0xbaa2, 0x4a7b, {0xb1, 0x83, 0x67, 0x8a, 0xf1, 0xfb, 0x0, 0x9e}});

		public:
			static IGUIApplication* GetInstance() noexcept
			{
				if (auto app = ICoreApplication::GetInstance())
				{
					return app->QueryInterface<IGUIApplication>().get();
				}
				return nullptr;
			}
			static void SetInstance(IGUIApplication* instance) noexcept
			{
				ICoreApplication::SetInstance(instance ? instance->QueryInterface<ICoreApplication>().get() : nullptr);
			}

		public:
			virtual ~IGUIApplication() = default;

		public:
			virtual std::shared_ptr<ITopLevelWidget> GetTopWidget() const = 0;
			virtual void SetTopWidget(std::shared_ptr<ITopLevelWidget> widget) = 0;

			virtual bool ShoudExitWhenLastWidgetDestroyed() const = 0;
			virtual void ExitWhenLastWidgetDestroyed(bool enable = true) = 0;

			virtual bool IsActive() const = 0;
			virtual void SetActive(bool active = true, std::shared_ptr<IWidget> widget = nullptr) = 0;

			virtual LayoutDirection GetLayoutDirection() const = 0;
			virtual void SetLayoutDirection(LayoutDirection direction) = 0;

			virtual String GetNativeTheme() const = 0;
			virtual bool SetNativeTheme(const String& themeName) = 0;

			virtual bool Yield(IWidget& widget, FlagSet<EventYieldFlag> flags) = 0;
			virtual bool YieldFor(IWidget& widget, FlagSet<EventCategory> toProcess) = 0;
	};
}
