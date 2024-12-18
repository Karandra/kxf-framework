#pragma once
#include "Common.h"
#include "FSPath.h"
#include "kxf/EventSystem/Event.h"
#include "kxf/Core/String.h"
#include "kxf/Core/DataSize.h"

namespace kxf
{
	class KXF_API FSActionEvent: public BasicEvent
	{
		public:
			kxf_EVENT_MEMBER(FSActionEvent, Copy);
			kxf_EVENT_MEMBER(FSActionEvent, Move);
			kxf_EVENT_MEMBER(FSActionEvent, Rename);
			kxf_EVENT_MEMBER(FSActionEvent, Remove);
			kxf_EVENT_MEMBER(FSActionEvent, Search);

		private:
			String m_String;
			FSPath m_Source;
			FSPath m_Destination;
			DataSize m_Completed = 0;
			DataSize m_Total = 0;
			DataSize m_Speed = 0;

		public:
			FSActionEvent() = default;

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<FSActionEvent>(std::move(*this));
			}

			// Paths and status
			String GetString() const
			{
				return m_String;
			}
			void SetString(String value)
			{
				m_String = std::move(value);
			}

			FSPath GetSource() const
			{
				return m_Source;
			}
			void SetSource(FSPath source)
			{
				m_Source = std::move(source);
			}
			
			FSPath GetDestination() const
			{
				return m_Destination;
			}
			void SetDestination(FSPath destination)
			{
				m_Destination = std::move(destination);
			}

			// Size status
			DataSize GetTotal() const
			{
				return m_Total;
			}
			DataSize GetCompleted() const
			{
				return m_Completed;
			}
			
			void SetTotal(DataSize value)
			{
				m_Total = value;
			}
			void SetCompleted(DataSize value)
			{
				m_Completed = value;
			}
			void SetProgress(DataSize completed, DataSize total)
			{
				m_Completed = completed;
				m_Total = total;
			}

			bool IsCompleted() const
			{
				return GetCompleted() >= GetTotal();
			}
			void MakeCompleted()
			{
				m_Completed = m_Total;
			}
			bool IsProgressKnown() const
			{
				return m_Completed.IsValid() && m_Total.IsValid();
			}
			double GetProgressRatio() const noexcept
			{
				return GetSizeRatio(m_Completed, m_Total);
			}

			// Speed
			bool IsSpeedKnown() const
			{
				return m_Speed.IsValid();
			}
			DataSize GetSpeed() const noexcept
			{
				return m_Speed;
			}
			void SetSpeed(DataSize value) noexcept
			{
				m_Speed = value;
			}
	};
}
