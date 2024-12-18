#pragma once
#include "kxf/UI/Common.h"
#include "Dialog.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/UI/Controls/Label.h"
#include "kxf/UI/Controls/Button.h"
#include "kxf/UI/Windows/Panel.h"
#include "kxf/UI/StdIcon.h"
#include "kxf/UI/StdButton.h"
#include <wx/nativewin.h>
#include <wx/artprov.h>
#include <wx/statbmp.h>
#include <wx/app.h>

namespace kxf::UI
{
	class KXF_API StdDialogControl final
	{
		public:
			enum class Type
			{
				ID,
				Window,
				Button,
				RadioButton,
			};

		private:
			union
			{
				wxWindowID ID = wxID_NONE;
				wxWindow* Control;
			} m_Data;
			Type m_Type = Type::ID;

		public:
			StdDialogControl(WidgetID id = {})
				:m_Type(Type::ID)
			{
				m_Data.ID = id.ToInt();
			}
			StdDialogControl(wxWindowID id)
				:m_Type(Type::ID)
			{
				m_Data.ID = id;
			}
			StdDialogControl(wxWindow* control)
				:m_Type(Type::Window)
			{
				m_Data.Control = control;
			}
			StdDialogControl(Button* control);

		public:
			bool IsID() const
			{
				return m_Type == Type::ID;
			}
			bool IsControl() const
			{
				return !IsID();
			}
			Type GetType() const
			{
				return m_Type;
			}
			wxWindow* GetControl() const
			{
				if (IsControl())
				{
					return m_Data.Control;
				}
				return nullptr;
			}
			template<class T> T* As() const
			{
				if (IsControl())
				{
					return static_cast<T*>(m_Data.Control);
				}
				return nullptr;
			}

			operator wxWindowID() const;
			operator wxWindow*() const;
			operator Button*() const;
	};
}

namespace kxf::UI
{
	class KXF_API IStdDialog: public RTTI::Interface<IStdDialog>
	{
		kxf_RTTI_DeclareIID(IStdDialog, {0x432d41e4, 0x2a7d, 0x44a0, {0x8f, 0x26, 0x49, 0x64, 0xdd, 0x31, 0x7a, 0x9f}});

		public:
			static constexpr StdIcon DefaultIconID = StdIcon::Information;
			static constexpr FlagSet<StdButton> DefaultButtons = StdButton::OK|StdButton::Cancel;

			kxf_EVENT_MEMBER(wxNotifyEvent, Button);
			kxf_EVENT_MEMBER(wxNotifyEvent, Navigate);
			kxf_EVENT_MEMBER(wxNotifyEvent, Navigating);
			kxf_EVENT_MEMBER(wxNotifyEvent, Overwrite);
			kxf_EVENT_MEMBER(wxNotifyEvent, Select);
			kxf_EVENT_MEMBER(wxNotifyEvent, TypeChanged);

		protected:
			bool ShowNativeWindow(wxDialog* window, bool show); // Will show/hide window if GetHandle() != nullptr and call ShowModal() otherwise
			int TranslateIconIDToWx(StdIcon id) const;
			StdIcon TranslateIconIDFromWx(int id) const;

		public:
			virtual wxString GetCaption() const = 0;
			virtual void SetCaption(const wxString& label) = 0;

			virtual wxString GetLabel() const = 0;
			virtual void SetLabel(const wxString& label) = 0;

			virtual GDIBitmap GetMainIcon() const = 0;
			virtual StdIcon GetMainIconID() const = 0;
			virtual void SetMainIcon(const GDIBitmap& icon) = 0;
			virtual void SetMainIcon(StdIcon iconID = DefaultIconID) = 0;

			virtual void SetDefaultButton(WidgetID id) = 0;
			virtual StdDialogControl GetButton(WidgetID id) const = 0;
			virtual StdDialogControl AddButton(WidgetID id, const String& label = {}, bool prepend = false) = 0;

			virtual WidgetID ShowModalDialog() = 0;
	};
}

namespace kxf::UI
{
	// This is probably the most broken class of them all...
	class KXF_API StdDialog: public Dialog, public IStdDialog
	{
		public:
			using StdButtonsIDs = std::vector<WidgetID>;

			static constexpr FlagSet<DialogStyle> DefaultStyle = Dialog::DefaultStyle;
			static const StdButtonsIDs ms_DefaultCloseIDs;
			static const StdButtonsIDs ms_DefaultEnterIDs;
			static const Color ms_WindowBackgroundColor;
			static const Color ms_LineBackgroundColor;

		private:
			static inline bool ms_IsBellAllowed = false;

		public:
			static bool IsBellAllowed()
			{
				return ms_IsBellAllowed;
			}
			static void SetAllowBell(bool allow)
			{
				ms_IsBellAllowed = allow;
			}

		protected:
			wxOrientation m_DialogResizeSide = wxBOTH;
			Label* m_CaptionLabel = nullptr;
			Label* m_ViewLabel = nullptr;
			Panel* m_ContentPanel = nullptr;
			wxStaticBitmap* m_IconView = nullptr;
			GDIBitmap m_MainIcon = wxNullBitmap;
			StdIcon m_MainIconID = DefaultIconID;
			std::vector<wxWindow*> m_UserControls;
			bool m_IsAutoSizeEnabled = true;

		private:
			Panel* m_ContentPanelLine = nullptr;
			wxSizer* m_DialogSizer = nullptr;
			wxSizer* m_ContentSizerBase = nullptr;
			wxSizer* m_ContentSizer = nullptr;
			wxSizer* m_IconSizer = nullptr;
			StdDialogButtonSizer* m_ButtonsSizer = nullptr;
			bool m_IsGlassFrameEnabled = false;
			wxNativeWindow m_GripperWindow;
			Size m_GripperWindowSize;

			wxSizerItem* m_ViewLabelSpacerSI = nullptr;
			wxSizerItem* m_ViewLabelSI = nullptr;

			StdButtonsIDs m_CloseIDs = ms_DefaultCloseIDs;
			StdButtonsIDs m_EnterIDs = ms_DefaultEnterIDs;
			FlagSet<StdButton> m_SelectedButtons = StdButton::None;

		protected:
			void PostCreate(const Point& pos = Point::UnspecifiedPosition());
			virtual int GetViewSizerProportion() const
			{
				return 1;
			}
			virtual wxOrientation GetViewSizerOrientation() const;
			virtual wxOrientation GetViewLabelSizerOrientation() const;
			virtual bool IsCloseBoxEnabled() const
			{
				return IsEscapeAllowed();
			}
			virtual bool IsEscapeAllowed(WidgetID* idOut = nullptr) const;
			virtual bool IsEnterAllowed(wxKeyEvent& event, WidgetID* idOut = nullptr) const;
			void SetResizingBehavior();
			static wxWindowID TranslateButtonConstantsToIDs(int btnValue);
			virtual Rect GetGlassRect() const
			{
				return Rect(0, 0, 0, m_ButtonsSizer->GetSize().GetHeight() + 1);
			}

		private:
			void OnStdButtonClick(wxCommandEvent& event);
			void OnClose(wxCloseEvent& event);
			void OnEscape(wxKeyEvent& event);
			void InitIcon()
			{
				if (!m_MainIcon && m_MainIconID != StdIcon::None)
				{
					SetMainIcon();
					LoadIcon();
				}
				else
				{
					SetIconVisibility();
				}
			}
			void LoadIcon()
			{
				m_IconView->SetBitmap(m_MainIcon.ToWxBitmap());
			}
			void SetIconVisibility()
			{
				if (m_MainIconID == StdIcon::None || !m_MainIcon)
				{
					m_ContentSizerBase->Hide(m_IconSizer);
				}
				else
				{
					m_ContentSizerBase->Show(m_IconSizer);
				}
			}
			void SetAffirmativeID(int id)
			{
				Dialog::SetAffirmativeId(id);
			}
			void SetEscapeID(int id)
			{
				Dialog::SetEscapeId(id);
			}
			String GetDefaultTitle()
			{
				return wxTheApp->GetAppDisplayName();
			}
			bool IsUserWindowHasFocus() const
			{
				for (const auto& window: m_UserControls)
				{
					if (window->HasFocus())
					{
						return true;
					}
				}
				return false;
			}

			bool MSWTranslateMessage(WXMSG* msg) override;
			void EnableGlassFrame();
			void* GetGripperWindow();
			void OnDrawFrameBorder(wxPaintEvent& event);

		public:
			StdDialog() = default;
			StdDialog(wxWindow* parent,
					  wxWindowID id,
					  const String& caption,
					  const Point& pos = Point::UnspecifiedPosition(),
					  const Size& size = Size::UnspecifiedSize(),
					  FlagSet<StdButton> buttons = DefaultButtons,
					  FlagSet<DialogStyle> style = DefaultStyle
			)
			{
				Create(parent, id, caption, pos, size, buttons, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& caption,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize(),
						FlagSet<StdButton> buttons = DefaultButtons,
						FlagSet<DialogStyle> style = DefaultStyle
			);

		public:
			virtual void SetAffirmativeId(int id)
			{

			}
			virtual void SetEscapeId(int id)
			{

			}

			wxWindow* GetContentWindow() const override
			{
				return m_ContentPanel;
			}
			virtual wxSizer* GetContentWindowSizer()
			{
				return GetDialogMainCtrl()->GetContainingSizer();
			}
			wxSizer* GetContentWindowMainSizer()
			{
				return m_ContentSizer;
			}
			virtual wxWindow* GetDialogMainCtrl() const = 0;
			virtual wxWindow* GetDialogFocusCtrl() const
			{
				return GetDialogMainCtrl();
			}

			virtual int ShowModal() override;
			void AdjustWindow(const Point &pos = Point(-2, -2), const Size& minSize = Size::UnspecifiedSize());
			void AddUserWindow(wxWindow* window)
			{
				m_UserControls.push_back(window);
			}

			// Layout
			virtual wxOrientation GetWindowResizeSide() const
			{
				return m_DialogResizeSide;
			}
			void SetWindowResizeSide(wxOrientation value)
			{
				m_DialogResizeSide = value;
				SetResizingBehavior();
			}
			bool IsAutoSizeEnabled() const
			{
				return m_IsAutoSizeEnabled;
			}
			void SetAutoSize(bool value)
			{
				m_IsAutoSizeEnabled = value;
			}

			// Misc UI functions
			void SetGlassFrameEnabled(bool value)
			{
				m_IsGlassFrameEnabled = value;
				EnableGlassFrame();
			}
			void SetGripperVisible(bool value)
			{
				if (value)
				{
					m_GripperWindow.SetSize(m_GripperWindowSize);
					m_GripperWindow.Show();

					Size size = Size(GetSize());
					SetSize(size + Size(1, 1));
					SetSize(size);
					Refresh();
				}
				else
				{
					m_GripperWindow.SetSize(Size(0, 0));
					m_GripperWindow.Hide();
				}
			}

			// Icons
			GDIBitmap GetMainIcon() const override
			{
				return m_MainIcon;
			}
			StdIcon GetMainIconID() const override
			{
				return m_MainIconID;
			}
			void SetMainIcon(const GDIBitmap& icon) override;
			void SetMainIcon(StdIcon iconID = DefaultIconID) override;

			// Caption
			wxString GetCaption() const override
			{
				return m_CaptionLabel->GetLabel();
			}
			void SetCaption(const wxString& label) override
			{
				m_CaptionLabel->SetLabel(label);
			}

			// Small label above main control
			wxString GetLabel() const override
			{
				return m_ViewLabel->GetLabel();
			}
			void SetLabel(const wxString& label) override;

			// Buttons customization
			void SetDefaultButton(WidgetID id) override;
			StdDialogControl GetButton(WidgetID id) const override;
			StdDialogControl AddButton(WidgetID id, const String& label = {}, bool prepend = false) override;

			WidgetID ShowModalDialog() override
			{
				return ShowModal();
			}

			// Keyboard control
			StdButtonsIDs GetCloseIDs() const
			{
				return m_CloseIDs;
			}
			void SetCloseIDs(const StdButtonsIDs& list)
			{
				m_CloseIDs = list;

				int style = GetWindowStyle();
				if (IsEscapeAllowed())
				{
					style |= wxCLOSE_BOX;
				}
				else
				{
					style &= ~wxCLOSE_BOX;
				}
				SetWindowStyle(style);
			}
			void SetDefaultCloseIDs()
			{
				m_CloseIDs = ms_DefaultCloseIDs;
				SetWindowStyle(GetWindowStyle()|wxCLOSE_BOX);
			}
			StdButtonsIDs GetEnterIDs() const
			{
				return m_EnterIDs;
			}
			void SetEnterIDs(const StdButtonsIDs& list)
			{
				m_EnterIDs = list;
			}
			void SetDefaultEnterDs()
			{
				m_EnterIDs = ms_DefaultEnterIDs;
			}

		public:
			wxDECLARE_ABSTRACT_CLASS(StdDialog);
	};
}
