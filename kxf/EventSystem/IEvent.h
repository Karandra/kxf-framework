#pragma once
#include "Common.h"
#include "EventID.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/Core/UniversallyUniqueID.h"
#include "kxf/Core/DateTime/TimeSpan.h"
#include "kxf/UI/WidgetID.h"

namespace kxf
{
	class IEvtHandler;
}
namespace kxf::EventSystem
{
	class EventAccessor;
}

namespace kxf
{
	class KXF_API IEvent: public RTTI::Interface<IEvent>
	{
		kxf_RTTI_DeclareIID(IEvent, {0x61df394f, 0x8d5c, 0x43ef, {0xbd, 0x9f, 0xfb, 0xeb, 0xbf, 0x1e, 0x97, 0xa5}});

		friend class EventSystem::EventAccessor;

		public:
			static inline const EventTag<IEvent> EvtNull = 0;
			static inline const EventTag<IEvent> EvtAny = -1;

		public:
			virtual ~IEvent() = default;

		public:
			virtual std::unique_ptr<IEvent> Move() noexcept = 0;

			virtual EventID GetEventID() const = 0;
			virtual TimeSpan GetTimestamp() const = 0;
			virtual UniversallyUniqueID GetUniqueID() const = 0;
			virtual FlagSet<EventCategory> GetEventCategory() const = 0;

			virtual std::shared_ptr<IEvtHandler> GetEventSource() const = 0;
			virtual void SetEventSource(std::shared_ptr<IEvtHandler> evtHandler) = 0;

			virtual bool IsSkipped() const = 0;
			virtual void Skip(bool skip = true) = 0;

			virtual bool IsAllowed() const = 0;
			virtual void Allow(bool allow = true) = 0;
	};

	class KXF_API IEventInternal: public RTTI::Interface<IEventInternal>
	{
		kxf_RTTI_DeclareIID(IEventInternal, {0x117da075, 0xb7e4, 0x45bb, {0xba, 0x33, 0x51, 0xa9, 0x57, 0x4c, 0xcc, 0x6f}});

		public:
			virtual ~IEventInternal() = default;

		public:
			virtual bool IsAsync() const = 0;
			virtual bool WasReQueued() const = 0;
			virtual bool WasProcessed() const = 0;
			virtual bool WillBeProcessedAgain() const = 0;

			virtual bool OnStartProcess(const EventID& eventID, const UniversallyUniqueID& uuid, FlagSet<ProcessEventFlag> flags, bool isAsync) = 0;
			virtual FlagSet<ProcessEventFlag> GetProcessFlags() const = 0;

			virtual std::unique_ptr<IEvent> WaitProcessed() = 0;
			virtual void SignalProcessed(std::unique_ptr<IEvent> event) = 0;

			virtual void PutWaitResult(std::unique_ptr<IEvent> event) = 0;
			virtual std::unique_ptr<IEvent> GetWaitResult() = 0;
	};
}

namespace kxf
{
	class KXF_API IIndirectInvocationEvent: public RTTI::Interface<IIndirectInvocationEvent>
	{
		kxf_RTTI_DeclareIID(IIndirectInvocationEvent, {0xb0046b41, 0xecd9, 0x4b46, {0xb1, 0xb5, 0xd6, 0x7d, 0xa, 0x77, 0x5c, 0xbe}});

		public:
			static inline const EventTag<IEvent> EvtIndirectInvocation = -2;

		public:
			virtual ~IIndirectInvocationEvent() = default;

		public:
			virtual void Execute() = 0;
	};

	class KXF_API ISignalInvocationEvent: public RTTI::Interface<ISignalInvocationEvent>
	{
		kxf_RTTI_DeclareIID(ISignalInvocationEvent, {0xb3ad1ef2, 0x96ee, 0x4ebf, {0x93, 0xac, 0x6, 0xf8, 0x5b, 0x2e, 0xe5, 0xc5}});

		public:
			virtual ~ISignalInvocationEvent() = default;

		public:
			virtual bool ExpectsResult() const = 0;
			virtual bool ContainsResult() const = 0;

			virtual bool GetParameters(void* parameters) = 0;
			virtual void TakeResult(void* value) = 0;
			virtual void PutResult(void* value) = 0;
	};

	template<class TFunc>
	requires(std::is_member_function_pointer_v<TFunc>)
	EventTag(TFunc func) -> EventTag<ISignalInvocationEvent>;
}

// Event declaration macros

// For global event IDs
#define kxf_EVENT_DECLARE_GLOBAL(type, name)						extern const KXF_API kxf::EventTag<type> KxEVT_##name
#define kxf_EVENT_DEFINE_GLOBAL(type, name)							const kxf::EventTag<type> KxEVT_##name = kxf::EventSystem::NewSimpleEventID()
#define kxf_EVENT_DEFINE_GLOBAL_AS(type, name, other)				const kxf::EventTag<type> KxEVT_##name = static_cast<kxf::EventID>(other)

// For local scope (namespace, function, etc)
#define kxf_EVENT_DECLARE_LOCAL(type, name)							extern KXF_API const kxf::EventTag<type> Evt##name
#define kxf_EVENT_DEFINE_LOCAL(type, name)							const kxf::EventTag<type> Evt##name = kxf::EventSystem::NewSimpleEventID()
#define kxf_EVENT_DEFINE_LOCAL_AS(type, name, other)				const kxf::EventTag<type> Evt##name = static_cast<kxf::EventID>(other)

// As static member of an event class
#define kxf_EVENT_DECLARE_MEMBER(type, name)						static const kxf::EventTag<type> Evt##name
#define kxf_EVENT_DEFINE_MEMBER(type, className, name)				const kxf::EventTag<type> className::Evt##name = kxf::EventSystem::NewSimpleEventID()
#define kxf_EVENT_DEFINE_MEMBER_AS(type, className, name, other)	const kxf::EventTag<type> className::Evt##name = static_cast<kxf::EventID>(other)
#define kxf_EVENT_DECLARE_ALIAS_TO_MEMBER(type, name)				inline const kxf::EventTag<type> Evt##name = type::Evt##name
#define kxf_EVENT_DECLARE_ALIAS_TO_MEMBER_AS(type, name, other)		inline const kxf::EventTag<type> Evt##name = type::Evt##other

// As inline static member of an event class
#define kxf_EVENT_MEMBER(type, name)								inline static const kxf::EventTag<type> Evt##name = kxf::EventSystem::NewSimpleEventID()
#define kxf_EVENT_MEMBER_AS(type, name, other)						inline static const kxf::EventTag<type> Evt##name = static_cast<kxf::EventID>(other)
#define kxf_EVENT_MEMBER_STRING(type, name)							inline static const kxf::EventTag<type> Evt##name = "kxf::" #type "::" #name
#define kxf_EVENT_MEMBER_UNIQUE_STRING(type, name, uniqueID)		inline static const kxf::EventTag<type> Evt##name = "kxf::" #type "::" #name "-" uniqueID
