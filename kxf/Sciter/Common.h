#pragma once
#include "kxf/Common.hpp"
#include "kxf/UI/Common.h"
#include "kxf/Core/Common.h"
#include "kxf/Core/String.h"
#include "kxf/Core/Version.h"
#include "kxf/Core/DateTime.h"
#include "kxf/Core/ResourceID.h"
#include "kxf/Drawing/Geometry.h"
#include "kxf/Drawing/Color.h"
#include "kxf/Drawing/Angle.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/Network/URI.h"
#include "CommonDefs.h"

namespace kxf::Sciter
{
	KXF_API String GetLibraryName();
	KXF_API Version GetLibraryVersion();

	KXF_API bool IsLibraryLoaded();
	KXF_API void* LoadLibrary(const FSPath& path);
	KXF_API void FreeLibrary();
}

namespace kxf::Sciter
{
	KXF_API String SizeUnitToString(SizeUnit unit);

	KXF_API CornerAlignment MapCornerAlignment(FlagSet<Alignment> alignment);
	KXF_API FlagSet<Alignment> MapCornerAlignment(CornerAlignment alignment);
}
