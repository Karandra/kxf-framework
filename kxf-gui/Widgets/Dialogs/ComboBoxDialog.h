#pragma once
#include "kxf-gui/Widgets/Common.h"
#include "StdDialog.h"
class wxComboBox;

namespace kxf::UI
{
	enum class ComboBoxDialogStyle: uint32_t
	{
		None = 0,

		GDIBitmap = 1 << 0,
		ReadOnly = 1 << 1
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(UI::ComboBoxDialogStyle);
	kxf_FlagSet_Extend(UI::ComboBoxDialogStyle, UI::DialogStyle);
}

namespace kxf::UI
{
	class KXF_API ComboBoxDialog: public StdDialog
	{
		public:
			static constexpr FlagSet<ComboBoxDialogStyle> DefaultStyle = ComboBoxDialogStyle::ReadOnly|*StdDialog::DefaultStyle;
			static constexpr int DefaultComboBoxWidth = 300;

		private:
			wxComboBox* m_View = nullptr;

		private:
			wxOrientation GetViewLabelSizerOrientation() const override;
			bool IsEnterAllowed(wxKeyEvent& event, WidgetID* idOut = nullptr) const override;

		public:
			ComboBoxDialog() = default;
			ComboBoxDialog(wxWindow* parent,
						   wxWindowID id,
						   const String& caption,
						   const Point& pos = Point::UnspecifiedPosition(),
						   const Size& size = Size::UnspecifiedSize(),
						   FlagSet<StdButton> buttons = DefaultButtons,
						   FlagSet<ComboBoxDialogStyle> style = DefaultStyle
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
						FlagSet<ComboBoxDialogStyle> style = DefaultStyle
			);

		public:
			wxComboBox* GetComboBox() const
			{
				return m_View;
			}
			wxWindow* GetDialogMainCtrl() const override
			{
				return GetComboBox();
			}
			int GetViewSizerProportion() const override
			{
				return 0;
			}
			
			int GetSelection()
			{
				if (GetComboBox())
				{
					return GetComboBox()->GetSelection();
				}
				return -1;
			}
			void SetSelection(int index)
			{
				if (GetComboBox())
				{
					GetComboBox()->SetSelection(index);
				}
			}
			
			String GetValue()
			{
				if (GetComboBox())
				{
					return GetComboBox()->GetValue();
				}
				return {};
			}
			void SetValue(const String& string)
			{
				if (GetComboBox())
				{
					GetComboBox()->SetValue(string);
				}
			}
			
			void SetItemLabel(int index, const String& label)
			{
				if (GetComboBox())
				{
					GetComboBox()->SetString(index, label);
				}
			}
			
			int InsertItem(int index, const String& label)
			{
				if (GetComboBox())
				{
					int nItemIndex = GetComboBox()->Insert(label, index);
					SetSelection(0);
					return nItemIndex;
				}
				return -1;
			}
			int AddItem(const String& label)
			{
				if (GetComboBox())
				{
					return InsertItem(GetComboBox()->GetCount(), label);
				}
				return -1;
			}
			void SetItems(const std::vector<String>& list)
			{
				if (GetComboBox())
				{
					GetComboBox()->Clear();
					for (const String& item: list)
					{
						GetComboBox()->Append(item);
					}
					SetSelection(0);
				}
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(ComboBoxDialog);
	};
}
