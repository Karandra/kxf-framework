#pragma once
#include "Common.h"
#include "IWidget.h"
#include "Private/WidgetUtility.h"
#include "kxf/EventSystem/IEvtHandler.h"

namespace kxf
{
	class KXF_API IWidgetItem: public RTTI::ExtendInterface<IWidgetItem, IEvtHandler>, public Private::WidgetDIP<IWidget>, public Private::WidgetDLU<IWidget>
	{
		kxf_RTTI_DeclareIID(IWidgetItem, {0x5f189381, 0xfa1, 0x4fe0, {0x9e, 0x1f, 0xdc, 0xb8, 0x94, 0x33, 0xb6, 0x9}});

		public:
			virtual ~IWidgetItem() = default;

		public:
			virtual std::shared_ptr<IWidget> GetOwningWidget() const = 0;

			virtual String GetLabel(FlagSet<WidgetTextFlag> flags = {}) const = 0;
			virtual void SetLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) = 0;

			virtual String GetDescription() const = 0;
			virtual void SetDescription(const String& description) = 0;

			virtual WidgetID GetID() const = 0;
			virtual void SetID(WidgetID id) = 0;

			virtual BitmapImage GetIcon() const = 0;
			virtual void SetIcon(const BitmapImage& icon) = 0;

			virtual bool IsEnabled() const = 0;
			virtual void SetEnabled(bool enabled = true) = 0;
			void Enable()
			{
				SetEnabled(true);
			}
			void Disable()
			{
				SetEnabled(false);
			}

			virtual bool IsVisible() const = 0;
			virtual void SetVisible(bool visible) = 0;
			void Show()
			{
				SetVisible(true);
			}
			void Hide()
			{
				SetVisible(false);
			}

			virtual Point GetPosition() const = 0;
			virtual void SetPosition(const Point& pos) = 0;

			virtual Rect GetRect(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const = 0;
			virtual Size GetSize(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const = 0;
			virtual void SetSize(const Size& size, FlagSet<WidgetSizeFlag> sizeType = WidgetSizeFlag::Widget) = 0;

			void FromDIP(int& x, int& y) const
			{
				if (auto widget = GetOwningWidget())
				{
					return widget->FromDIP(x, y);
				}
			}
			using WidgetDIP::FromDIP;

			void ToDIP(int& x, int& y) const
			{
				if (auto widget = GetOwningWidget())
				{
					return widget->ToDIP(x, y);
				}
			}
			using WidgetDIP::ToDIP;

			void DialogUnitsToPixels(int& x, int& y) const
			{
				if (auto widget = GetOwningWidget())
				{
					return widget->DialogUnitsToPixels(x, y);
				}
			}
			using WidgetDLU::DialogUnitsToPixels;

			void PixelsToDialogUnits(int& x, int& y) const
			{
				if (auto widget = GetOwningWidget())
				{
					return widget->PixelsToDialogUnits(x, y);
				}
			}
			using WidgetDLU::PixelsToDialogUnits;
	};
}
