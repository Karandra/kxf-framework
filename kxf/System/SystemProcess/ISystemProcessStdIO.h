#pragma once
#include "../Common.h"
#include "kxf/Core/String.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class IInputStream;
	class IOutputStream;
}

namespace kxf
{
	class KXF_API ISystemProcessStdIO: public RTTI::Interface<ISystemProcessStdIO>
	{
		kxf_RTTI_DeclareIID(ISystemProcessStdIO, {0xbeb81b73, 0x3c92, 0x4e7b, {0x8a, 0x85, 0x7, 0x69, 0x4, 0x24, 0xe3, 0xa8}});

		public:
			virtual ~ISystemProcessStdIO() = default;

		public:
			virtual IOutputStream& GetStdIn() = 0;
			virtual IInputStream& GetStdOut() = 0;
			virtual IInputStream& GetStdErr() = 0;

			virtual String ReadStdOut() const = 0;
			virtual String ReadStdErr() const = 0;
	};
}
