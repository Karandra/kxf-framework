#pragma once
#include "../Common.h"
#include "CoroutineImpl.h"
#include "YieldInstruction.h"

namespace kxf::Async
{
	template<class TCallable>
	class CoroutineCallableWrapper;

	template<class TClass>
	class CoroutineMethodWrapper;
}

namespace kxf
{
	class KXF_API Coroutine: public Async::CoroutineBase
	{
		private:
			static Coroutine* DoRun(std::unique_ptr<Coroutine> coroutine)
			{
				return static_cast<Coroutine*>(CoroutineBase::Run(std::move(coroutine)));
			}

		public:
			static Coroutine* Run(std::unique_ptr<Coroutine> coroutine)
			{
				return DoRun(std::move(coroutine));
			}
			
			template<class TCallable>
			static Coroutine* Run(TCallable&& func)
			{
				return DoRun(std::make_unique<Async::CoroutineCallableWrapper<TCallable>>(std::forward<TCallable>(func)));
			}
			
			template<class TClass>
			static Coroutine* Run(Async::YieldInstruction(TClass::*method)(Coroutine&), TClass* object)
			{
				return DoRun(std::make_unique<Async::CoroutineMethodWrapper<TClass>>(method, object));
			}
	};
}

namespace kxf::Async
{
	template<class TCallable>
	class CoroutineCallableWrapper: public Coroutine
	{
		private:
			TCallable m_Callable;

		protected:
			YieldInstruction Execute() override
			{
				return std::invoke(m_Callable, *this);
			}

		public:
			CoroutineCallableWrapper(TCallable&& func)
				:m_Callable(std::forward<TCallable>(func))
			{
			}
	};

	template<class TClass>
	class CoroutineMethodWrapper: public Coroutine
	{
		public:
			using TMethod = YieldInstruction(TClass::*)(Coroutine&);

		private:
			TClass* m_Object = nullptr;
			TMethod m_Method = nullptr;

		protected:
			YieldInstruction Execute() override
			{
				return std::invoke(m_Method, m_Object, *this);
			}

		public:
			CoroutineMethodWrapper(TMethod method, TClass* object)
				:m_Method(method), m_Object(object)
			{
			}
	};
}
