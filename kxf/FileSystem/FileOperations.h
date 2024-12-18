#pragma once
#include "Common.h"
#include "FSPath.h"

namespace kxf::FileSystem
{
	KXF_API FSPath CreateTempPathName(const FSPath& rootDirectory);
	KXF_API FSPath PathFrom83Name(const FSPath& path);
	KXF_API FSPath PathTo83Name(const FSPath& path);
	KXF_API FSPath GetAbsolutePath(const FSPath& relativePath);
	KXF_API FSPath AbbreviatePath(const FSPath& path, int maxCharacters);
}
