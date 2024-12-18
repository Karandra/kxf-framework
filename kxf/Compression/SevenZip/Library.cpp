#include "kxf-pch.h"
#include "Library.h"
#include "kxf/System/HResult.h"

namespace
{
	using CreateObjectFunc = HRESULT(__stdcall*)(const ::GUID* classID, const ::GUID* interfaceID, void** outObject);

	constexpr auto g_DefaultLibraryPath = "7z.dll";
}

namespace kxf::SevenZip
{
	bool Library::Load()
	{
		return Load(g_DefaultLibraryPath);
	}
	bool Library::Load(const FSPath& libraryPath)
	{
		if (IsLoaded())
		{
			return true;
		}

		if (m_Library.Load(libraryPath))
		{
			m_CreateObjectFunc = m_Library.GetExportedFunctionAddress("CreateObject");
			return m_CreateObjectFunc != nullptr;
		}
		return false;
	}
	void Library::Unload() noexcept
	{
		if (m_Library)
		{
			m_Library.Unload();
		}
		m_CreateObjectFunc = nullptr;
	}

	bool Library::CreateObject(const NativeUUID& classID, const NativeUUID& interfaceID, void** object) const
	{
		if (!IsLoaded())
		{
			throw std::runtime_error("7-Zip library isn't loaded");
		}

		::GUID classGUID = COM::ToGUID(classID);
		::GUID interfaceGUID = COM::ToGUID(interfaceID);
		return HResult(std::invoke(reinterpret_cast<CreateObjectFunc>(m_CreateObjectFunc), &classGUID, &interfaceGUID, object)).IsSuccess();
	}
}
