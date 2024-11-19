#pragma once
#include "../Common.h"
#include "CoroutineImpl.h"

namespace kxf
{
	class Coroutine;
}
namespace kxf::Async
{
	class CoroutineBase;

	enum class InstructionType
	{
		Delay,
		Continue,
		Terminate,
	};

	template<class T>
	void AssertStateType()
	{
		static_assert(std::is_integral_v<T> || std::is_enum_v<T>);
	}
}

namespace kxf::Async
{
	class KX_API YieldInstruction final
	{
		friend class CoroutineBase;
		friend class Coroutine;

		private:
			InstructionType m_Type = InstructionType::Continue;
			std::optional<intptr_t> m_NextState;
			TimeSpan m_Delay;

		protected:
			YieldInstruction(InstructionType instruction) noexcept
				:m_Type(instruction)
			{
			}
			
			template<class T>
			YieldInstruction(InstructionType type, const T& nextState) noexcept
				:m_Type(type), m_NextState(static_cast<intptr_t>(nextState))
			{
				Async::AssertStateType<T>();
			}

		public:
			InstructionType GetType() const noexcept
			{
				return m_Type;
			}
			TimeSpan GetDelay() const
			{
				return m_Delay;
			}
			
			template<class T = intptr_t>
			std::optional<T> GetNextState() const noexcept
			{
				Async::AssertStateType<T>();
				if (m_NextState)
				{
					return static_cast<T>(*m_NextState);
				}
				else
				{
					return std::nullopt;
				}
			}
	};
}
