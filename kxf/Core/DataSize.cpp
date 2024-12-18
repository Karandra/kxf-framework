#include "kxf-pch.h"
#include "DataSize.h"
#include "Math.h"
#include "kxf/Application/ICoreApplication.h"

namespace
{
	using namespace kxf;

	std::optional<String> GetSizeUnitString(DataSizeUnit unit)
	{
		if (auto app = ICoreApplication::GetInstance())
		{
			const ILocalizationPackage& localizationPackage = app->GetLocalizationPackage();
			switch (unit)
			{
				case DataSizeUnit::Bytes:
				{
					return localizationPackage.GetItem("DataSizeUnit.Bytes");
				}
				case DataSizeUnit::KiloBytes:
				{
					return localizationPackage.GetItem("DataSizeUnit.KB");
				}
				case DataSizeUnit::MegaBytes:
				{
					return localizationPackage.GetItem("DataSizeUnit.MB");
				}
				case DataSizeUnit::GigaBytes:
				{
					return localizationPackage.GetItem("DataSizeUnit.GB");
				}
				case DataSizeUnit::TeraBytes:
				{
					return localizationPackage.GetItem("DataSizeUnit.TB");
				}
			};
		}
		return {};
	}
	bool AddUnitLabelIfNeeded(String& result, FlagSet<DataSizeFormat> format, DataSizeUnit unit)
	{
		if (format & DataSizeFormat::WithLabel && !result.IsEmpty())
		{
			if (auto label = GetSizeUnitString(unit))
			{
				result += ' ';
				result += *label;

				return true;
			}
		}
		return false;
	}
	
	template<class T>
	String FormatWithUnitBase(T value, FlagSet<DataSizeFormat> format, DataSizeUnit unit, int precision)
	{
		String result;
		if (format & DataSizeFormat::Fractional)
		{
			// Default precision is 2 digits
			result = String::FromFloatingPoint(static_cast<double>(value), precision >= 0 ? precision : 2);
		}
		else
		{
			result = kxf::ToString(value);
		}
		AddUnitLabelIfNeeded(result, format, unit);

		return result;
	}
	
	String FormatWithUnit(DataSize value, FlagSet<DataSizeFormat> format, DataSizeUnit unit, int precision)
	{
		if (format & DataSizeFormat::Fractional)
		{
			return FormatWithUnitBase(value.ToUnit<double>(unit), format, unit, precision);
		}
		else
		{
			return FormatWithUnitBase(value.ToUnit<int64_t>(unit), format, unit, precision);
		}
	}
}

namespace kxf
{
	String DataSize::ToString(DataSizeUnit unit, FlagSet<DataSizeFormat> format, int precision) const
	{
		// Short-circuit for zero
		if (IsNull())
		{
			if (auto label = GetSizeUnitString(DataSizeUnit::Bytes))
			{
				return Format("0 {}", *label);
			}
			return "0";
		}

		switch (unit)
		{
			case DataSizeUnit::Auto:
			{
				// https://programming.guide/worlds-most-copied-so-snippet.html
				// 0 for bytes, 1 for KB, 2 for MB and so on. Matches the values in 'DataSizeUnit'.
				const int digitGroups = Math::Log(m_Value, 1024);
				const double value = m_Value / std::pow(1024, digitGroups);

				return FormatWithUnitBase(value, format, static_cast<DataSizeUnit>(digitGroups), precision);
			}
			case DataSizeUnit::Bytes:
			case DataSizeUnit::KiloBytes:
			case DataSizeUnit::MegaBytes:
			case DataSizeUnit::GigaBytes:
			case DataSizeUnit::TeraBytes:
			{
				return FormatWithUnit(*this, format, unit, precision);
			}
		};
		return {};
	}
}
