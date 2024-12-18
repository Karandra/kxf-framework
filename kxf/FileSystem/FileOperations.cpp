#include "kxf-pch.h"
#include "FileOperations.h"
#include "kxf/Core/RegEx.h"
#include "kxf/Utility/String.h"
#include <wx/filename.h>
#include <shlwapi.h>
#include "kxf/System/UndefWindows.h"

namespace
{
	template<class TFunc>
	kxf::FSPath CallWinAPIWithLengthPrecalc(const kxf::FSPath& filePath, TFunc&& func)
	{
		using namespace kxf;

		String path = filePath.GetFullPathTryNS(FSPathNamespace::Win32File);
		const DWORD length = std::invoke(func, path.wc_str(), nullptr, 0);
		if (length != 0)
		{
			String result;
			std::invoke(func, path.wc_str(), Utility::StringBuffer(result, length), length);

			FSPath fsPath = std::move(result);
			fsPath.EnsureNamespaceSet(filePath.GetNamespace());
			return fsPath;
		}
		return {};
	}
}

namespace kxf::FileSystem
{
	FSPath CreateTempPathName(const FSPath& rootDirectory)
	{
		FSPath fsPath = String(wxFileName::CreateTempFileName(rootDirectory.GetFullPath()));
		fsPath.EnsureNamespaceSet(rootDirectory.GetNamespace());
		return fsPath;
	}
	FSPath PathFrom83Name(const FSPath& path)
	{
		return CallWinAPIWithLengthPrecalc(path, ::GetLongPathNameW);
	}
	FSPath PathTo83Name(const FSPath& path)
	{
		return CallWinAPIWithLengthPrecalc(path, ::GetShortPathNameW);
	}
	FSPath GetAbsolutePath(const FSPath& relativePath)
	{
		if (relativePath.IsRelative())
		{
			String pathString = relativePath.GetFullPathTryNS(FSPathNamespace::Win32File);

			const DWORD length = ::GetFullPathNameW(pathString.wc_str(), 0, nullptr, nullptr);
			if (length != 0)
			{
				String result;
				LPWSTR oldPathStart = nullptr;
				::GetFullPathNameW(pathString.wc_str(), length, Utility::StringBuffer(result, length), &oldPathStart);

				FSPath fsPath = std::move(result);
				fsPath.EnsureNamespaceSet(relativePath.GetNamespace());
				return fsPath;
			}
			return {};
		}
		return relativePath;
	}
	FSPath AbbreviatePath(const FSPath& path, int maxCharacters)
	{
		if (maxCharacters > 0)
		{
			const size_t sourceLength = path.GetLength();
			if (static_cast<size_t>(maxCharacters) < sourceLength && sourceLength < MAX_PATH)
			{
				maxCharacters++;

				String source = path.GetFullPath();
				String result;
				::PathCompactPathExW(Utility::StringBuffer(result, maxCharacters), source.wc_str(), maxCharacters, 0);

				return FSPath(std::move(result)).SetNamespace(path.GetNamespace());
			}
			else if (sourceLength >= MAX_PATH)
			{
				// PathCompactPathEx doesn't work for paths longer than 'MAX_PATH'
				// Taking Jeff Atwood's idea but with a bit different RegEx.
				// Doesn't guarantees final length to not exceed 'maxCharacters'.
				// https://blog.codinghorror.com/shortening-long-file-paths/

				String source = path.GetFullPath();
				RegEx regEx(kxS(R"((\w+:\\|)([^\\]+[^\\]+).*\\([^\\]+))"));
				if (regEx.Matches(source))
				{
					regEx.ReplaceAll(source, kxS(R"(\1\2\\...\\\3)"));
				}

				// If it's still longer, just truncate it and add ellipsis
				if (source.length() > static_cast<size_t>(maxCharacters))
				{
					source.RemoveRight(source.length() - maxCharacters - 3);
					source += "...";
				}
				return FSPath(std::move(source)).SetNamespace(path.GetNamespace());
			}
		}
		return {};
	}
}
