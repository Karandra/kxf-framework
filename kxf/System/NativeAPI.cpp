#include "KxfPCH.h"
#include "NativeAPI.h"
#include "kxf/Log/Common.h"
#include "kxf/Utility/Common.h"
#include <Windows.h>
#include <wx/module.h>
#include "UndefWindows.h"

#define DECLARE_LIBRARY(name)	\
static_assert(static_cast<size_t>(NativeAPISet::##name) < Utility::ArraySize<decltype(m_LoadedLibraries)>::value);	\
{	\
	auto& item = m_LoadedLibraries[static_cast<size_t>(NativeAPISet::##name)];	\
	\
	item.Name = L#name L".dll";	\
	item.Type = NativeAPISet::name;	\
	\
	m_Count++;	\
}

#define DEFINE_FUNCTION(name)										T##name name = nullptr
#define INIT_FUNCTION_AS(dll, name, in_dll_name)					dll##::name = reinterpret_cast<dll##::T##name>(::GetProcAddress(reinterpret_cast<HMODULE>(m_LoadedLibraries[static_cast<size_t>(NativeAPISet::##dll)].Handle), #in_dll_name))
#define INIT_FUNCTION(dll, name)									INIT_FUNCTION_AS(dll, name, name)

namespace kxf
{
	NativeAPILoader& NativeAPILoader::GetInstance()
	{
		static NativeAPILoader instance;
		return instance;
	}

	NativeAPILoader::NativeAPILoader() noexcept
	{
		m_LoadedLibraries.fill({});

		using namespace NativeAPI;
		DECLARE_LIBRARY(NtDLL);
		DECLARE_LIBRARY(Kernel32);
		DECLARE_LIBRARY(KernelBase);
		DECLARE_LIBRARY(User32);
		DECLARE_LIBRARY(ShlWAPI);
		DECLARE_LIBRARY(DWMAPI);
		DECLARE_LIBRARY(DbgHelp);
		DECLARE_LIBRARY(DXGI);
		DECLARE_LIBRARY(DComp);
	}

	size_t NativeAPILoader::DoLoadLibraries(std::initializer_list<NativeAPISet> apiSets) noexcept
	{
		if (!m_IsLoaded)
		{
			size_t count = 0;
			for (size_t i = 0; i < m_Count; i++)
			{
				auto& item = m_LoadedLibraries[i];
				if (apiSets.size() == 0 || std::find(apiSets.begin(), apiSets.end(), item.Type) != apiSets.end())
				{
					if (item.Handle = ::LoadLibraryW(item.Name))
					{
						count++;
					}
					else
					{
						Log::Warning("Couldn't load \"{}\" library", item.Name);
					}
				}
			}

			m_IsLoaded = count != 0;
			return count;
		}
		return 0;
	}
	size_t NativeAPILoader::DoUnloadLibraries() noexcept
	{
		if (m_IsLoaded)
		{
			size_t count = 0;
			for (size_t i = 0; i < m_Count; i++)
			{
				auto& item = m_LoadedLibraries[i];
				if (item.Handle)
				{
					::FreeLibrary(reinterpret_cast<HMODULE>(item.Handle));
					item.Handle = nullptr;
					count++;
				}
			}

			m_IsLoaded = false;
			return count;
		}
		return 0;
	}

	void NativeAPILoader::InitializeNtDLL() noexcept
	{
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::NtDLL))
		{
			INIT_FUNCTION(NtDLL, RtlGetVersion);
			INIT_FUNCTION(NtDLL, NtQueryInformationProcess);
			INIT_FUNCTION(NtDLL, RtlGetLastNtStatus);
			INIT_FUNCTION(NtDLL, RtlSetLastWin32ErrorAndNtStatusFromNtStatus);
			INIT_FUNCTION(NtDLL, RtlNtStatusToDosError);
			INIT_FUNCTION(NtDLL, NtSuspendProcess);
			INIT_FUNCTION(NtDLL, NtResumeProcess);
			INIT_FUNCTION(NtDLL, LdrRegisterDllNotification);
			INIT_FUNCTION(NtDLL, LdrUnregisterDllNotification);
		}
	}
	void NativeAPILoader::InitializeKernel32() noexcept
	{
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::Kernel32))
		{
			INIT_FUNCTION(Kernel32, Wow64DisableWow64FsRedirection);
			INIT_FUNCTION(Kernel32, Wow64RevertWow64FsRedirection);

			INIT_FUNCTION(Kernel32, IsWow64Process);

			INIT_FUNCTION(Kernel32, SetDefaultDllDirectories);
			INIT_FUNCTION(Kernel32, AddDllDirectory);
			INIT_FUNCTION(Kernel32, RemoveDllDirectory);
			INIT_FUNCTION(Kernel32, SetDllDirectoryW);
			INIT_FUNCTION(Kernel32, GetDllDirectoryW);
		}
	}
	void NativeAPILoader::InitializeKernelBase() noexcept
	{
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::KernelBase))
		{
			INIT_FUNCTION(KernelBase, PathCchCanonicalizeEx);
		}
	}
	void NativeAPILoader::InitializeUser32() noexcept
	{
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::User32))
		{
			INIT_FUNCTION(User32, EnableNonClientDpiScaling);
			INIT_FUNCTION(User32, SetThreadDpiAwarenessContext);
			INIT_FUNCTION(User32, GetDpiForSystem);
			INIT_FUNCTION(User32, GetDpiForWindow);
		}
	}
	void NativeAPILoader::InitializeShlWAPI() noexcept
	{
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::ShlWAPI))
		{
			INIT_FUNCTION(ShlWAPI, PathCanonicalizeW);
		}
	}
	void NativeAPILoader::InitializeDWMAPI() noexcept
	{
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::DWMAPI))
		{
			INIT_FUNCTION(DWMAPI, DwmIsCompositionEnabled);
			INIT_FUNCTION(DWMAPI, DwmGetColorizationColor);
			INIT_FUNCTION(DWMAPI, DwmExtendFrameIntoClientArea);
			INIT_FUNCTION(DWMAPI, DwmEnableBlurBehindWindow);
		}
	}
	void NativeAPILoader::InitializeDbgHelp() noexcept
	{
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::DbgHelp))
		{
			INIT_FUNCTION(DbgHelp, ImageNtHeader);
		}
	}
	void NativeAPILoader::InitializeDXGI() noexcept
	{
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::DXGI))
		{
			INIT_FUNCTION(DXGI, CreateDXGIFactory1);
			INIT_FUNCTION(DXGI, CreateDXGIFactory2);
		}
	}
	void NativeAPILoader::InitializeDComp() noexcept
	{
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::DComp))
		{
			INIT_FUNCTION(DComp, DCompositionCreateDevice);
		}
	}

	bool NativeAPILoader::IsLibraryLoaded(NativeAPISet library) const noexcept
	{
		if (m_IsLoaded)
		{
			const size_t index = static_cast<size_t>(library);
			return index < m_LoadedLibraries.size() && m_LoadedLibraries[index].Handle;
		}
		return false;
	}
}

namespace kxf::NativeAPI
{
	namespace NtDLL
	{
		DEFINE_FUNCTION(RtlGetVersion);
		DEFINE_FUNCTION(NtQueryInformationProcess);
		DEFINE_FUNCTION(RtlGetLastNtStatus);
		DEFINE_FUNCTION(RtlSetLastWin32ErrorAndNtStatusFromNtStatus);
		DEFINE_FUNCTION(RtlNtStatusToDosError);
		DEFINE_FUNCTION(NtSuspendProcess);
		DEFINE_FUNCTION(NtResumeProcess);
		DEFINE_FUNCTION(LdrRegisterDllNotification);
		DEFINE_FUNCTION(LdrUnregisterDllNotification);
	}
	namespace Kernel32
	{
		DEFINE_FUNCTION(Wow64DisableWow64FsRedirection);
		DEFINE_FUNCTION(Wow64RevertWow64FsRedirection);

		DEFINE_FUNCTION(IsWow64Process);

		DEFINE_FUNCTION(SetDefaultDllDirectories);
		DEFINE_FUNCTION(AddDllDirectory);
		DEFINE_FUNCTION(RemoveDllDirectory);
		DEFINE_FUNCTION(SetDllDirectoryW);
		DEFINE_FUNCTION(GetDllDirectoryW);
	}
	namespace KernelBase
	{
		DEFINE_FUNCTION(PathCchCanonicalizeEx);
	}
	namespace User32
	{
		DEFINE_FUNCTION(EnableNonClientDpiScaling);
		DEFINE_FUNCTION(SetThreadDpiAwarenessContext);
		DEFINE_FUNCTION(GetDpiForSystem);
		DEFINE_FUNCTION(GetDpiForWindow);
	}
	namespace ShlWAPI
	{
		DEFINE_FUNCTION(PathCanonicalizeW);
	}
	namespace DWMAPI
	{
		DEFINE_FUNCTION(DwmIsCompositionEnabled);
		DEFINE_FUNCTION(DwmGetColorizationColor);
		DEFINE_FUNCTION(DwmExtendFrameIntoClientArea);
		DEFINE_FUNCTION(DwmEnableBlurBehindWindow);
	}
	namespace DbgHelp
	{
		DEFINE_FUNCTION(ImageNtHeader);
	}
	namespace DXGI
	{
		DEFINE_FUNCTION(CreateDXGIFactory1);
		DEFINE_FUNCTION(CreateDXGIFactory2);
	}
	namespace DComp
	{
		DEFINE_FUNCTION(DCompositionCreateDevice);
	}
}

namespace kxf::NativeAPI::Private
{
	class InitializationModule final: public wxModule
	{
		private:
			NativeAPILoader& m_Loader = NativeAPILoader::GetInstance();

		public:
			bool OnInit() noexcept override
			{
				if (m_Loader.LoadLibraries() != 0)
				{
					m_Loader.InitializeNtDLL();
					m_Loader.InitializeKernel32();
					m_Loader.InitializeKernelBase();
					m_Loader.InitializeUser32();
					m_Loader.InitializeShlWAPI();
					m_Loader.InitializeDWMAPI();
					m_Loader.InitializeDbgHelp();
					m_Loader.InitializeDXGI();
					m_Loader.InitializeDComp();

					return true;
				}
				return false;
			}
			void OnExit() noexcept override
			{
				m_Loader.UnloadLibraries();
			}

		private:
			wxDECLARE_DYNAMIC_CLASS(InitializationModule);
	};
}

wxIMPLEMENT_DYNAMIC_CLASS(kxf::NativeAPI::Private::InitializationModule, wxModule);
