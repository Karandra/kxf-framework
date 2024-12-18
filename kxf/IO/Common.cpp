#include "kxf-pch.h"
#include "Common.h"
#include <wx/stream.h> 
#include <wx/filefn.h> 

namespace kxf::IO
{
	std::optional<wxSeekMode> ToWxSeekMode(IOStreamSeek seekMode) noexcept
	{
		switch (seekMode)
		{
			case IOStreamSeek::FromStart:
			{
				return wxSeekMode::wxFromStart;
			}
			case IOStreamSeek::FromCurrent:
			{
				return wxSeekMode::wxFromCurrent;
			}
			case IOStreamSeek::FromEnd:
			{
				return wxSeekMode::wxFromEnd;
			}
		};
		return {};
	}
	std::optional<IOStreamSeek> FromWxSeekMode(wxSeekMode seekMode) noexcept
	{
		switch (seekMode)
		{
			case wxSeekMode::wxFromStart:
			{
				return IOStreamSeek::FromStart;
			}
			case wxSeekMode::wxFromCurrent:
			{
				return IOStreamSeek::FromCurrent;
			}
			case wxSeekMode::wxFromEnd:
			{
				return IOStreamSeek::FromEnd;
			}
		};
		return {};
	}
}
