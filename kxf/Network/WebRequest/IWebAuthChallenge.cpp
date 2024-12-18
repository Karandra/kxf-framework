#include "kxf-pch.h"
#include "IWebAuthChallenge.h"

namespace
{
	kxf::NullWebAuthChallenge g_NullWebAuthChallenge;
}

namespace kxf
{
	IWebAuthChallenge& NullWebAuthChallenge::Get()
	{
		return g_NullWebAuthChallenge;
	}
}
