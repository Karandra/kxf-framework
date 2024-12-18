#include "kxf-pch.h"
#include "DynamicLibrary.h"
#include "kxf/System/NativeAPI.h"
#include "kxf/System/Private/System.h"
#include "kxf/System/Private/BinaryResourceDefines.h"
#include "kxf/FileSystem/NativeFileSystem.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/INativeStream.h"
#include "kxf/Drawing/ImageBundle.h"
#include "kxf/Drawing/GDIRenderer/GDIBitmap.h"
#include "kxf/Drawing/GDIRenderer/GDICursor.h"
#include "kxf/Drawing/GDIRenderer/GDIIcon.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/String.h"
#include "kxf/Utility/ScopeGuard.h"

#include <Windows.h>
#include <DbgHelp.h>
#include <cwchar>
#include "kxf/Win32/UndefMacros.h"
#pragma comment(lib, "DbgHelp.lib")

namespace
{
	EXTERN_C IMAGE_DOS_HEADER __ImageBase;

	using namespace kxf;

	HMODULE AsHMODULE(void* handle) noexcept
	{
		return reinterpret_cast<HMODULE>(handle);
	}
	LPCWSTR GetNameOrID(const String& name)
	{
		if (auto id = name.ToInteger<ULONG>())
		{
			return MAKEINTRESOURCEW(*id);
		}
		return name.wc_str();
	}
	DWORD GetLangID(const Locale& locale) noexcept
	{
		if (auto langID = locale.GetLangID())
		{
			return langID->ID;
		}
		return MAKELANGID(LANG_NEUTRAL, SORT_DEFAULT);
	}

	bool IsHandleDataFile(void* handle) noexcept
	{
		return reinterpret_cast<ULONG_PTR>(handle) & static_cast<ULONG_PTR>(1);
	}
	bool IsHandleImageResourse(void* handle) noexcept
	{
		return reinterpret_cast<ULONG_PTR>(handle) & static_cast<ULONG_PTR>(2);
	}

	constexpr FlagSet<uint32_t> MapDynamicLibraryLoadFlags(FlagSet<DynamicLibraryFlag> flags) noexcept
	{
		FlagSet<uint32_t> nativeFlags;
		nativeFlags.Add(LOAD_LIBRARY_AS_DATAFILE, flags & DynamicLibraryFlag::Resource);
		nativeFlags.Add(LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE, flags & DynamicLibraryFlag::Resource && flags & DynamicLibraryFlag::Exclusive);
		nativeFlags.Add(LOAD_LIBRARY_AS_IMAGE_RESOURCE, flags & DynamicLibraryFlag::ImageResource);
		nativeFlags.Add(LOAD_LIBRARY_SEARCH_USER_DIRS, flags & DynamicLibraryFlag::SearchUserDirectories);
		nativeFlags.Add(LOAD_LIBRARY_SEARCH_SYSTEM32, flags & DynamicLibraryFlag::SearchSystemDirectories);
		nativeFlags.Add(LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR, flags & DynamicLibraryFlag::SearchLibraryDirectory);
		nativeFlags.Add(LOAD_LIBRARY_SEARCH_APPLICATION_DIR, flags & DynamicLibraryFlag::SearchApplicationDirectory);

		return nativeFlags;
	}

	template<class TFunc>
	struct CallContext final
	{
		TFunc& Callable;
		size_t CallCount = 0;

		CallContext(TFunc& func)
			:Callable(func)
		{
		}
	};

	template<class TFunc>
	void MapDLL(void* handle, TFunc&& func)
	{
		// The unfortunate interaction between LOAD_LIBRARY_AS_DATAFILE and DialogBox
		// https://devblogs.microsoft.com/oldnewthing/20051006-09/?p=33883
		const auto base = reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(handle) & ~(static_cast<uintptr_t>(0xFFFF)));

		auto dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
		auto ntHeader = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base +  dosHeader->e_lfanew);
		auto sections = IMAGE_FIRST_SECTION(ntHeader);
		auto AdjustRVA = [&](uintptr_t rva) -> uintptr_t
		{
			for (size_t i = 0; i < ntHeader->FileHeader.NumberOfSections; i++)
			{
				const auto section = sections[i];
				if (section.VirtualAddress <= rva && rva < section.VirtualAddress + section.Misc.VirtualSize)
				{
					return rva - section.VirtualAddress + section.PointerToRawData;
				}
			}
			return 0;
		};

		func(base, dosHeader, ntHeader, sections, AdjustRVA);
	}

	constexpr Size g_DefaultIconSize = {0, 0};

	HRSRC GetResourceHandle(HMODULE handle, const String& type, const String& name, const Locale& locale)
	{
		return ::FindResourceExW(handle, GetNameOrID(type), GetNameOrID(name), GetLangID(locale));
	}
	std::span<const std::byte> GetResourceData(HMODULE handle, HRSRC resHandle)
	{
		if (resHandle)
		{
			if (HGLOBAL resDataHandle = ::LoadResource(handle, resHandle))
			{
				void* resData = ::LockResource(resDataHandle);
				DWORD resSize = ::SizeofResource(handle, resHandle);
				if (resData && resSize != 0)
				{
					return {reinterpret_cast<const std::byte*>(resData), resSize};
				}
			}
		}
		return {};
	}

	HANDLE DoLoadGDIImage(HMODULE handle, const String& name, const String& type, UINT gdiType, Size size, const Locale& locale)
	{
		if (HGLOBAL resDataHandle = ::LoadResource(handle, GetResourceHandle(handle, type, name, locale)))
		{
			return ::LoadImageW(handle, GetNameOrID(name), gdiType, size.GetWidth(), size.GetHeight(), LR_DEFAULTCOLOR);
		}
		return nullptr;
	}

	template<class T>
	T LoadGDIImage(HMODULE handle, const String& name, const String& type, UINT gdiType, Size size, const Locale& locale)
	{
		if (HANDLE imageHandle = DoLoadGDIImage(handle, name, type, gdiType, size, locale))
		{
			T image{};
			image.AttachHandle(imageHandle);
			return image;
		}
		return {};
	}
}

namespace kxf
{
	DynamicLibrary DynamicLibrary::GetCurrentModule() noexcept
	{
		// Retrieve the HMODULE for the current DLL or EXE using this symbol that the linker provides for every module

		DynamicLibrary library;
		library.m_Handle = reinterpret_cast<void*>(&__ImageBase);
		library.m_ShouldUnload = false;

		return library;
	}
	DynamicLibrary DynamicLibrary::GetExecutingModule() noexcept
	{
		DynamicLibrary library;
		library.m_Handle = reinterpret_cast<void*>(::GetModuleHandleW(nullptr));
		library.m_ShouldUnload = false;

		return library;
	}
	DynamicLibrary DynamicLibrary::GetLoadedModule(const String& name) noexcept
	{
		HMODULE handle = nullptr;
		if (::GetModuleHandleExW(0, name.wc_str(), &handle))
		{
			DynamicLibrary library;
			library.m_Handle = reinterpret_cast<void*>(handle);
			library.m_ShouldUnload = false;

			return library;
		}
		return {};
	}

	DynamicLibrary::SearchDirectory* DynamicLibrary::AddSearchDirectory(const FSPath& path)
	{
		if (NativeAPI::Kernel32::AddDllDirectory)
		{
			String pathString = path.GetFullPathTryNS(FSPathNamespace::Win32File);
			return reinterpret_cast<SearchDirectory*>(NativeAPI::Kernel32::AddDllDirectory(pathString.wc_str()));
		}
		return nullptr;
	}
	bool DynamicLibrary::RemoveSearchDirectory(SearchDirectory& handle)
	{
		if (NativeAPI::Kernel32::RemoveDllDirectory)
		{
			return NativeAPI::Kernel32::RemoveDllDirectory(reinterpret_cast<DLL_DIRECTORY_COOKIE>(&handle));
		}
		return false;
	}

	// General
	FSPath DynamicLibrary::GetFilePath() const
	{
		if (m_Handle)
		{
			String result;
			if (::GetModuleFileNameW(AsHMODULE(*m_Handle), Utility::StringBuffer(result, INT16_MAX, true), INT16_MAX) != 0)
			{
				return result;
			}
		}
		return {};
	}

	bool DynamicLibrary::Load(const FSPath& path, FlagSet<DynamicLibraryFlag> flags)
	{
		Unload();

		if (path)
		{
			String pathString = path.IsAbsolute() ? path.GetFullPathTryNS(FSPathNamespace::Win32File) : path.GetFullPath();
			m_Handle = reinterpret_cast<void*>(::LoadLibraryExW(pathString.wc_str(), nullptr, *MapDynamicLibraryLoadFlags(flags)));
			m_LoadFlags = flags;
			m_ShouldUnload = true;

			return !IsNull();
		}
		return false;
	}
	void DynamicLibrary::Unload() noexcept
	{
		if (!IsNull() && m_ShouldUnload)
		{
			::FreeLibrary(AsHMODULE(*m_Handle));
		}
		m_Handle = {};
		m_LoadFlags = DynamicLibraryFlag::None;
		m_ShouldUnload = false;
	}

	// Functions
	size_t DynamicLibrary::EnumExportedFunctions(std::function<CallbackCommand(String, size_t, void*)> func) const
	{
		size_t count = 0;
		MapDLL(*m_Handle, [&](const char* base, const IMAGE_DOS_HEADER* dosHeader, const IMAGE_NT_HEADERS64* ntHeader, const IMAGE_SECTION_HEADER* sections, auto& adjustRVA)
		{
			const auto exportDirectory = ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
			if (exportDirectory.Size != 0)
			{
				auto exports = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(base + adjustRVA(exportDirectory.VirtualAddress));
				if (auto namesArray = reinterpret_cast<const uint32_t*>(reinterpret_cast<uintptr_t>(base) + adjustRVA(exports->AddressOfNames)))
				{
					auto nameOrdinalsArray = reinterpret_cast<const uint16_t*>(reinterpret_cast<uintptr_t>(base) + adjustRVA(exports->AddressOfNameOrdinals));
					auto functionsArray = reinterpret_cast<const uint32_t*>(reinterpret_cast<uintptr_t>(base) + adjustRVA(exports->AddressOfFunctions));

					for (size_t i = 0; i < static_cast<size_t>(exports->NumberOfNames); i++)
					{
						const auto name = reinterpret_cast<const char*>(base + adjustRVA(namesArray[i]));
						const auto ordinal = nameOrdinalsArray[i];
						const auto funcRVA = static_cast<intptr_t>(functionsArray[i]);

						if (name && *name)
						{
							count++;
							if (std::invoke(func, name, ordinal + 1, reinterpret_cast<void*>(funcRVA)) == CallbackCommand::Terminate)
							{
								break;
							}
						}
					}
				}
			}
		});
		return count;
	}

	void* DynamicLibrary::GetExportedFunctionAddress(const char* name) const
	{
		if (m_Handle)
		{
			return ::GetProcAddress(AsHMODULE(*m_Handle), name);
		}
		return nullptr;
	}
	void* DynamicLibrary::GetExportedFunctionAddress(size_t ordinal) const
	{
		if (m_Handle)
		{
			return ::GetProcAddress(AsHMODULE(*m_Handle), MAKEINTRESOURCEA(ordinal));
		}
		return nullptr;
	}

	bool DynamicLibrary::ContainsExportedFunction(const String& name)
	{
		if (IsResource())
		{
			bool found = false;
			EnumExportedFunctions([&found, &searchFor = name](const String& name, size_t ordinal, void* rva)
			{
				if (searchFor == name)
				{
					found = true;
					return CallbackCommand::Terminate;
				}
				return CallbackCommand::Continue;
			});
			return found;
		}
		else
		{
			return GetExportedFunctionAddress(name) != nullptr;
		}
	}
	bool DynamicLibrary::ContainsExportedFunction(size_t ordinal)
	{
		if (IsResource())
		{
			bool found = false;
			EnumExportedFunctions([&found, &searchFor = ordinal](const String& name, size_t ordinal, void* rva)
			{
				if (searchFor == ordinal)
				{
					found = true;
					return CallbackCommand::Terminate;
				}
				return CallbackCommand::Continue;
			});
			return found;
		}
		else
		{
			return GetExportedFunctionAddress(ordinal) != nullptr;
		}
	}

	// Dependencies
	size_t DynamicLibrary::EnumDependencyModuleNames(std::function<CallbackCommand(String)> func) const
	{
		if (m_Handle)
		{
			size_t count = 0;
			MapDLL(*m_Handle, [&](const char* base, const IMAGE_DOS_HEADER* dosHeader, const IMAGE_NT_HEADERS64* ntHeader, const IMAGE_SECTION_HEADER* sections, auto& adjustRVA)
			{
				auto importDirectory = ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
				if (importDirectory.Size != 0)
				{
					auto imports = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(base + adjustRVA(importDirectory.VirtualAddress));
					for (auto it = imports; it->Characteristics != 0; ++it)
					{
						const auto name = reinterpret_cast<const char*>(base + adjustRVA(it->Name));
						if (name && *name)
						{
							count++;
							if (std::invoke(func, name) == CallbackCommand::Terminate)
							{
								break;
							}
						}
					}
				}
			});
			return count;
		}
		return 0;
	}

	// Resources
	bool DynamicLibrary::IsResource() const noexcept
	{
		return m_Handle ? IsHandleDataFile(*m_Handle) : false;
	}
	bool DynamicLibrary::IsImageResource() const noexcept
	{
		return m_Handle ? IsHandleImageResourse(*m_Handle) : false;
	}
	bool DynamicLibrary::IsAnyResource() const noexcept
	{
		return IsResource() || IsImageResource();
	}

	bool DynamicLibrary::IsResourceExist(const String& resType, const String& resName, const Locale& locale) const
	{
		return m_Handle && GetResourceHandle(AsHMODULE(*m_Handle), resType, resName, locale) != nullptr;
	}
	size_t DynamicLibrary::EnumResourceTypes(std::function<CallbackCommand(String)> func, const Locale& locale) const
	{
		if (m_Handle)
		{
			CallContext callContext(func);
			::EnumResourceTypesExW(AsHMODULE(*m_Handle), [](HMODULE handle, LPWSTR resType, LONG_PTR lParam) -> BOOL
			{
				auto& context = *reinterpret_cast<decltype(callContext)*>(lParam);

				context.CallCount++;
				return std::invoke(context.Callable, resType) == CallbackCommand::Continue;
			}, reinterpret_cast<LONG_PTR>(&callContext), 0, GetLangID(locale));
			return callContext.CallCount;
		}
		return 0;
	}
	size_t DynamicLibrary::EnumResourceNames(const String& resType, std::function<CallbackCommand(String)> func, const Locale& locale) const
	{
		if (m_Handle)
		{
			CallContext callContext(func);
			::EnumResourceNamesExW(AsHMODULE(*m_Handle), GetNameOrID(resType), [](HMODULE handle, LPCWSTR resType, LPWSTR resName, LONG_PTR lParam) -> BOOL
			{
				auto& context = *reinterpret_cast<decltype(callContext)*>(lParam);

				context.CallCount++;
				return std::invoke(context.Callable, resName) == CallbackCommand::Continue;
			}, reinterpret_cast<LONG_PTR>(&callContext), 0, GetLangID(locale));
		}
		return 0;
	}
	size_t DynamicLibrary::EnumResourceLanguages(const String& resType, const String& resName, std::function<CallbackCommand(Locale)> func) const
	{
		if (m_Handle)
		{
			CallContext callContext(func);
			::EnumResourceLanguagesExW(AsHMODULE(*m_Handle), GetNameOrID(resType), GetNameOrID(resName), [](HMODULE handle, LPCWSTR resType, LPCWSTR resName, WORD langID, LONG_PTR lParam) -> BOOL
			{
				auto& context = *reinterpret_cast<decltype(callContext)*>(lParam);

				context.CallCount++;
				return std::invoke(context.Callable, Locale::FromLangID(Localization::LangID(langID, SORT_DEFAULT))) == CallbackCommand::Continue;
			}, reinterpret_cast<LONG_PTR>(&callContext), 0, 0);
			return callContext.CallCount;
		}
		return 0;
	}
	std::span<const std::byte> DynamicLibrary::GetResource(const String& resType, const String& resName, const Locale& locale) const
	{
		if (m_Handle)
		{
			return GetResourceData(AsHMODULE(*m_Handle), GetResourceHandle(AsHMODULE(*m_Handle), resType, resName, locale));
		}
		return {};
	}
	String DynamicLibrary::GetMessageResource(uint32_t messageID, const Locale& locale) const
	{
		if (m_Handle)
		{
			return System::Private::FormatMessage(*m_Handle, messageID, FORMAT_MESSAGE_FROM_HMODULE, locale);
		}
		return {};
	}

	size_t DynamicLibrary::GetIconResourceCount(const String& name, const Locale& locale) const
	{
		auto groupBuffer = GetResource(System::Private::ResourceTypeToName(RT_GROUP_ICON), name, locale);
		if (!groupBuffer.empty())
		{
			const auto* iconGroup =  reinterpret_cast<const System::Private::IconGroupDirectory*>(groupBuffer.data());
			return iconGroup->idCount;
		}
		return 0;
	}
	GDIIcon DynamicLibrary::GetIconResource(const String& name, const Size& size, const Locale& locale) const
	{
		return LoadGDIImage<GDIIcon>(AsHMODULE(*m_Handle), name, System::Private::ResourceTypeToName(RT_GROUP_ICON), IMAGE_ICON, size, locale);
	}
	GDIIcon DynamicLibrary::GetIconResource(const String& name, size_t index, const Locale& locale) const
	{
		using System::Private::ResourceTypeToName;
		using System::Private::IconGroupDirectory;
		using System::Private::IconGroupEntry;

		auto groupBuffer = GetResource(ResourceTypeToName(RT_GROUP_ICON), name, locale);
		if (!groupBuffer.empty())
		{
			const IconGroupDirectory* iconGroup = reinterpret_cast<const IconGroupDirectory*>(groupBuffer.data());

			const size_t iconCount = iconGroup->idCount;
			if (index >= iconCount)
			{
				index = iconCount - 1;
			}

			const IconGroupEntry* iconInfo = &iconGroup->idEntries[index];
			const size_t imageID = iconInfo->id;

			auto iconBuffer = GetResourceData(AsHMODULE(*m_Handle), GetResourceHandle(AsHMODULE(*m_Handle), ResourceTypeToName(RT_ICON), ResourceTypeToName(imageID), locale));
			if (!iconBuffer.empty())
			{
				int width = 0;
				int height = 0;

				// I don't remember what '0x00030000' is. WHY DIDN'T I ADD A LINK TO WHERE I FOUND THIS?!
				constexpr DWORD dwVer = 0x00030000u;
				auto iconBufferData = const_cast<BYTE*>(reinterpret_cast<const BYTE*>(iconBuffer.data()));
				if (HICON iconHandle = ::CreateIconFromResourceEx(iconBufferData, iconBuffer.size_bytes(), TRUE, dwVer, width, height, LR_DEFAULTCOLOR))
				{
					GDIIcon icon;
					icon.AttachHandle(iconHandle);
					return icon;
				}
			}
		}
		return {};
	}
	ImageBundle DynamicLibrary::GetIconBundleResource(const String& name, const Locale& locale) const
	{
		const size_t count = GetIconResourceCount(name, locale);

		ImageBundle bundle(count);
		for (size_t i = 0; i < count; i++)
		{
			bundle.AddImage(GetIconResource(name, i, locale).ToBitmapImage());
		}
		return bundle;
	}

	GDIBitmap DynamicLibrary::GetBitmapResource(const String& name, const Locale& locale) const
	{
		return LoadGDIImage<GDIBitmap>(AsHMODULE(*m_Handle), name, System::Private::ResourceTypeToName(RT_BITMAP), IMAGE_BITMAP, g_DefaultIconSize, locale);
	}
	GDICursor DynamicLibrary::GetCursorResource(const String& name, const Locale& locale) const
	{
		return LoadGDIImage<GDICursor>(AsHMODULE(*m_Handle), name, System::Private::ResourceTypeToName(RT_CURSOR), IMAGE_CURSOR, g_DefaultIconSize, locale);
	}
	String DynamicLibrary::GetStringResource(const String& name, const Locale& locale) const
	{
		if (m_Handle)
		{
			// LoadString can not load strings with non-default locale
			if (locale && !locale.IsInvariant())
			{
				// http://forum.sources.ru/index.php?showtopic=375357
				if (auto stringID = name.ToInteger<size_t>())
				{
					auto data = GetResource(System::Private::ResourceTypeToName(RT_STRING), System::Private::ResourceTypeToName(*stringID / 16 + 1), locale);
					if (!data.empty())
					{
						stringID = *stringID % 16;
						const size_t tableSize = data.size_bytes() / sizeof(wchar_t);
						size_t offset = 0;
						size_t index = 0;

						const wchar_t* stringData = reinterpret_cast<const wchar_t*>(data.data());
						while (offset < tableSize)
						{
							if (index == *stringID)
							{
								size_t stringLength = stringData[offset];
								if (stringLength > 0)
								{
									return String(&(stringData[offset + 1]), stringLength);
								}
								break;
							}

							offset += stringData[offset] + 1;
							index++;
						}
					}
				}
			}
			else
			{
				if (auto id = name.ToInteger<int>())
				{
					LPWSTR string = nullptr;
					int length = LoadStringW(AsHMODULE(*m_Handle), *id, reinterpret_cast<LPWSTR>(&string), 0);
					if (length > 0)
					{
						return String(string, length);
					}
				}
			}
		}
		return {};
	}
}
