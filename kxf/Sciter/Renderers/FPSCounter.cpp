#include "kxf-pch.h"
#include "FPSCounter.h"

namespace kxf::Sciter
{
	void FPSCounter::OnStartRender()
	{
	}
	void FPSCounter::OnEndRender()
	{
		const int64_t trackTime = m_TrackTime.GetMilliseconds();
		const int64_t now = ::GetTickCount64();
		const int64_t before = now - trackTime;

		while (!m_FrameTime.empty())
		{
			const int64_t early = m_FrameTime.front();
			if (early >= before)
			{
				break;
			}
			m_FrameTime.pop_front();
		}

		m_FrameTime.push_back(now);
		m_AverageFrameCount = (double)m_FrameTime.size() / ((double)trackTime * 0.001);
	}
}
