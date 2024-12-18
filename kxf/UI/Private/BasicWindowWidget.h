#pragma once
#include "Common.h"
#include "BasicWxWidget.h"
#include "../ITopLevelWidget.h"
class wxTopLevelWindow;

namespace kxf::Private
{
	class KXF_API BasicWindowWidgetBase: public BasicWxWidgetBase
	{
		private:
			bool m_PreventApplicationExit = true;

		public:
			BasicWindowWidgetBase(IWidget& widget) noexcept
				:BasicWxWidgetBase(widget)
			{
			}
			~BasicWindowWidgetBase() = default;

		public:
			// BasicWxWidgetBase
			wxTopLevelWindow* GetWxWindow() const noexcept;

		public:
			// BasicTopLevelWindowWidgetBase
			BitmapImage GetIcon() const;
			void SetIcon(const BitmapImage& icon);

			ImageBundle GetIconPack() const;
			void SetIconPack(const ImageBundle& icon);

			String GetTitle() const;
			void SetTitle(const String& title);

			// State
			bool IsMinimized() const;
			void Minimize();

			bool IsMaximized() const;
			void Maximize();
			void Restore();

			bool EnableCloseButton(bool enable);
			bool EnableMinimizeButton(bool enable);
			bool EnableMaximizeButton(bool enable);

			bool IsFullScreen() const;
			bool ShowFullScreen(bool show);
			void ShowWithoutActivation();

			// Misc
			std::shared_ptr<IWidget> GetDefaultWidget() const;
			void SetDefaultWidget(const IWidget& widget);

			std::shared_ptr<IMenuWidget> GetSystemMenu() const;
			void RequestUserAttention(FlagSet<StdIcon> icon);

			bool ShouldPreventApplicationExit() const;
			void SetPreventApplicationExit(bool enable);
	};
}

namespace kxf::Private
{
	template<class TDerived, class TWindow, class TInterface, class TWindowImpl = BasicWindowWidgetBase>
	class BasicWindowWidget: public BasicWxWidget<TDerived, TWindow, TInterface, TWindowImpl>
	{
		protected:
			using BasicWxWidget<TDerived, TWindow, TInterface, TWindowImpl>::GetImpl;

		public:
			// Icon and title
			BitmapImage GetIcon() const override
			{
				return GetImpl().GetIcon();
			}
			void SetIcon(const BitmapImage& icon) override
			{
				GetImpl().SetIcon(icon);
			}

			ImageBundle GetIconPack() const override
			{
				return GetImpl().GetIconPack();
			}
			void SetIconPack(const ImageBundle& iconPack) override
			{
				GetImpl().SetIconPack(iconPack);
			}

			String GetTitle() const override
			{
				return GetImpl().GetTitle();
			}
			void SetTitle(const String& title) override
			{
				GetImpl().SetTitle(title);
			}

			// State
			bool IsMinimized() const override
			{
				return GetImpl().IsMinimized();
			}
			void Minimize() override
			{
				GetImpl().Minimize();
			}

			bool IsMaximized() const override
			{
				return GetImpl().IsMaximized();
			}
			void Maximize() override
			{
				GetImpl().Maximize();
			}
			void Restore() override
			{
				GetImpl().Restore();
			}

			bool EnableCloseButton(bool enable = true) override
			{
				return GetImpl().EnableCloseButton(enable);
			}
			bool EnableMinimizeButton(bool enable = true) override
			{
				return GetImpl().EnableMinimizeButton(enable);
			}
			bool EnableMaximizeButton(bool enable = true) override
			{
				return GetImpl().EnableMaximizeButton(enable);
			}

			bool IsFullScreen() const override
			{
				return GetImpl().IsFullScreen();
			}
			bool ShowFullScreen(bool show = true) override
			{
				return GetImpl().ShowFullScreen(show);
			}
			void ShowWithoutActivation() override
			{
				GetImpl().ShowWithoutActivation();
			}

			// Misc
			std::shared_ptr<IWidget> GetDefaultWidget() const override
			{
				return GetImpl().GetDefaultWidget();
			}
			void SetDefaultWidget(const IWidget& widget) override
			{
				GetImpl().SetDefaultWidget(widget);
			}

			std::shared_ptr<IMenuWidget> GetSystemMenu() const override
			{
				return GetImpl().GetSystemMenu();
			}
			void RequestUserAttention(FlagSet<StdIcon> icon = StdIcon::Information) override
			{
				GetImpl().RequestUserAttention(icon);
			}

			bool ShouldPreventApplicationExit() const override
			{
				return GetImpl().ShouldPreventApplicationExit();
			}
			void SetPreventApplicationExit(bool enable = true) override
			{
				GetImpl().SetPreventApplicationExit(enable);
			}
	};
}
