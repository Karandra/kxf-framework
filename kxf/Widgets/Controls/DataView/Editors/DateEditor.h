#pragma once
#include "../Editor.h"
#include "kxf/Core/WithOptions.h"

namespace kxf::UI::DataView
{
	enum class DateEditorOption: uint32_t
	{
		None = 0,

		Spin = 1 << 0,
		Dropdown = 1 << 2,
		AllowNone = 1 << 3,
		ShowCentury = 1 << 4,
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::DataView::DateEditorOption);
}

namespace kxf::UI::DataView
{
	class KXF_API DateTimeValue: public WithOptions<DateEditorOption, DateEditorOption::None>
	{
		protected:
			DateTime m_Value;
			DateTime m_RangeLower;
			DateTime m_RangeUpper;

		public:
			DateTimeValue(const DateTime& value = {})
				:m_Value(value)
			{
			}
			DateTimeValue(const DateTime& value, const DateTime& lower, const DateTime& upper)
				:m_Value(value)
			{
				SetDateTimeRange(lower, upper);
			}

		public:
			bool FromAny(Any value);
			void Clear()
			{
				*this = {};
			}

			bool HasDateTime() const
			{
				return m_Value.IsValid();
			}
			DateTime GetDateTime() const
			{
				return m_Value;
			}
			void SetDateTime(const DateTime& value)
			{
				m_Value = value;
			}
			void ClearDateTime()
			{
				m_Value = {};
			}

			bool HasDateRange() const
			{
				return m_RangeLower.IsValid() && m_RangeUpper.IsValid();
			}
			std::pair<DateTime, DateTime> GetDateTimeRange() const
			{
				return {m_RangeLower, m_RangeUpper};
			}
			void SetDateTimeRange(const DateTime& lower, const DateTime& upper)
			{
				m_RangeLower = lower;
				m_RangeUpper = upper;

				if (m_RangeLower > m_RangeUpper)
				{
					std::swap(m_RangeLower, m_RangeUpper);
				}
			}
			void ClearDateTimeRange()
			{
				m_RangeLower = {};
				m_RangeUpper = {};
			}
	};
}

namespace kxf::UI::DataView
{
	class KXF_API DateEditor: public Editor
	{
		protected:
			wxWindow* CreateControl(wxWindow& parent, const Rect& cellRect, Any value) override;
			Any GetValue(wxWindow& control) const override;
	};
}
