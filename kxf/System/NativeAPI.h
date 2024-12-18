#pragma once
#include "Common.h"
#include "kxf/Win32/UndefMacros.h"

namespace kxf
{
	class FSPath;

	enum class NativeAPISet
	{
		None = -1,

		NtDLL,
		Kernel32,
		KernelBase,
		User32,
		ShlWAPI,
		DWMAPI,
		DbgHelp,
		OleAcc,
		DXGI,
		DComp
	};
}
namespace kxf::NativeAPI::Private
{
	class InitializationModule;
}

namespace kxf
{
	class KXF_API NativeAPILoader final
	{
		friend class NativeAPI::Private::InitializationModule;

		private:
			struct LibraryRecord final
			{
				public:
					void* Handle = nullptr;
					const wchar_t* Name = nullptr;
					NativeAPISet Type = NativeAPISet::None;

				public:
					bool IsNull() const noexcept
					{
						return Handle == nullptr || Name == nullptr || Type == NativeAPISet::None;
					}
			};

		public:
			static NativeAPILoader& GetInstance();

		private:
			std::array<LibraryRecord, 16> m_LoadedLibraries = {};
			String m_LookupDirectory;
			size_t m_TotalCount = 0;

		private:
			size_t GetLoadedLibrariesCount() const noexcept;
			size_t DoLoadLibraries(std::initializer_list<NativeAPISet> apiSets, bool moduleMode = false) noexcept;
			size_t DoUnloadLibraries() noexcept;

			void InitializeNtDLL() noexcept;
			void InitializeKernel32() noexcept;
			void InitializeKernelBase() noexcept;
			void InitializeUser32() noexcept;
			void InitializeShlWAPI() noexcept;
			void InitializeDWMAPI() noexcept;
			void InitializeDbgHelp() noexcept;
			void InitializeOleAcc() noexcept;
			void InitializeDXGI() noexcept;
			void InitializeDComp() noexcept;

		private:
			NativeAPILoader() noexcept;

		public:
			size_t LoadLibraries() noexcept
			{
				return DoLoadLibraries({});
			}
			size_t LoadLibraries(std::initializer_list<NativeAPISet> apiSets) noexcept
			{
				return apiSets.size() != 0 ? DoLoadLibraries(std::move(apiSets)) : 0;
			}
			size_t UnloadLibraries() noexcept
			{
				return DoUnloadLibraries();
			}
			void Initialize();
			bool IsLibraryLoaded(NativeAPISet library) const noexcept;

			void SetLookupDirectory(const FSPath& path);
	};
}

#define kxf_NativeAPI	__stdcall

#define kxf_NativeAPI_DeclateFunc(ret_type, call_conv, name, ...)	\
	using T##name = ret_type (call_conv*)(__VA_ARGS__);	\
	extern T##name name

struct _GUID;
struct IDXGIDevice;

namespace kxf::NativeAPI
{
	namespace
	{
		using BOOL = int32_t;
		using LONG = int32_t;
		using UINT = uint32_t;
		using ULONG = uint32_t;
		using DWORD = uint32_t;
		using SIZE_T = size_t;

		using HWND = void*;
		using HANDLE = void*;
		using MARGINS = void;
		using DWM_BLURBEHIND = void;
		using IMAGE_NT_HEADERS = void;
		using DPI_AWARENESS_CONTEXT = void*;
		using RTL_OSVERSIONINFOEXW = void;
		using DLL_DIRECTORY_COOKIE = void*;
		using NTSTATUS = int32_t;
		using HRESULT = int32_t;
		using GUID = ::_GUID;
		using IID = ::_GUID;

		using LDR_DLL_NOTIFICATION_DATA = void;
		using LDR_DLL_NOTIFICATION_FUNCTION = void(kxf_NativeAPI*)(ULONG reason, const LDR_DLL_NOTIFICATION_DATA* data, void* context);
	}

	namespace NtDLL
	{
		kxf_NativeAPI_DeclateFunc(NTSTATUS, kxf_NativeAPI, RtlGetVersion, RTL_OSVERSIONINFOEXW*);
		kxf_NativeAPI_DeclateFunc(NTSTATUS, kxf_NativeAPI, NtQueryInformationProcess, HANDLE, int, void*, ULONG, ULONG*);
		kxf_NativeAPI_DeclateFunc(NTSTATUS, kxf_NativeAPI, RtlGetLastNtStatus);
		kxf_NativeAPI_DeclateFunc(void, kxf_NativeAPI, RtlSetLastWin32ErrorAndNtStatusFromNtStatus, NTSTATUS);
		kxf_NativeAPI_DeclateFunc(ULONG, kxf_NativeAPI, RtlNtStatusToDosError, ULONG);
		kxf_NativeAPI_DeclateFunc(LONG, kxf_NativeAPI, NtSuspendProcess, HANDLE);
		kxf_NativeAPI_DeclateFunc(LONG, kxf_NativeAPI, NtResumeProcess, HANDLE);
		kxf_NativeAPI_DeclateFunc(NTSTATUS, kxf_NativeAPI, LdrRegisterDllNotification, ULONG, LDR_DLL_NOTIFICATION_FUNCTION, void*, void**);
		kxf_NativeAPI_DeclateFunc(NTSTATUS, kxf_NativeAPI, LdrUnregisterDllNotification, void*);
	}
	namespace Kernel32
	{
		kxf_NativeAPI_DeclateFunc(BOOL, kxf_NativeAPI, Wow64DisableWow64FsRedirection, void**);
		kxf_NativeAPI_DeclateFunc(BOOL, kxf_NativeAPI, Wow64RevertWow64FsRedirection, void*);

		kxf_NativeAPI_DeclateFunc(BOOL, kxf_NativeAPI, IsWow64Process, HANDLE, BOOL*);
		kxf_NativeAPI_DeclateFunc(BOOL, kxf_NativeAPI, IsWow64Process2, HANDLE, uint16_t*, uint16_t*);

		kxf_NativeAPI_DeclateFunc(BOOL, kxf_NativeAPI, SetDefaultDllDirectories, DWORD);
		kxf_NativeAPI_DeclateFunc(DLL_DIRECTORY_COOKIE, kxf_NativeAPI, AddDllDirectory, const wchar_t*);
		kxf_NativeAPI_DeclateFunc(BOOL, kxf_NativeAPI, RemoveDllDirectory, DLL_DIRECTORY_COOKIE);
		kxf_NativeAPI_DeclateFunc(BOOL, kxf_NativeAPI, SetDllDirectoryW, const wchar_t*);
		kxf_NativeAPI_DeclateFunc(DWORD, kxf_NativeAPI, GetDllDirectoryW, DWORD, wchar_t*);
		kxf_NativeAPI_DeclateFunc(HRESULT, kxf_NativeAPI, GetThreadDescription, HANDLE, wchar_t**);
		kxf_NativeAPI_DeclateFunc(HRESULT, kxf_NativeAPI, SetThreadDescription, HANDLE, const wchar_t*);

		kxf_NativeAPI_DeclateFunc(void*, kxf_NativeAPI, VirtualAlloc2, HANDLE, void*, SIZE_T, ULONG, ULONG, void*, ULONG);
	}
	namespace KernelBase
	{
		kxf_NativeAPI_DeclateFunc(HRESULT, kxf_NativeAPI, PathCchCanonicalizeEx, wchar_t*, size_t, const wchar_t*, ULONG);
	}
	namespace User32
	{
		kxf_NativeAPI_DeclateFunc(UINT, kxf_NativeAPI, GetDpiForSystem);
		kxf_NativeAPI_DeclateFunc(UINT, kxf_NativeAPI, GetDpiForWindow, HWND);
		kxf_NativeAPI_DeclateFunc(BOOL, kxf_NativeAPI, EnableNonClientDpiScaling, HWND);
		kxf_NativeAPI_DeclateFunc(DPI_AWARENESS_CONTEXT, kxf_NativeAPI, SetThreadDpiAwarenessContext, DPI_AWARENESS_CONTEXT);
	}
	namespace ShlWAPI
	{
		kxf_NativeAPI_DeclateFunc(BOOL, kxf_NativeAPI, PathCanonicalizeW, wchar_t*, const wchar_t*);
	}
	namespace DWMAPI
	{
		kxf_NativeAPI_DeclateFunc(HRESULT, kxf_NativeAPI, DwmIsCompositionEnabled, BOOL*);
		kxf_NativeAPI_DeclateFunc(HRESULT, kxf_NativeAPI, DwmGetColorizationColor, DWORD*, BOOL*);
		kxf_NativeAPI_DeclateFunc(HRESULT, kxf_NativeAPI, DwmExtendFrameIntoClientArea, HWND, const MARGINS*);
		kxf_NativeAPI_DeclateFunc(HRESULT, kxf_NativeAPI, DwmEnableBlurBehindWindow, HWND, const DWM_BLURBEHIND*);
	}
	namespace DbgHelp
	{
		kxf_NativeAPI_DeclateFunc(IMAGE_NT_HEADERS*, kxf_NativeAPI, ImageNtHeader, void*);
	}
	namespace OleAcc
	{
		kxf_NativeAPI_DeclateFunc(HANDLE, kxf_NativeAPI, GetProcessHandleFromHwnd, HWND);
	}
	namespace DXGI
	{
		kxf_NativeAPI_DeclateFunc(HRESULT, kxf_NativeAPI, CreateDXGIFactory1, const ::IID&, void**);
		kxf_NativeAPI_DeclateFunc(HRESULT, kxf_NativeAPI, CreateDXGIFactory2, UINT, const ::IID&, void**);
	}
	namespace DComp
	{
		kxf_NativeAPI_DeclateFunc(HRESULT, kxf_NativeAPI, DCompositionCreateDevice, IDXGIDevice*, const ::IID&, void**);
	}
}

#undef kxf_NativeAPI
#undef kxf_NativeAPI_DeclateFunc
