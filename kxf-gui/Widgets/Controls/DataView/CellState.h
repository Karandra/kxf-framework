#pragma once
#include "Common.h"
#include "kxf-gui/Drawing/IRendererNative.h"

namespace kxf::UI::DataView
{
	class MainWindow;
}

namespace kxf::UI::DataView
{
	class KXF_API CellState final
	{
		private:
			enum class Value: uint32_t
			{
				None = 0,
				Current = 1 << 0,
				Selected = 1 << 1,
				HotTracked = 1 << 2,
				DropTarget = 1 << 3,
			};

		private:
			FlagSet<Value> m_Value;

		public:
			bool IsCurrent() const
			{
				return m_Value.Contains(Value::Current);
			}
			bool IsSelected() const
			{
				return m_Value.Contains(Value::Selected);
			}
			bool IsHotTracked() const
			{
				return m_Value.Contains(Value::HotTracked);
			}
			bool IsDropTarget() const
			{
				return m_Value.Contains(Value::DropTarget);
			}

			CellState& SetCurrent()
			{
				m_Value.Add(Value::Current);
				return *this;
			}
			CellState& SetItemSelected()
			{
				m_Value.Add(Value::Selected);
				return *this;
			}
			CellState& SetHotTracked()
			{
				m_Value.Add(Value::HotTracked);
				return *this;
			}
			CellState& SetDropTarget()
			{
				m_Value.Add(Value::DropTarget);
				return *this;
			}

		public:
			FlagSet<NativeWidgetFlag> ToNativeWidgetFlags(const MainWindow& window) const;
	};
}
