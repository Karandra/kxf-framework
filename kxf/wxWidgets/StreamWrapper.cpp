#include "kxf-pch.h"
#include "StreamWrapper.h"

namespace
{
	kxf::StreamError TranslateErrorCode(const wxStreamBase& stream)
	{
		using namespace kxf;

		switch (stream.GetLastError())
		{
			case wxStreamError::wxSTREAM_NO_ERROR:
			{
				return StreamError::Success();
			}
			case wxStreamError::wxSTREAM_EOF:
			{
				return StreamErrorCode::EndOfStream;
			}
			case wxStreamError::wxSTREAM_READ_ERROR:
			{
				return StreamErrorCode::ReadError;
			}
			case wxStreamError::wxSTREAM_WRITE_ERROR:
			{
				return StreamErrorCode::WriteError;
			}
		};
		return StreamError::Fail();
	}
}

namespace kxf::wxWidgets
{
	StreamError InputStreamWrapper::GetLastError() const
	{
		return m_LastError ? *m_LastError : TranslateErrorCode(*m_Stream);
	}
}

namespace kxf::wxWidgets
{
	StreamError OutputStreamWrapper::GetLastError() const
	{
		return m_LastError ? *m_LastError : TranslateErrorCode(*m_Stream);
	}
}
