#include "kxf-pch.h"
#include "StreamReaderWriter.h"
#include <wx/string.h>
#include <wx/ustring.h>

namespace kxf
{
	bool IO::InputStreamReader::ReadStringASCII(String& value, size_t size)
	{
		std::vector<char> buffer;
		if (ReadVector(buffer, size))
		{
			DoRemoveTrailingNulls(buffer);
			value = String::FromASCII({buffer.data(), buffer.size()});
			return true;
		}
		else
		{
			value.clear();
			return false;
		}
	}
	bool IO::InputStreamReader::ReadStringUTF8(String& value, size_t size)
	{
		std::vector<char> buffer;
		if (ReadVector(buffer, size))
		{
			DoRemoveTrailingNulls(buffer);
			value = String::FromUTF8({buffer.data(), buffer.size()});
			return true;
		}
		else
		{
			value.clear();
			return false;
		}
	}
	bool IO::InputStreamReader::ReadStringUTF16(String& value, size_t size)
	{
		std::vector<wchar_t> buffer;
		if (ReadVector(buffer, size))
		{
			DoRemoveTrailingNulls(buffer);
			value = String(buffer.data(), buffer.size());
			return true;
		}
		else
		{
			value.clear();
			return false;
		}
	}
	bool IO::InputStreamReader::ReadStringUTF32(String& value, size_t size)
	{
		std::vector<wxChar32> buffer;
		if (ReadVector(buffer, size))
		{
			DoRemoveTrailingNulls(buffer);
			value = wxString(wxUString(wxScopedU32CharBuffer::CreateNonOwned(buffer.data(), buffer.size())));
			return true;
		}
		else
		{
			value.clear();
			return false;
		}
	}
}

namespace kxf
{
	bool IO::OutputStreamWriter::WriteStringASCII(const String& value, char replaceWith)
	{
		const auto buffer = value.ToASCII(replaceWith);
		return WriteBuffer(buffer.data(), buffer.length());
	}
	bool IO::OutputStreamWriter::WriteStringUTF8(const String& value)
	{
		const auto buffer = value.ToUTF8();
		return WriteBuffer(buffer.data(), buffer.length());
	}
	bool IO::OutputStreamWriter::WriteStringUTF16(const String& value)
	{
		return WriteBuffer(value.wc_str(), value.length());
	}
	bool IO::OutputStreamWriter::WriteStringUTF32(const String& value)
	{
		wxUString buffer;
		buffer.assignFromUTF16(value.wc_str(), value.length());

		return WriteBuffer(buffer.data(), buffer.length() * sizeof(wxChar32));
	}
}
