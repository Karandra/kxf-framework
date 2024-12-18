#pragma once
#include "../Common.h"
#include "../Element.h"
#include "kxf/EventSystem/Event.h"

namespace kxf
{
	class KXF_API SciterEvent: public BasicEvent
	{
		public:
			kxf_EVENT_MEMBER(SciterEvent, Attached);
			kxf_EVENT_MEMBER(SciterEvent, Detached);

			kxf_EVENT_MEMBER(SciterEvent, EngineCreated);
			kxf_EVENT_MEMBER(SciterEvent, EngineDestroyed);

			kxf_EVENT_MEMBER(SciterEvent, Gesture);
			kxf_EVENT_MEMBER(SciterEvent, Exchange);

			kxf_EVENT_MEMBER(SciterEvent, ContentLoaded);
			kxf_EVENT_MEMBER(SciterEvent, TIScriptCall);
			kxf_EVENT_MEMBER(SciterEvent, ScriptingCall);

		protected:
			Sciter::Host* m_Host = nullptr;
			Sciter::ElementUID* m_Element = nullptr;
			Sciter::ElementUID* m_TargetElement = nullptr;

		protected:
			Sciter::Element RetrieveElement(Sciter::ElementUID* uid) const;
			void AcceptElement(Sciter::ElementUID*& uid, const Sciter::Element& element);

		public:
			SciterEvent(Sciter::Host& host)
				:m_Host(&host)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<SciterEvent>(std::move(*this));
			}
			FlagSet<EventCategory> GetEventCategory() const noexcept override
			{
				return EventCategory::UI;
			}

			//SciterEvent
			Sciter::Host& GetHost() const
			{
				return *m_Host;
			}

			Sciter::Element GetElement() const
			{
				return RetrieveElement(m_Element);
			}
			void SetElement(const Sciter::Element& element)
			{
				AcceptElement(m_Element, element);
			}
	
			Sciter::Element GetTargetElement() const
			{
				return RetrieveElement(m_TargetElement);
			}
			void SetTargetElement(const Sciter::Element& element)
			{
				AcceptElement(m_TargetElement, element);
			}
	};
}

namespace kxf::Sciter
{
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, Attached);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, Detached);

	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, EngineCreated);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, EngineDestroyed);

	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, Gesture);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, Exchange);

	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, ContentLoaded);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, TIScriptCall);
	kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, ScriptingCall);
}
