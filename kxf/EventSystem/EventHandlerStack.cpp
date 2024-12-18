#include "kxf-pch.h"
#include "EventHandlerStack.h"
#include "kxf/Core/Enumerator.h"

namespace kxf
{
	bool EvtHandlerStack::Push(IEvtHandler& evtHandler) noexcept
	{
		// New handler can't be part of another chain
		if (evtHandler.IsUnlinked())
		{
			evtHandler.SetNextHandler(m_Top);
			m_Top->SetPrevHandler(&evtHandler);
			m_Top = &evtHandler;

			return true;
		}
		return false;
	}
	bool EvtHandlerStack::Remove(IEvtHandler& evtHandler) noexcept
	{
		// Short circuit for the last handler
		if (&evtHandler == m_Top)
		{
			return Pop() != nullptr;
		}

		// Check if this handler is part of any chain
		if (!evtHandler.IsUnlinked())
		{
			// Is it part of our chain?
			for (IEvtHandler& chainItem: EnumItems(Order::LastToFirst))
			{
				// Unlink it
				if (&chainItem == &evtHandler)
				{
					chainItem.Unlink();
					return true;
				}
			};
		}
		return false;
	}
	IEvtHandler* EvtHandlerStack::Pop() noexcept
	{
		// We need to pop the stack, i.e. we need to remove the latest added handler
		IEvtHandler* topHandler = m_Top;

		// We can't pop if we have only one item and the top handler should have no previous handlers set
		if (topHandler != m_Base && !topHandler->GetPrevHandler())
		{
			// The second handler should have non-null next handler
			if (IEvtHandler* nextHandler = topHandler->GetNextHandler())
			{
				topHandler->SetNextHandler(nullptr);
				nextHandler->SetPrevHandler(nullptr);

				// Now top handler is completely unlinked, set next handler as the new current handler
				m_Top = nextHandler;

				// And return the popped one
				return topHandler;
			}
		}
		return nullptr;
	}

	size_t EvtHandlerStack::GetCount() const noexcept
	{
		return EnumItems(Order::LastToFirst).CalcTotalCount();
	}
	Enumerator<IEvtHandler&> EvtHandlerStack::EnumItems(Order order, bool chainedItemsOnly) const noexcept
	{
		return [item = order == Order::FirstToLast ? m_Base : m_Top, this, order, chainedItemsOnly]() mutable -> optional_ref<IEvtHandler>
		{
			if (item && (!chainedItemsOnly || item != m_Base))
			{
				auto result = item;
				if (order == Order::FirstToLast)
				{
					item = item->GetPrevHandler();
				}
				else
				{
					item = item->GetNextHandler();
				}

				return *result;
			}
			return {};
		};
	}
}
