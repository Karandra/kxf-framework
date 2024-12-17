#include "KxfPCH.h"
#include "Common.h"
#include "SciterAPI.h"
#include "kxf/Log/ScopedLogger.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/System/ExecutableVersionResource.h"

namespace
{
	void* g_SciterLibrary = nullptr;
}

namespace kxf::Sciter
{
	String GetLibraryName()
	{
		return "Sciter-JS";
	}
	Version GetLibraryVersion()
	{
		if (g_SciterLibrary)
		{
			DynamicLibrary library;
			library.AttachHandle(g_SciterLibrary);

			ExecutableVersionResource versionResource(library.GetFilePath());
			if (versionResource)
			{
				return versionResource.GetAnyVersion();
			}
		}
		return {};
	}

	bool IsLibraryLoaded()
	{
		return g_SciterLibrary != nullptr;
	}
	void* LoadLibrary(const FSPath& path)
	{
		if (!g_SciterLibrary)
		{
			auto fullPath = path.GetFullPathTryNS(FSPathNamespace::Win32File);
			Log::Info("Loading Sciter DLL from: '{}'", fullPath);

			g_SciterLibrary = ::LoadLibraryW(fullPath.wc_str());
			if (!g_SciterLibrary)
			{
				Log::Error("Failed to loader Sciter DLL");
			}
		}
		return g_SciterLibrary;
	}
	void FreeLibrary()
	{
		// Sciter API includes don't have function to unload its DLL and it causes a crash
		// if we do unload the DLL manually. So we're going to let it loaded itself when process exists.
		// It seems Sciter does something in its DllMain's 'DLL_PROCESS_DETACH' event.
		#if 0
		if (g_SciterLibrary)
		{
			::FreeLibrary(reinterpret_cast<HMODULE>(g_SciterLibrary));
			g_SciterLibrary = nullptr;
		}
		#endif
	}
}

namespace kxf::Sciter
{
	String SizeUnitToString(SizeUnit unit)
	{
		#define Map(unit)	\
			case SizeUnit::##unit:	\
			{	\
				return wxS(#unit);	\
			}	\

		switch (unit)
		{
			// Absolute
			Map(cm);
			Map(mm);
			Map(in);
			Map(px);
			Map(pt);
			Map(pc);
			Map(dip);

			// Relative
			Map(em);
			Map(ex);
			Map(ch);
			Map(rem);
			Map(vw);
			Map(vh);
			Map(vmin);
			Map(vmax);

			case SizeUnit::pr:
			{
				return "%";
			}
			case SizeUnit::sp:
			{
				return "%%";
			}
		};
		return {};

		#undef Map
	}

	CornerAlignment MapCornerAlignment(FlagSet<Alignment> alignment)
	{
		if (alignment == Alignment::Invalid)
		{
			return CornerAlignment::None;
		}
		else if (alignment & Alignment::Right)
		{
			// Right alignment
			if (alignment & Alignment::CenterVertical)
			{
				return CornerAlignment::MiddleRight;
			}
			else if (alignment & Alignment::Bottom)
			{
				return CornerAlignment::BottomRight;
			}
			else
			{
				return CornerAlignment::TopRight;
			}
		}
		else if (alignment & wxALIGN_CENTRE_HORIZONTAL)
		{
			// Horizontally-centered
			if (alignment & Alignment::CenterVertical)
			{
				return CornerAlignment::MiddleCenter;
			}
			else if (alignment & Alignment::Bottom)
			{
				return CornerAlignment::BottomCenter;
			}
			else
			{
				return CornerAlignment::TopCenter;
			}
		}
		else
		{
			// Left aligned
			if (alignment & Alignment::CenterVertical)
			{
				return CornerAlignment::MiddleLeft;
			}
			else if (alignment & Alignment::Bottom)
			{
				return CornerAlignment::BottomLeft;
			}
			else
			{
				return CornerAlignment::TopLeft;
			}
		}
		return CornerAlignment::None;
	}
	FlagSet<Alignment> MapCornerAlignment(CornerAlignment alignment)
	{
		switch (alignment)
		{
			case CornerAlignment::TopLeft:
			{
				return Alignment::Left;
			}
			case CornerAlignment::TopCenter:
			{
				return Alignment::CenterHorizontal;
			}
			case CornerAlignment::TopRight:
			{
				return Alignment::Right;
			}

			case CornerAlignment::MiddleLeft:
			{
				return Alignment::CenterVertical;
			}
			case CornerAlignment::MiddleCenter:
			{
				return Alignment::CenterVertical|Alignment::CenterHorizontal;
			}
			case CornerAlignment::MiddleRight:
			{
				return Alignment::Right|Alignment::CenterVertical;
			}

			case CornerAlignment::BottomLeft:
			{
				return Alignment::Bottom;
			}
			case CornerAlignment::BottomCenter:
			{
				return Alignment::Bottom|Alignment::CenterHorizontal;
			}
			case CornerAlignment::BottomRight:
			{
				return Alignment::Bottom|Alignment::Right;
			}
		};
		return Alignment::Invalid;
	}
}
