#pragma once
#include "../Common.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/Drawing/Geometry.h"
#include "kxf/DateTime/TimeSpan.h"

namespace kxf
{
	class SystemThread;
	class SystemProcess;
}

namespace kxf
{
	class KXF_API ISystemWindow: public RTTI::Interface<ISystemWindow>
	{
		kxf_RTTI_DeclareIID(ISystemWindow, {0xc1663541, 0xe5e3, 0x4610, {0x8a, 0x66, 0xab, 0xdc, 0x16, 0x82, 0x58, 0xb7}});

		public:
			virtual ~ISystemWindow() = default;

		public:
			virtual bool IsNull() const = 0;
			virtual uint32_t GetID() const = 0;
			virtual void* GetHandle() const = 0;
			virtual SystemThread GetOwningThread() const = 0;
			virtual SystemProcess GetOwningProcess() const = 0;

			virtual String GetText() const = 0;
			virtual bool SetText(const String& label) = 0;

			virtual Rect GetRect() const = 0;
			virtual Point ScreenToClient(const Point& point) const = 0;
			virtual Point ClientToScreen(const Point& point) const = 0;
			virtual Size MapPoints(Rect& rect, const ISystemWindow& targetWindow) const = 0;

			virtual std::optional<intptr_t> GetValue(int index) const = 0;
			virtual std::optional<intptr_t> SetValue(int index, intptr_t value) = 0;
			virtual bool PostMessage(uint32_t message, intptr_t wParam, intptr_t lParam) = 0;
			virtual std::optional<intptr_t> SendMessage(uint32_t message, intptr_t wParam, intptr_t lParam, FlagSet<uint32_t> flags = {}, TimeSpan timeout = {}) = 0;

			virtual bool IsVisible() const = 0;
			virtual bool Show(SHWindowCommand command, bool async = false) = 0;
			virtual bool Close() = 0;
			virtual bool Destroy() = 0;

			virtual bool IsEnabled() const = 0;
			virtual bool SetEnabled(bool enable = true) = 0;
	};
}
