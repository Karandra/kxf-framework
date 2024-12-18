#pragma once
#include "../Common.h"
#include "kxf/Core/String.h"
#include "kxf/Localization/Locale.h"

namespace kxf::System::Private
{
	String FormatMessage(const void* source, uint32_t messageID, FlagSet<uint32_t> flags = {}, const Locale& locale = {}) noexcept;
	size_t EnumWindows(std::function<CallbackCommand(void*, uint32_t, uint32_t)> func, std::optional<uint32_t> pid = {}, std::optional<uint32_t> tid = {});
	size_t EnumThreads(std::function<CallbackCommand(uint32_t, uint32_t)> func, std::optional<uint32_t> pid = {}, std::optional<uint32_t> tid = {});

	String ResourceTypeToName(size_t id);
	String ResourceTypeToName(const wchar_t* id);
	const wchar_t* MakeIntResource(int resID);

	FlagSet<uint32_t> MapSystemStandardAccess(FlagSet<SystemStandardAccess> access) noexcept;
	FlagSet<uint32_t> MapSystemThreadAccess(FlagSet<SystemThreadAccess> access) noexcept;
	FlagSet<uint32_t> MapSystemProcessAccess(FlagSet<SystemProcessAccess> access) noexcept;

	std::optional<uint32_t> MapSystemProcessPriority(SystemProcessPriority priority) noexcept;
	SystemProcessPriority MapSystemProcessPriority(uint32_t priority) noexcept;

	String GetKernelObjectName(const String& name, KernelObjectNamespace ns);
}
