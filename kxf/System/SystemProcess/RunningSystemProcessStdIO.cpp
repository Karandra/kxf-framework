#include "kxf-pch.h"
#include "RunningSystemProcessStdIO.h"
#include "kxf/IO/IStream.h"
#include "kxf/Utility/ScopeGuard.h"
#include <wx/string.h>

namespace
{
	kxf::String ReadToString(kxf::IInputStream& stream)
	{
		using namespace kxf;

		String result;
		char buffer[std::numeric_limits<int16_t>::max()] = {};

		while (stream.CanRead())
		{
			if (stream.Read(buffer, std::size(buffer)).LastRead() != 0)
			{
				result += wxString(buffer, wxConvWhateverWorks, stream.LastRead().ToBytes());
			}
			else
			{
				break;
			}
		}
		return result;
	}
}

namespace kxf
{
	void RunningSystemProcessStdIO::OnStreamsInitialized()
	{
		if (m_InStream)
		{
			m_InStreamWrapper = *m_InStream;
		}
		if (m_OutStream)
		{
			m_OutStreamWrapper = *m_OutStream;
		}
		if (m_ErrorStream)
		{
			m_ErrorStreamWrapper = *m_ErrorStream;
		}
	}

	String RunningSystemProcessStdIO::ReadStdOut() const
	{
		if (m_OutStreamWrapper)
		{
			Utility::ScopeGuard atExit = [&, oldOffset = m_OutStreamWrapper->TellI()]()
			{
				m_OutStreamWrapper->SeekI(oldOffset, IOStreamSeek::FromStart);
			};
			return ReadToString(*m_OutStreamWrapper);
		}
		return {};
	}
	String RunningSystemProcessStdIO::ReadStdErr() const
	{
		if (m_ErrorStreamWrapper)
		{
			Utility::ScopeGuard atExit = [&, oldOffset = m_ErrorStreamWrapper->TellI()]()
			{
				m_ErrorStreamWrapper->SeekI(oldOffset, IOStreamSeek::FromStart);
			};
			return ReadToString(*m_ErrorStreamWrapper);
		}
		return {};
	}
}
