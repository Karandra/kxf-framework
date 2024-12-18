#include "kxf-pch.h"
#include "ShellFileTypeInfo.h"
#include "ShellFileTypeManager.h"

namespace kxf
{
	bool ShellFileTypeInfo::IsURLProtocol(const FSPath& ext) const
	{
		auto it = m_URLProtocolMap.find(ext.GetExtension());
		if (it != m_URLProtocolMap.end())
		{
			return it->second;
		}
		return false;
	}
	ShellFileTypeInfo& ShellFileTypeInfo::SetURLProtocol(const FSPath& ext, bool protocol)
	{
		m_URLProtocolMap.insert_or_assign(ext.GetExtension(), protocol);
		return *this;
	}
}
