#pragma once
#include "Common.h"
#include "kxf-gui/Drawing/IRendererNative.h"

namespace kxf
{
	class IWidget;
}

namespace kxf::DataView
{
	class KXF_API CellState final
	{
		private:
			enum class Value: uint32_t
			{
				None = 0,
				Current = FlagSetValue<Value>(0),
				Selected = FlagSetValue<Value>(1),
				HotTracked = FlagSetValue<Value>(2),
				DropTarget = FlagSetValue<Value>(3)
			};

		private:
			FlagSet<Value> m_Value;

		public:
			bool IsCurrent() const noexcept
			{
				return m_Value.Contains(Value::Current);
			}
			bool IsSelected() const noexcept
			{
				return m_Value.Contains(Value::Selected);
			}
			bool IsHotTracked() const noexcept
			{
				return m_Value.Contains(Value::HotTracked);
			}
			bool IsDropTarget() const noexcept
			{
				return m_Value.Contains(Value::DropTarget);
			}

			CellState& SetCurrent() noexcept
			{
				m_Value.Add(Value::Current);
				return *this;
			}
			CellState& SetItemSelected() noexcept
			{
				m_Value.Add(Value::Selected);
				return *this;
			}
			CellState& SetHotTracked() noexcept
			{
				m_Value.Add(Value::HotTracked);
				return *this;
			}
			CellState& SetDropTarget() noexcept
			{
				m_Value.Add(Value::DropTarget);
				return *this;
			}

		public:
			FlagSet<NativeWidgetFlag> ToNativeWidgetFlags(const IWidget& widget) const noexcept;
	};
}
