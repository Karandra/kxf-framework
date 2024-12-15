#pragma once
#include "kxf/Common.hpp"
#include "kxf/Core/String.h"
#include "kxf/Core/ResourceID.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/FileSystem/FileItem.h"
#include "kxf/FileSystem/IFileSystem.h"
#include "Constants.h"

namespace kxf
{
	class Locale;
	class WidgetID;
	class DynamicLibrary;
}

namespace kxf::Localization
{
	KX_API String GetStandardString(StdID id);
	KX_API String GetStandardString(WidgetID id);

	KX_API size_t SearchPackages(const IFileSystem& fileSystem, const FSPath& directory, std::function<CallbackCommand(Locale, FileItem)> func);
	KX_API size_t SearchPackages(const DynamicLibrary& library, std::function<CallbackCommand(Locale, FileItem)> func);
}
