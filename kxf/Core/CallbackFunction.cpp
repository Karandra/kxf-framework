#include "KxfPCH.h"
#include "CallbackFunction.h"

namespace kxf
{
	size_t CallbackFunctionState::AsIndex(CallbackCommand command) const noexcept
	{
		auto index = ToInt(command);
		if (index >= 0 && static_cast<size_t>(index) < m_Counters.size())
		{
			return static_cast<size_t>(index);
		}
		return std::numeric_limits<size_t>::max();
	}
	void CallbackFunctionState::UpdateWith(CallbackCommand command) noexcept
	{
		if (auto index = AsIndex(command); index < m_Counters.size())
		{
			m_Counters[index]++;
			m_LastCommand = command;
		}
		else
		{
			m_LastCommand = CallbackCommand::Terminate;
		}
	}

	size_t CallbackFunctionState::GetCount() const noexcept
	{
		return GetCount(CallbackCommand::Continue) + GetCount(CallbackCommand::Discard);
	}
	size_t CallbackFunctionState::GetCount(CallbackCommand command) const noexcept
	{
		if (auto index = AsIndex(command); index < m_Counters.size())
		{
			return m_Counters[index];
		}
		return 0;
	}
	size_t CallbackFunctionState::GetTotalCount() const noexcept
	{
		return std::accumulate(m_Counters.begin(), m_Counters.end(), 0_uz);
	}
}
