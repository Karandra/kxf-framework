#pragma once
#include "Common.h"
#include "kxf/IO/StreamDelegate.h"
class wxZlibInputStream;
class wxZlibOutputStream;

namespace kxf
{
	enum class ZLibHeader
	{
		None = -1,

		Auto,
		ZLib,
		GZip,
	};
}

namespace kxf
{
	class KXF_API ZLibInputStream final: public InputStreamDelegate
	{
		private:
			std::unique_ptr<wxZlibInputStream> m_ZStream;

		public:
			ZLibInputStream(IInputStream& stream, ZLibHeader header = ZLibHeader::Auto);
			ZLibInputStream(std::unique_ptr<IInputStream> stream, ZLibHeader header = ZLibHeader::Auto);
	};
}

namespace kxf
{
	class KXF_API ZLibOutputStream final: public OutputStreamDelegate
	{
		private:
			std::unique_ptr<wxZlibOutputStream> m_ZStream;

		public:
			ZLibOutputStream(IOutputStream& stream, ZLibHeader header = ZLibHeader::Auto);
			ZLibOutputStream(std::unique_ptr<IOutputStream> stream, ZLibHeader header = ZLibHeader::Auto);
	};
}
