#pragma once
#include "kxf/Common.hpp"
#include "kxf/Core/StdID.h"
#include "kxf/Core/String.h"
#include "kxf/Core/ResourceID.h"
#include "kxf/Core/CallbackFunction.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/FileSystem/FileItem.h"
#include "kxf/FileSystem/IFileSystem.h"
#include "Constants.h"

namespace kxf
{
	class Locale;
	class WidgetID;
	class FSPath;
	class FileItem;
	class IFileSystem;
	class DynamicLibrary;
}

namespace kxf::Localization
{
	KXF_API String GetStandardString(StdID id);
	KXF_API String GetStandardString(const WidgetID& id);

	KXF_API CallbackResult<void> SearchPackages(const IFileSystem& fileSystem, const FSPath& directory, CallbackFunction<Locale, FileItem, String> func);
	KXF_API CallbackResult<void> SearchPackages(const DynamicLibrary& library, CallbackFunction<Locale, FileItem, String> func);
}
