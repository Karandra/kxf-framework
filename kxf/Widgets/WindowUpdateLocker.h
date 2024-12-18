#pragma once
#include "Common.h"
#include <wx/window.h>

namespace kxf::UI
{
	class WindowUpdateLocker final
	{
		private:
			wxWindow* m_Window = nullptr;

		public:
			WindowUpdateLocker() = default;
			WindowUpdateLocker(wxWindow& window)
				:m_Window(&window)
			{
				m_Window->Freeze();
			}
			WindowUpdateLocker(const WindowUpdateLocker&) = delete;
			WindowUpdateLocker(WindowUpdateLocker&& other)
			{
				*this = std::move(other);
			}
			~WindowUpdateLocker()
			{
				if (m_Window)
				{
					m_Window->Thaw();
				}
			}

		public:
			WindowUpdateLocker& operator=(const WindowUpdateLocker&) = delete;
			WindowUpdateLocker& operator=(WindowUpdateLocker&& other)
			{
				m_Window = other.m_Window;
				other.m_Window = nullptr;

				return *this;
			}
	};
}
