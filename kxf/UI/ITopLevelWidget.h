#pragma once
#include "Common.h"
#include "IWidget.h"
#include "kxf/Drawing/ImageBundle.h"

namespace kxf
{
	class IMenuWidget;
}

namespace kxf
{
	class KXF_API ITopLevelWidget: public RTTI::ExtendInterface<ITopLevelWidget, IWidget>
	{
		kxf_RTTI_DeclareIID(ITopLevelWidget, {0x66b86a18, 0x9717, 0x4497, {0xbe, 0xbf, 0xb1, 0x64, 0x88, 0x73, 0x40, 0xde}});

		public:
			virtual ~ITopLevelWidget() = default;

		public:
			// Icon and title
			virtual BitmapImage GetIcon() const = 0;
			virtual void SetIcon(const BitmapImage& icon) = 0;

			virtual ImageBundle GetIconPack() const = 0;
			virtual void SetIconPack(const ImageBundle& iconPack) = 0;

			virtual String GetTitle() const = 0;
			virtual void SetTitle(const String& title) = 0;

			// State
			virtual bool IsMinimized() const = 0;
			virtual void Minimize() = 0;

			virtual bool IsMaximized() const = 0;
			virtual void Maximize() = 0;
			virtual void Restore() = 0;

			virtual bool EnableCloseButton(bool enable = true) = 0;
			virtual bool EnableMinimizeButton(bool enable = true) = 0;
			virtual bool EnableMaximizeButton(bool enable = true) = 0;

			virtual bool IsFullScreen() const = 0;
			virtual bool ShowFullScreen(bool show = true) = 0;
			virtual void ShowWithoutActivation() = 0;

			// Misc
			virtual std::shared_ptr<IWidget> GetDefaultWidget() const = 0;
			virtual void SetDefaultWidget(const IWidget& widget) = 0;

			virtual std::shared_ptr<IMenuWidget> GetSystemMenu() const = 0;
			virtual void RequestUserAttention(FlagSet<StdIcon> icon = StdIcon::Information) = 0;

			virtual bool ShouldPreventApplicationExit() const = 0;
			virtual void SetPreventApplicationExit(bool enable = true) = 0;
	};
}
