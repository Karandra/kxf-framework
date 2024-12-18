#pragma once
#include "Common.h"
#include "kxf/System/HResult.h"

namespace kxf
{
	enum class NativeWidgetProperty
	{
		None = -1,

		ID,
		Style,
		ExStyle,
		Instance,
		UserData,
		WindowProcedure,

		DialogResult,
		DialogUserData,
		DialogProcedure
	};
}

namespace kxf
{
	class KXF_API INativeWidget: public RTTI::Interface<INativeWidget>
	{
		kxf_RTTI_DeclareIID(INativeWidget, {0xffedb80, 0x1a2e, 0x48e7, {0xbe, 0x29, 0x98, 0x16, 0x65, 0x5b, 0x32, 0xb}});

		public:
			void OnMessageProcessed(void* handle, uint32_t messageID, intptr_t result);

		public:
			virtual ~INativeWidget() = default;

		public:
			virtual void* GetNativeHandle() const = 0;
			virtual String GetIntrinsicText() const = 0;

			virtual intptr_t GetWindowProperty(NativeWidgetProperty index) const = 0;
			virtual intptr_t SetWindowProperty(NativeWidgetProperty index, intptr_t value) = 0;

			virtual bool PostMessage(uint32_t messageID, intptr_t wParam = 0, intptr_t lParam = 0) = 0;
			virtual bool NotifyMessage(uint32_t messageID, intptr_t wParam = 0, intptr_t lParam = 0) = 0;
			virtual bool SendMessageSignal(uint32_t messageID, intptr_t wParam = 0, intptr_t lParam = 0) = 0;
			virtual intptr_t SendMessage(uint32_t messageID, intptr_t wParam = 0, intptr_t lParam = 0, TimeSpan timeout = {}) = 0;

			virtual HResult SetWindowTheme(const String& applicationName, const std::vector<String>& subIDs = {}) = 0;
			virtual bool EnableNonClientDPIScaling() = 0;

			virtual bool IsForegroundWindow() const = 0;
			virtual bool SetForegroundWindow() = 0;
	};
}

namespace kxf::Widgets
{
	void* GetNativeHandleOf(const IWidget& widget) noexcept;
	void* GetShellNativeHandle() noexcept;
	void* GetAnyTopLevelNativeHandle(const IWidget* widget = nullptr) noexcept;
}
