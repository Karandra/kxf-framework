#include "kxf-pch.h"
#include "ZLibStream.h"
#include "kxf/wxWidgets/StreamWrapper.h"
#include <wx/zstream.h>

namespace
{
	constexpr wxZLibFlags MapHeaderType(kxf::ZLibHeader type) noexcept
	{
		using namespace kxf;

		switch (type)
		{
			case ZLibHeader::Auto:
			{
				return wxZLIB_AUTO;
			}
			case ZLibHeader::ZLib:
			{
				return wxZLIB_ZLIB;
			}
			case ZLibHeader::GZip:
			{
				return wxZLIB_GZIP;
			}
		};
		return wxZLIB_NO_HEADER;
	}
}

namespace kxf
{
	ZLibInputStream::ZLibInputStream(IInputStream& stream, ZLibHeader header)
	{
		m_ZStream = std::make_unique<wxZlibInputStream>(new wxWidgets::InputStreamWrapperWx(stream), ToInt(MapHeaderType(header)));
		InputStreamDelegate::m_Stream = std::make_unique<wxWidgets::InputStreamWrapper>(*m_ZStream);
	}
	ZLibInputStream::ZLibInputStream(std::unique_ptr<IInputStream> stream, ZLibHeader header)
	{
		m_ZStream = std::make_unique<wxZlibInputStream>(new wxWidgets::InputStreamWrapperWx(std::move(stream)), ToInt(MapHeaderType(header)));
		InputStreamDelegate::m_Stream = std::make_unique<wxWidgets::InputStreamWrapper>(*m_ZStream);
	}
}

namespace kxf
{
	ZLibOutputStream::ZLibOutputStream(IOutputStream& stream, ZLibHeader header)
	{
		m_ZStream = std::make_unique<wxZlibOutputStream>(new wxWidgets::OutputStreamWrapperWx(stream), ToInt(MapHeaderType(header)));
		OutputStreamDelegate::m_Stream = std::make_unique<wxWidgets::OutputStreamWrapper>(*m_ZStream);
	}
	ZLibOutputStream::ZLibOutputStream(std::unique_ptr<IOutputStream> stream, ZLibHeader header)
	{
		m_ZStream = std::make_unique<wxZlibOutputStream>(new wxWidgets::OutputStreamWrapperWx(std::move(stream)), ToInt(MapHeaderType(header)));
		OutputStreamDelegate::m_Stream = std::make_unique<wxWidgets::OutputStreamWrapper>(*m_ZStream);
	}
}
