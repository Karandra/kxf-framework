#pragma once
#include "kxf-gui/Widgets/Common.h"
#include "kxf-gui/Drawing/WithImageList.h"
#include "StatusBar.h"
#include "IProgressMeter.h"
#include <wx/renderer.h>
#include <wx/toplevel.h>

namespace kxf::UI
{
	class KXF_API StatusBarEx: public StatusBar, public WithImageList, public IProgressMeter
	{
		public:
			static constexpr FlagSet<StatusBarStyle> DefaultStyle = StatusBar::DefaultStyle;

		private:
			std::unordered_map<int, int> m_Images;
			FlagSet<StatusBarStyle> m_Style = DefaultStyle;
			Color m_BorderColor;
			int m_State = wxCONTROL_NONE;
			bool m_IsSeparatorsVisible = false;

			int m_ProgressPos = 0;
			int m_ProgressRange = 100;
			int m_ProgressStep = 10;
	
		private:
			void OnPaint(wxPaintEvent& event);
			void OnSize(wxSizeEvent& event);
			void OnMouseDown(wxMouseEvent& event);
			void OnEnter(wxMouseEvent& event);
			void OnLeave(wxMouseEvent& event);
		
			wxEllipsizeMode GetEllipsizeMode() const;
			wxTopLevelWindow* GetTLWParent() const;
			void MakeTopmost();

		protected:
			// IProgressBar
			int DoGetRange() const override
			{
				return m_ProgressRange;
			}
			void DoSetRange(int range) override
			{
				m_ProgressRange = range;
				ScheduleRefresh();
			}

			int DoGetValue() const override
			{
				return m_ProgressPos;
			}
			void DoSetValue(int value) override
			{
				m_ProgressPos = value;
				ScheduleRefresh();
			}

			int DoGetStep() const override
			{
				return m_ProgressStep;
			}
			void DoSetStep(int step) override
			{
				m_ProgressStep = step <= m_ProgressRange ? step : m_ProgressRange;
			}

			void DoPulse() override
			{
				ScheduleRefresh();
			}
			bool DoIsPulsing() const override
			{
				return false;
			}

		public:
			StatusBarEx() = default;
			StatusBarEx(wxWindow* parent,
						wxWindowID id,
						int fieldsCount = 1,
						FlagSet<StatusBarStyle> style = DefaultStyle
			)
			{
				Create(parent, id, fieldsCount, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						int fieldsCount = 1,
						FlagSet<StatusBarStyle> style = DefaultStyle
			);

		public:
			Color GetBorderColor() const
			{
				return m_BorderColor;
			}
			void SetBorderColor(const Color& color)
			{
				m_BorderColor = color;
				ScheduleRefresh();
			}
		
			bool SetForegroundColour(const wxColour& color) override;
			bool SetBackgroundColour(const wxColour& color) override;
		
			bool IsSeparatorsVisible() const
			{
				return m_IsSeparatorsVisible;
			}
			void SetSeparatorsVisible(bool visible = true)
			{
				m_IsSeparatorsVisible = visible;
				ScheduleRefresh();
			}
		
			int GetStatusImage(int index)
			{
				if (index < GetFieldsCount())
				{
					return m_Images[index];
				}
				return Drawing::InvalidImageIndex;
			}
			void SetStatusImage(int imageIndex, int index)
			{
				if (index < GetFieldsCount())
				{
					m_Images[index] = imageIndex;
					ScheduleRefresh();
				}
			}
		
			void SetMinHeight(int height) override;
			void SetStatusText(const String& text, int index = 0)
			{
				StatusBar::SetStatusText(text, index);
				ScheduleRefresh();
			}

			void SetFieldsCount(int count) override
			{
				StatusBar::SetFieldsCount(count);
				ScheduleRefresh();
			}
			void SetFieldsCount(const std::vector<int>& widths) override
			{
				StatusBar::SetFieldsCount(widths);
				ScheduleRefresh();
			}

			void UseDefaultColors();
			void UseBackgroundColors();

		public:
			wxDECLARE_DYNAMIC_CLASS(StatusBarEx);
	};
}
