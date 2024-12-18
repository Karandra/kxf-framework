#include "kxf-pch.h"
#include "ResourceID.h"

namespace kxf
{
	String ResourceID::GetPath() const
	{
		String result = m_Value.GetServer();
		String path = m_Value.GetPath();

		if (!result.EndsWith('/') && !path.StartsWith('/'))
		{
			result += '/';
		}
		result += path;

		return result;
	}
}
