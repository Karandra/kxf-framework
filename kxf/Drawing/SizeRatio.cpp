#include "kxf-pch.h"
#include "SizeRatio.h"
#include "kxf/System/SystemInformation.h"

namespace kxf::Geometry
{
	SizeRatio SizeRatio::FromSystemIcon() noexcept
	{
		return System::GetMetric(SystemSizeMetric::Icon);
	}
	SizeRatio SizeRatio::FromSystemSmallIcon() noexcept
	{
		return System::GetMetric(SystemSizeMetric::IconSmall);
	}
}
