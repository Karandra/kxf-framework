#pragma once
#include "IEvtHandler.h"

namespace kxf
{
	class KXF_API EvtHandlerStack final
	{
		public:
			enum class Order
			{
				LastToFirst,
				FirstToLast,
			};

		private:
			IEvtHandler* m_Base = nullptr;
			IEvtHandler* m_Top = nullptr;

		public:
			EvtHandlerStack(IEvtHandler& first) noexcept
				:m_Base(&first), m_Top(&first)
			{
			}
			EvtHandlerStack(EvtHandlerStack&& other) noexcept
			{
				*this = std::move(other);
			}
			EvtHandlerStack(const EvtHandlerStack&) = delete;
			~EvtHandlerStack() = default;

		public:
			bool Push(IEvtHandler& evtHandler) noexcept;
			bool Remove(IEvtHandler& evtHandler) noexcept;
			IEvtHandler* Pop() noexcept;

			IEvtHandler* GetBase() const noexcept
			{
				return m_Base;
			}
			IEvtHandler* GetTop() const noexcept
			{
				return m_Top;
			}
			bool HasChainedItems() const noexcept
			{
				return m_Base != m_Top;
			}

			size_t GetCount() const noexcept;
			Enumerator<IEvtHandler&> EnumItems(Order order, bool chainedItemsOnly = false) const noexcept;

		public:
			EvtHandlerStack& operator=(EvtHandlerStack&& other) noexcept
			{
				m_Base = std::exchange(other.m_Base, nullptr);
				m_Top = std::exchange(other.m_Top, nullptr);

				return *this;
			}
			EvtHandlerStack& operator=(const EvtHandlerStack&) = delete;
	};
}
