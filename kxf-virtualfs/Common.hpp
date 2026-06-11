#pragma once
#include "kxf/Common.hpp"

#ifdef KXF_DYNAMIC_LIBRARY
	#ifdef KXF_LIBRARY_VIRTUALFS
		#define KXF_API_VIRTUALFS __declspec(dllexport)
	#else
		#define KXF_API_VIRTUALFS __declspec(dllimport)
	#endif
#elif defined KXF_STATIC_LIBRARY
	#define KXF_API_VIRTUALFS
#endif
