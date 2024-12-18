#pragma once
#include "kxf/UI/Common.h"
#include "kxf/UI/Controls/IProgressMeter.h"
#include "kxf/System/HResult.h"
#include "kxf/Utility/Common.h"
#include "StdDialog.h"

namespace kxf::UI
{
	namespace Private
	{
		class TaskDialogNativeInfo;
	}

	enum class TaskDialogStyle: uint32_t
	{
		None = 0,

		Hyperlinks = 1 << 0,
		CommandLinks = 1 << 1,
		CommandLinksNoIcon = 1 << 2,
		ExMessageInFooter = 1 << 3,
		ExMessageExpanded = 1 << 4,
		ProgressBar = 1 << 5,
		ProgressBarPulse = 1 << 6,
		CallbackTimer = 1 << 7,
		CheckBox = 1 << 8,
		CheckBoxChecked = 1 << 9,
		SizeToContent = 1 << 10
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::TaskDialogStyle);
	kxf_FlagSet_Extend(UI::TaskDialogStyle, UI::DialogStyle);
}

namespace kxf::UI
{
	class KXF_API TaskDialog: public Dialog, public RTTI::Implementation<TaskDialog, IStdDialog, IProgressMeter>
	{
		friend class Private::TaskDialogNativeInfo;

		public:
			static constexpr FlagSet<TaskDialogStyle> DefaultStyle = TaskDialogStyle::None;
			static constexpr FlagSet<StdButton> DefaultButtons = StdButton::OK;

		private:
			struct ButtonItem final
			{
				String Label;
				wxWindowID ID = wxID_NONE;
			};

		private:
			std::unique_ptr<Private::TaskDialogNativeInfo> m_NativeInfo;
			FlagSet<TaskDialogStyle> m_Style;
			wxWindow* m_Parent = nullptr;
			void* m_Handle = nullptr;

			GDIIcon m_MainIcon;
			GDIIcon m_FooterIcon;
			StdIcon m_MainIconID = StdIcon::Information;
			StdIcon m_FooterIconID = StdIcon::None;

			String m_Title;
			String m_Caption;
			String m_ExMessage;
			String m_Message;
			String m_ButtonExpand;
			String m_ButtonCollapse;
			String m_FooterMessage;
			String m_CheckBoxLabel;
			FlagSet<StdButton> m_StandardButtons = DefaultButtons;
			wxWindowID m_DefaultButton = wxID_NONE;
			wxWindowID m_DefaultRadioButton = wxID_NONE;
			wxWindowID m_SelectedRadioButton = wxID_NONE;
			bool m_CheckBoxValue = false;

			std::vector<ButtonItem> m_Buttons;
			std::vector<ButtonItem> m_RadioButtons;

			// Progress meter
			int m_ProgressPos = 0;
			int m_ProgressStep = 10;
			int m_ProgressRange = 100;
			TimeSpan m_ProgressPulseInterval;

		private:
			HResult OnDialogEvent(void* windowHandle, uint32_t notification, void* wParam, void* lParam);
			wxWindowID DoShowDialog(bool isModal = true);
			void OnClose(wxCloseEvent& event);

		protected:
			int DoGetRange() const override
			{
				return m_ProgressRange;
			}
			void DoSetRange(int range) override;

			int DoGetValue() const override
			{
				return m_ProgressPos;
			}
			void DoSetValue(int value) override;

			int DoGetStep() const override
			{
				return m_ProgressStep;
			}
			void DoSetStep(int step) override
			{
				m_ProgressStep = step;
			}

			bool DoIsPulsing() const override
			{
				return m_Style & TaskDialogStyle::ProgressBarPulse;
			}
			void DoPulse() override;

		public:
			TaskDialog();
			TaskDialog(wxWindow* parent,
					   wxWindowID id,
					   String caption = {},
					   String message = {},
					   FlagSet<StdButton> buttons = DefaultButtons,
					   StdIcon mainIcon = StdIcon::None,
					   FlagSet<TaskDialogStyle> style = DefaultStyle
			);
			bool Create(wxWindow* parent,
						wxWindowID id,
						String caption = {},
						String message = {},
						FlagSet<StdButton> buttons = DefaultButtons,
						StdIcon mainIcon = StdIcon::None,
						FlagSet<TaskDialogStyle> style = DefaultStyle
			);
			~TaskDialog();

		public:
			WXWidget GetHandle() const override
			{
				return reinterpret_cast<WXWidget>(m_Handle);
			}
			bool Show(bool show = true) override;
			int ShowModal() override;
			
			bool Realize();
			void UpdateWindowUI(long flags = wxUPDATE_UI_NONE) override;

			// Icons
			StdIcon GetMainIconID() const override;
			GDIBitmap GetMainIcon() const override;
			void SetMainIcon(StdIcon iconID) override;
			void SetMainIcon(const GDIBitmap& icon) override;
			
			StdIcon GetFooterIconID();
			GDIBitmap GetFooterIcon();
			void SetFooterIcon(StdIcon iconID);
			void SetFooterIcon(const GDIBitmap& icon);

			// Strings
			wxString GetTitle() const override
			{
				return m_Title;
			}
			void SetTitle(const wxString& string = {}) override;
			void SetDefaultTitle();

			wxString GetCaption() const override
			{
				return m_Caption;
			}
			void SetCaption(const wxString& string) override;

			wxString GetLabel() const override
			{
				return wxTopLevelWindow::GetLabel();
			}
			void SetLabel(const wxString& string) override
			{
				wxTopLevelWindow::SetLabel(string);
			}

			String GetMessage() const
			{
				return m_Message;
			}
			void SetMessage(String string);
			
			String GetExMessage() const
			{
				return m_ExMessage;
			}
			void SetExMessage(String string);
			
			String GetFooter() const
			{
				return m_FooterMessage;
			}
			void SetFooter(String string);
			
			String GetCheckBoxLabel() const
			{
				return m_CheckBoxLabel;
			}
			void SetCheckBoxLabel(String text);

			// Buttons
			void SetStandardButtons(FlagSet<StdButton> buttons)
			{
				m_StandardButtons = buttons;
			}
			void SetDefaultButton(WidgetID id) override
			{
				m_DefaultButton = id.ToInt();
			}
			void SetDefaultRadioButton(WidgetID id)
			{
				m_DefaultRadioButton = id.ToInt();
			}

			StdDialogControl GetButton(WidgetID id) const override;
			StdDialogControl AddButton(WidgetID id, const String& label = {}, bool prepend = false) override;
			StdDialogControl AddRadioButton(WidgetID id, const String& label = {});

			WidgetID ShowModalDialog() override
			{
				return ShowModal();
			}

			bool GetCheckBoxValue() const
			{
				return m_CheckBoxValue;
			}
			wxWindowID GetSelectedButton() const
			{
				return GetReturnCode();
			}
			wxWindowID GetSelectedRadioButton() const
			{
				return m_SelectedRadioButton;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(TaskDialog);
	};
}
