#include "kxf-pch.h"
#include "Math.h"
#include <locale>
#include <random>
#include <cmath>
#include <iostream>

namespace
{
	const double g_PI = std::acos(-1.0);
	const double g_PI_Half = std::acos(0.0);
}

namespace kxf::Math
{
	UniChar GetDecimalSeparator()
	{
		return std::use_facet<std::numpunct<XChar>>(std::cout.getloc()).decimal_point();
	}
	UniChar GetDecimalCSeparator()
	{
		return '.';
	}

	double GetPI()
	{
		return g_PI;
	}
	double GetHalfPI()
	{
		return g_PI_Half;
	}

	double Round(double x, int roundTo)
	{
		if (roundTo <= 0)
		{
			return std::floor(x);
		}
		else
		{
			const double n10r = std::pow(10, roundTo);
			return std::round(x * n10r) / n10r;
		}
	}
	double Truncate(double x, int truncateTo)
	{
		if (truncateTo > 0)
		{
			// That's not really how you should do it, but eh... whatever
			String formatted = Format("{:.{}f}", x, std::min(truncateTo, DBL_DIG));
			if (auto value = formatted.ToFloatingPoint())
			{
				return *value;
			}
		}
		return std::floor(x);
	}

	double Log(double x, int base)
	{
		switch (base)
		{
			case 2:
			{
				return std::log(x);
			}
			case 10:
			{
				return std::log10(x);
			}
			default:
			{
				return std::log(x) / std::log(base);
			}
		};
	}
	double LdExp(double mantissa, int exp, int base)
	{
		if (base == 2)
		{
			return std::ldexp(mantissa, exp);
		}
		else
		{
			return mantissa * std::pow(base, exp);
		}
	}
	double FrExp(double x, int* exp, int base)
	{
		switch (base)
		{
			case 2:
			{
				return std::frexp(x, exp);
			}
			case 10:
			{
				*exp = static_cast<int>(std::log10(x));
				return std::pow(x / 10.0, *exp);
			}
			default:
			{
				*exp = static_cast<int>(Log(x, base));
				return std::pow(x / static_cast<double>(base), *exp);
			}
		};
	}

	double Sec(double x)
	{
		return 1.0 / std::cos(x);
	}
	double Csc(double x)
	{
		return 1.0 / std::sin(x);
	}
	double Cot(double x)
	{
		return 1.0 / std::tan(x);
	}
	double ACot(double x)
	{
		return g_PI_Half - std::atan(x);
	}
	double Sinh(double x)
	{
		return std::sinh(x);
	}
	double Cosh(double x)
	{
		return std::cosh(x);
	}
	double Tanh(double x)
	{
		return std::tanh(x);
	}
	double Coth(double x)
	{
		return (std::exp(x) + std::exp(-x)) / (std::exp(x) - std::exp(-x));
	}
	double ASinh(double x)
	{
		return std::asinh(x);
	}
	double ACosh(double x)
	{
		return std::acosh(x);
	}
	double ATanh(double x)
	{
		return std::atanh(x);
	}
	double ACoth(double x)
	{
		return ATanh(1.0 / x);
	}

	int64_t RandomInt(int64_t start, int64_t end)
	{
		double value = RandomFloat();
		value *= (double)(end - start) + 1.0;
		return (int64_t)value + start;
	}
	int64_t RandomInt(int64_t end)
	{
		return RandomInt(0, end);
	}
	double RandomFloat()
	{
		return (double)std::rand() * (1.0 / ((double)RAND_MAX + 1.0));
	}
	double RandomFloat(double start, double end)
	{
		double x = RandomInt(start, end)*RandomFloat();
		while (x < start || x > end)
		{
			x = RandomInt(start, end) * RandomFloat();
		}
		return x;
	}
	double RandomFloat(double end)
	{
		return RandomFloat(0, end);
	}

	double Map(double x, double inMin, double inMax, double outMin, double outMax)
	{
		return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
	}

	std::optional<int64_t> FromBase(const String& x, int base)
	{
		return x.ToInteger<int64_t>(base);
	}
	String ToBase(int64_t x, int base)
	{
		if (base == 10)
		{
			return ToString(x);
		}
		else if (base <= 36)
		{
			if (x != 0)
			{
				// Fill symbols table
				char symbols[36] = {0};
				for (int i = 0; i < 36; i++)
				{
					if (i < 10)
					{
						symbols[i] = i + '0';
					}
					else
					{
						symbols[i] = i + 'A' - 10;
					}
				}

				String result;
				int64_t reminder = 0;
				while (x != 0)
				{
					reminder = std::fmod(x, base);
					x = x/base;

					result.Prepend(symbols[reminder]);
				}

				return result;
			}
			return "0";
		}
		return {};
	}

	int Classify(double x)
	{
		return std::fpclassify(x);
	}
	bool IsFinite(double x)
	{
		return std::isfinite(x);
	}
	bool IsInf(double x)
	{
		return std::isinf(x);
	}
	bool IsNan(double x)
	{
		return std::isnan(x);
	}
	bool IsNormal(double x)
	{
		return std::isnormal(x);
	}
	bool SignBit(double x)
	{
		return std::signbit(x);
	}
}
