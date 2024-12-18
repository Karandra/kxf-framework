#include "kxf-pch.h"
#include "IID.h"
#include "kxf/Core/UniversallyUniqueID.h"

namespace kxf
{
	UniversallyUniqueID IID::ToUniversallyUniqueID() const noexcept
	{
		return m_ID;
	}
}
