#pragma once
#include "Common.h"

namespace kxf
{
	class IEvent;
	class IEvtHandler;
}

namespace kxf
{
	class KXF_API IEventExecutor
	{
		public:
			virtual ~IEventExecutor() = default;

		public:
			virtual void Execute(IEvtHandler& evtHandler, IEvent& event) = 0;
			virtual bool IsSameAs(const IEventExecutor& other) const noexcept = 0;
			virtual IEvtHandler* GetTargetHandler() noexcept = 0;
	};
}

namespace kxf::EventSystem
{
	class NullEventExecutor final: public IEventExecutor
	{
		public:
			static NullEventExecutor& Get() noexcept
			{
				static NullEventExecutor executor;
				return executor;
			}

		private:
			NullEventExecutor() noexcept = default;

		public:
			NullEventExecutor(const NullEventExecutor&) = delete;

		public:
			void Execute(IEvtHandler& evtHandler, IEvent& event) noexcept override
			{
			}
			bool IsSameAs(const IEventExecutor& other) const noexcept override
			{
				return this == &Get();
			}
			IEvtHandler* GetTargetHandler() noexcept override
			{
				return nullptr;
			}

		public:
			NullEventExecutor& operator=(const NullEventExecutor&) = delete;
	};
}
