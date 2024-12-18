#pragma once
#include "Common.h"
#include "Event.h"
#include "IEvtHandler.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class KXF_API IWithEvtHandler: public RTTI::Interface<IWithEvtHandler>
	{
		kxf_RTTI_DeclareIID(IWithEvtHandler, {0x9672e207, 0xdb80, 0x4cc0, {0x91, 0x31, 0x72, 0x19, 0x26, 0xfa, 0xea, 0x22}});

		public:
			virtual ~IWithEvtHandler() = default;

		public:
			virtual IEvtHandler* GetEvtHandler() const = 0;
			virtual void SetEvtHandler(IEvtHandler& evtHandler) = 0;
			virtual void SetEvtHandler(std::unique_ptr<IEvtHandler> evtHandler) = 0;
	};
}
