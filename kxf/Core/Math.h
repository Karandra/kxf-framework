#pragma once
#include "Common.h"
#include "String.h"

namespace kxf::Math
{
	KXF_API UniChar GetDecimalSeparator();
	KXF_API UniChar GetDecimalCSeparator();

	KXF_API double GetPI();
	KXF_API double GetHalfPI();

	KXF_API double Round(double x, int roundTo = 0);
	KXF_API double Truncate(double x, int truncateTo = 0);

	KXF_API double Log(double x, int base);
	KXF_API double LdExp(double mantissa, int64_t exp, int base);
	KXF_API double FrExp(double x, int* exp, int base);

	KXF_API double Sec(double x);
	KXF_API double Csc(double x);
	KXF_API double Cot(double x);
	KXF_API double ACot(double x);
	KXF_API double Sinh(double x);
	KXF_API double Cosh(double x);
	KXF_API double Tanh(double x);
	KXF_API double Coth(double x);
	KXF_API double ASinh(double x);
	KXF_API double ACosh(double x);
	KXF_API double ATanh(double x);
	KXF_API double ACoth(double x);

	KXF_API int64_t RandomInt(int64_t start, int64_t end);
	KXF_API int64_t RandomInt(int64_t end);
	KXF_API double RandomFloat();
	KXF_API double RandomFloat(double start, double end);
	KXF_API double RandomFloat(double end);

	KXF_API double Map(double x, double inMin, double inMax, double outMin, double outMax);

	KXF_API std::optional<int64_t> FromBase(const String& x, int base = 10);
	KXF_API String ToBase(int64_t x, int base = 10);

	KXF_API int Classify(double x);
	KXF_API bool IsFinite(double x);
	KXF_API bool IsInf(double x);
	KXF_API bool IsNan(double x);
	KXF_API bool IsNormal(double x);
	KXF_API bool SignBit(double x);
};
