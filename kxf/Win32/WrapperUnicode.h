#pragma once
#include <utility>

// Macro to make quick template function to resolve A/W functions
#ifndef kxf_MakeWinUnicodeCallWrapper
	#ifdef UNICODE
	#define kxf_MakeWinUnicodeCallWrapper(funcName)	\
				template<class... Args>	\
				auto funcName(Args&&... arg)	\
				{	\
					return ::funcName##W(std::forward<Args>(arg)...);	\
				}
	#else
	#define kxf_MakeWinUnicodeCallWrapper(funcName)	\
						template<class... Args>	\
						auto funcName(Args&&... arg)	\
						{	\
							return ::funcName##A(std::forward<Args>(arg)...);	\
						}
	#endif
#endif
