#include "kxf-pch.h"
#include "COM.h"
#include "kxf/Utility/Common.h"

#include <Windows.h>
#include <combaseapi.h>
#include <unknwn.h>
#include "kxf/Win32/UndefMacros.h"

namespace
{
	using namespace kxf;

	constexpr FlagSet<DWORD> MapCOMInitFlag(FlagSet<COMInitFlag> flags) noexcept
	{
		FlagSet<DWORD> nativeFlags;
		nativeFlags.Add(COINIT::COINIT_DISABLE_OLE1DDE, flags & COMInitFlag::DisableOLE1DDE);
		nativeFlags.Add(COINIT::COINIT_SPEED_OVER_MEMORY, flags & COMInitFlag::SpeedOverMemory);

		return nativeFlags;
	}
	constexpr FlagSet<CLSCTX> MapClassContext(FlagSet<COMClassContext> context) noexcept
	{
		FlagSet<CLSCTX> nativeContext;
		nativeContext.Add(CLSCTX::CLSCTX_INPROC_SERVER, context & COMClassContext::InprocServer);
		nativeContext.Add(CLSCTX::CLSCTX_INPROC_HANDLER, context & COMClassContext::InprocHandler);
		nativeContext.Add(CLSCTX::CLSCTX_LOCAL_SERVER, context & COMClassContext::LocalServer);
		nativeContext.Add(CLSCTX::CLSCTX_REMOTE_SERVER, context & COMClassContext::RemoteServer);
		nativeContext.Add(CLSCTX::CLSCTX_ENABLE_CODE_DOWNLOAD, context & COMClassContext::EnableCodeDownload);
		nativeContext.Add(CLSCTX::CLSCTX_NO_CODE_DOWNLOAD, context & COMClassContext::NoCodeDownload);
		nativeContext.Add(CLSCTX::CLSCTX_NO_CUSTOM_MARSHAL, context & COMClassContext::NoCustomMarshal);
		nativeContext.Add(CLSCTX::CLSCTX_NO_FAILURE_LOG, context & COMClassContext::NoFailureLog);
		nativeContext.Add(CLSCTX::CLSCTX_DISABLE_AAA, context & COMClassContext::DisableAAA);
		nativeContext.Add(CLSCTX::CLSCTX_ENABLE_AAA, context & COMClassContext::EnableAAA);
		nativeContext.Add(CLSCTX::CLSCTX_ACTIVATE_AAA_AS_IU, context & COMClassContext::ActivateAAAAsIU);
		nativeContext.Add(CLSCTX::CLSCTX_FROM_DEFAULT_CONTEXT, context & COMClassContext::FromDefaultContext);
		nativeContext.Add(CLSCTX::CLSCTX_ACTIVATE_32_BIT_SERVER, context & COMClassContext::Activate32BitServer);
		nativeContext.Add(CLSCTX::CLSCTX_ACTIVATE_64_BIT_SERVER, context & COMClassContext::Activate64BitServer);
		nativeContext.Add(CLSCTX::CLSCTX_ACTIVATE_ARM32_SERVER, context & COMClassContext::ActivateARM32Server);
		nativeContext.Add(CLSCTX::CLSCTX_ENABLE_CLOAKING, context & COMClassContext::EnableCloaking);
		nativeContext.Add(CLSCTX::CLSCTX_APPCONTAINER, context & COMClassContext::AppContainer);

		return nativeContext;
	}
}

namespace kxf::COM
{
	void* AllocateMemory(size_t size) noexcept
	{
		return ::CoTaskMemAlloc(size);
	}
	void* ReallocateMemory(void* address, size_t size) noexcept
	{
		return ::CoTaskMemRealloc(address, size);
	}
	void FreeMemory(void* address) noexcept
	{
		::CoTaskMemFree(address);
	}

	wchar_t* AllocateBSTR(const wchar_t* data) noexcept
	{
		return ::SysAllocString(data);
	}
	void FreeBSTR(wchar_t* data) noexcept
	{
		::SysFreeString(data);
	}

	::_GUID ToGUID(const NativeUUID& uuid) noexcept
	{
		::GUID guid = {};
		guid.Data1 = uuid.Data1;
		guid.Data2 = uuid.Data2;
		guid.Data3 = uuid.Data3;
		for (size_t i = 0; i < std::size(guid.Data4); i++)
		{
			guid.Data4[i] = uuid.Data4[i];
		}

		return guid;
	}
	NativeUUID FromGUID(const ::_GUID& guid) noexcept
	{
		NativeUUID uuid;
		uuid.Data1 = guid.Data1;
		uuid.Data2 = guid.Data2;
		uuid.Data3 = guid.Data3;
		for (size_t i = 0; i < std::size(uuid.Data4); i++)
		{
			uuid.Data4[i] = guid.Data4[i];
		}

		return uuid;
	}

	HResult CreateInstance(const NativeUUID& classID, COMClassContext classContext, const NativeUUID& iid, void** result, IUnknown* outer) noexcept
	{
		return ::CoCreateInstance(ToGUID(classID), outer, MapClassContext(classContext), ToGUID(iid), result);
	}
}

namespace kxf
{
	void COMInitGuard::DoInitialize(COMThreadingModel threadingModel, FlagSet<COMInitFlag> flags) noexcept
	{
		if (!IsInitialized())
		{
			switch (threadingModel)
			{
				case COMThreadingModel::Apartment:
				{
					m_Status = ::CoInitializeEx(nullptr, COINIT::COINIT_APARTMENTTHREADED|*MapCOMInitFlag(flags));
				}
				case COMThreadingModel::Concurrent:
				{
					m_Status = ::CoInitializeEx(nullptr, COINIT::COINIT_MULTITHREADED|*MapCOMInitFlag(flags));
				}
			};
		}
	}
	void COMInitGuard::DoUninitialize() noexcept
	{
		if (IsInitialized())
		{
			::CoUninitialize();
			m_Status = HResult::Pending();
		}
	}

	bool COMInitGuard::IsInitialized() const noexcept
	{
		return m_Status.IsSuccess() || m_Status == RPC_E_CHANGED_MODE;
	}
}

namespace kxf
{
	void OLEInitGuard::DoInitialize() noexcept
	{
		if (!IsInitialized())
		{
			m_Status = ::OleInitialize(nullptr);
		}
	}
	void OLEInitGuard::DoUninitialize() noexcept
	{
		if (IsInitialized())
		{
			::OleUninitialize();
			m_Status = HResult::Pending();
		}
	}
}
