#pragma once
#include "ISystemProcessStdIO.h"
#include "kxf/wxWidgets/StreamWrapper.h"

#include <Windows.h>
#include <wx/private/pipestream.h>
#include <wx/private/streamtempinput.h>
#include "kxf/Win32/UndefMacros.h"

namespace kxf
{
	class KXF_API RunningSystemProcessStdIO: public ISystemProcessStdIO
	{
		private:
			mutable std::optional<wxWidgets::OutputStreamWrapper> m_InStreamWrapper;
			mutable std::optional<wxWidgets::InputStreamWrapper> m_OutStreamWrapper;
			mutable std::optional<wxWidgets::InputStreamWrapper> m_ErrorStreamWrapper;

		protected:
			std::unique_ptr<wxPipeOutputStream> m_InStream = nullptr;
			std::unique_ptr<wxPipeInputStream> m_OutStream = nullptr;
			std::unique_ptr<wxPipeInputStream> m_ErrorStream = nullptr;

		protected:
			void OnStreamsInitialized();

		public:
			RunningSystemProcessStdIO() = default;

		public:
			IOutputStream& GetStdIn() override
			{
				return *m_InStreamWrapper;
			}
			IInputStream& GetStdOut() override
			{
				return *m_OutStreamWrapper;
			}
			IInputStream& GetStdErr() override
			{
				return *m_ErrorStreamWrapper;
			}

			String ReadStdOut() const override;
			String ReadStdErr() const override;
	};
}
