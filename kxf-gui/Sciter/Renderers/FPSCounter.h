#pragma once
#include "kxf/Core/DateTime.h"
#include <deque>

namespace kxf::Sciter
{
	class FPSCounter final
	{
		public:
			class Watcher final
			{
				private:
					FPSCounter& m_Counter;

				public:
					Watcher(FPSCounter& counter)
						:m_Counter(counter)
					{
						m_Counter.OnStartRender();
					}
					~Watcher()
					{
						m_Counter.OnEndRender();
					}
			};

		private:
			std::deque<int64_t> m_FrameTime;
			double m_AverageFrameCount = 0.0;
			TimeSpan m_TrackTime;

		public:
			FPSCounter(const TimeSpan& trackTime = TimeSpan::Milliseconds(1000))
				:m_TrackTime(trackTime)
			{
			}

		public:
			void OnStartRender();
			void OnEndRender();

			Watcher CreateWatcher()
			{
				return *this;
			}
			double GetCount() const
			{
				return m_AverageFrameCount;
			}
	};
}
