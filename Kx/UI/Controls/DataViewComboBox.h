#pragma once
#include "Kx/UI/Common.h"
#include "ComboPopup.h"
#include "Kx/General/WithOptions.h"
#include "KxFramework/DataView2/DataView2.h"

namespace KxFramework::UI
{
	class Panel;
	class ComboControl;
}

namespace KxFramework::UI::DataView
{
	using namespace KxDataView2;

	enum class ComboCtrlOption
	{
		None = 0,
		DismissOnSelect = 1 << 0,
		ForceGetStringOnDismiss = 1 << 1,
		AltPopupWindow = 1 << 2,
		HorizontalLayout = 1 << 3,
	};
}
namespace KxFramework::EnumClass
{
	Kx_EnumClass_AllowEverything(UI::DataView::ComboCtrlOption);
}

namespace KxFramework::UI::DataView
{
	class KX_API ComboCtrl:
		public View,
		public ComboPopup,
		public WithOptions<ComboCtrlOption, ComboCtrlOption::ForceGetStringOnDismiss|ComboCtrlOption::AltPopupWindow>
	{
		public:
			static constexpr int DefaultStyle = wxCB_READONLY;
			static constexpr CtrlStyle DefaultDataViewStyle = CtrlStyle::Default;

			KxEVENT_MEMBER(KxDataView2::Event, GetStringValue);
			KxEVENT_MEMBER(KxDataView2::Event, SetStringValue);

		protected:
			CtrlStyle m_DataViewFlags = DefaultDataViewStyle;
			wxBoxSizer* m_Sizer = nullptr;
			Panel* m_BackgroundWindow = nullptr;

			ComboControl* m_ComboCtrl = nullptr;
			wxEvtHandler m_EvtHandler_DataView;
			wxEvtHandler m_EvtHandler_ComboCtrl;
			Node* m_Selection = nullptr;
			bool m_PopupWinEvtHandlerPushed = false;
			int m_MaxVisibleItems = -1;

		private:
			int CalculateItemsHeight() const;
			void UpdatePopupHeight();

		protected:
			void OnInternalIdle() override;
			void OnSelectItem(KxDataView2::Event& event);
			void OnScroll(wxMouseEvent& event);

			bool FindItem(const wxString& value, wxString* trueItem = nullptr) override;
			void OnPopup() override;
			void OnDismiss() override;
			void OnDoShowPopup() override;
			bool Create(wxWindow* window) override;

		public:
			ComboCtrl() = default;
			ComboCtrl(wxWindow* parent,
						 wxWindowID id,
						 long style = DefaultStyle,
						 const wxValidator& validator = wxDefaultValidator
			)
			{
				Create(parent, id, style, validator);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						long style = DefaultStyle,
						const wxValidator& validator = wxDefaultValidator
			);
			~ComboCtrl();

		public:
			wxWindow* GetControl() override;
			wxComboCtrl* GetComboControl();
			wxWindow* ComboGetBackgroundWindow();
			wxString GetStringValue() const override;
			void SetStringValue(const wxString& value) override;

			CtrlStyle GetDataViewFlags() const
			{
				return m_DataViewFlags;
			}
			void SetDataViewFlags(CtrlStyle flags)
			{
				m_DataViewFlags = flags;
			}

			void ComboPopup();
			void ComboDismiss();
			void ComboRefreshLabel();

			int ComboGetMaxVisibleItems() const;
			void ComboSetMaxVisibleItems(int count);

			void ComboSetPopupExtents(int nLeft = 0, int nRight = 0);
			void ComboSetPopupMinWidth(int width = -1);
			void ComboSetPopupAnchor(wxDirection nSide = static_cast<wxDirection>(0));

		public:
			wxDECLARE_DYNAMIC_CLASS_NO_COPY(ComboCtrl);
	};
}