#pragma once
#include "Common.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/Core/UniversallyUniqueID.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class KXF_API IStreamOnFileSystem: public RTTI::Interface<IStreamOnFileSystem>
	{
		kxf_RTTI_DeclareIID(IStreamOnFileSystem, {0xf5a807ab, 0xe82a, 0x4215, {0x90, 0xb3, 0xc2, 0x7f, 0xd7, 0x44, 0xfc, 0x2f}});

		public:
			IStreamOnFileSystem() noexcept = default;
			virtual ~IStreamOnFileSystem() = default;

		public:
			virtual FSPath GetFilePath() const = 0;
			virtual UniversallyUniqueID GetFileUniqueID() const = 0;
	};
}
