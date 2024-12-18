#pragma once
#include "Common.h"
#include "kxf/EventSystem/IEvent.h"
#include "kxf/UI/WidgetID.h"

namespace kxf
{
	class IWidgetEvent;
}

namespace kxf
{
	class KXF_API IWidgetEvent: public RTTI::Interface<IWidgetEvent>
	{
		kxf_RTTI_DeclareIID(IWidgetEvent, {0x4552fa23, 0xb7da, 0x44c5, {0x86, 0x93, 0x30, 0x40, 0x87, 0x31, 0x33, 0x72}});

		public:
			struct PropagationLevel final
			{
				// Don't propagate it at all
				static constexpr size_t None = 0;

				// Propagate it until it is processed
				static constexpr size_t Max = std::numeric_limits<uint32_t>::max();

				PropagationLevel() noexcept = delete;
			};

		public:
			virtual ~IWidgetEvent() = default;

		public:
			virtual std::shared_ptr<IWidget> GetWidget() const = 0;
			virtual void SetWidget(std::shared_ptr<IWidget> widget) = 0;

			virtual WidgetID GetWidgetID() const = 0;
			virtual void SetWidgetID(WidgetID id) = 0;

			virtual size_t StopPropagation() = 0;
			virtual void ResumePropagation(size_t level) = 0;
			virtual bool ShouldPropagate() const = 0;
	};
}
