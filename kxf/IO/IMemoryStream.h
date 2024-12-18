#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class MemoryStreamBuffer;
}

namespace kxf
{
	class KXF_API IMemoryStream: public RTTI::Interface<IMemoryStream>
	{
		kxf_RTTI_DeclareIID(IMemoryStream, {0x3e20ad70, 0x8357, 0x4470, {0xac, 0x30, 0xe5, 0xe9, 0xd4, 0x7d, 0xe8, 0x14}});

		public:
			virtual MemoryStreamBuffer DetachStreamBuffer() = 0;
			virtual void AttachStreamBuffer(MemoryStreamBuffer streamBuffer) = 0;

			virtual MemoryStreamBuffer& GetStreamBuffer() = 0;
			virtual const MemoryStreamBuffer& GetStreamBuffer() const = 0;

			virtual size_t CopyToBuffer(void* buffer, size_t size) const = 0;
	};
}
