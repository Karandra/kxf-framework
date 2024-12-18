#include "kxf-pch.h"
#include "TextDocument.h"
#include "kxf/FileSystem/NativeFileSystem.h"
#include "kxf/IO/StreamReaderWriter.h"
#include "kxf/Utility/ScopeGuard.h"
#include <wx/textfile.h>

namespace
{
	wxTextFileType MapLineBreakType(kxf::LineBreakFormat lineBreakFormat) noexcept
	{
		using namespace kxf;

		switch (lineBreakFormat)
		{
			case LineBreakFormat::Unix:
			{
				return wxTextFileType::wxTextFileType_Unix;
			}
			case LineBreakFormat::Windows:
			{
				return wxTextFileType::wxTextFileType_Dos;
			}
			case LineBreakFormat::Macintosh:
			{
				return wxTextFileType::wxTextFileType_Mac;
			}
		}
		return wxTextFileType::wxTextFileType_None;
	}
}

namespace kxf::TextDocument
{
	size_t Read(const FSPath& filePath, std::function<bool(String)> func)
	{
		wxTextFile textFile(filePath.GetFullPath());
		if (textFile.Open())
		{
			Utility::ScopeGuard atExit([&]()
			{
				textFile.Close();
			});

			size_t count = 0;
			for (size_t i = 0; i < textFile.GetLineCount(); i++)
			{
				count++;
				if (!std::invoke(func, String(textFile.GetLine(i))))
				{
					break;
				}
			}
			return count;
		}
		return 0;
	}
	String Read(const FSPath& filePath, LineBreakFormat lineBreakFormat)
	{
		String result;
		const XChar* lineBreak = wxTextFile::GetEOL(MapLineBreakType(lineBreakFormat));

		Read(filePath, [&](String line)
		{
			if (!result.IsEmpty())
			{
				result += lineBreak;
			}
			result += std::move(line);

			return true;
		});
		return result;
	}

	bool Write(const FSPath& filePath, const String& text, bool append, LineBreakFormat lineBreakFormat)
	{
		const IOStreamDisposition disposition = append ? IOStreamDisposition::OpenExisting : IOStreamDisposition::CreateAlways;
		if (auto stream = NativeFileSystem().OpenToWrite(filePath, disposition))
		{
			bool isSuccess = true;
			const wxTextFileType lineBreakFormatWx = MapLineBreakType(lineBreakFormat);

			IO::OutputStreamWriter writer(*stream);
			if (append)
			{
				writer.SeekToEnd();
				isSuccess = writer.WriteStringUTF8(wxTextFile::GetEOL(lineBreakFormatWx));
			}
			return isSuccess && writer.WriteStringUTF8(wxTextFile::Translate(text, lineBreakFormatWx));
		}
		return false;
	}
	bool Write(const FSPath& filePath, std::function<String()> func, bool append, LineBreakFormat lineBreakFormat)
	{
		const IOStreamDisposition disposition = append ? IOStreamDisposition::OpenExisting : IOStreamDisposition::CreateAlways;
		if (auto stream = NativeFileSystem().OpenToWrite(filePath, disposition))
		{
			const wxTextFileType lineBreakFormatWx = MapLineBreakType(lineBreakFormat);
			const XChar* lineBreak = wxTextFile::GetEOL(lineBreakFormatWx);
			bool isSuccess = true;

			IO::OutputStreamWriter writer(*stream);
			if (append)
			{
				writer.SeekToEnd();
				isSuccess = writer.WriteStringUTF8(lineBreak);
			}

			while (isSuccess)
			{
				String text = std::invoke(func);
				if (!text.IsEmpty())
				{
					isSuccess = writer.WriteStringUTF8(wxTextFile::Translate(text, lineBreakFormatWx));
					if (isSuccess)
					{
						isSuccess = writer.WriteStringUTF8(lineBreak);
					}
				}
				else
				{
					break;
				}
			}
			return isSuccess;
		}
		return false;
	}
}
