#pragma once
#include "kxf/Common.hpp"
#include "kxf/Core/String.h"
#include "kxf/Core/Version.h"

namespace kxf::Crypto
{
	KXF_API String GetLibraryName();
	KXF_API Version GetLibraryVersion();
}
