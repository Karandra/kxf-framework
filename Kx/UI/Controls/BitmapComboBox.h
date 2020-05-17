#pragma once
#include "Kx/UI/Common.h"
#include "Kx/UI/WindowRefreshScheduler.h"
#include "Kx/Drawing/WithImageList.h"
#include "ComboBox.h"
#include <wx/systhemectrl.h>
#include <wx/bmpcbox.h>

namespace KxFramework::UI
{
	class KX_API BitmapComboBox: public WindowRefreshScheduler<wxSystemThemedControl<wxBitmapComboBox>>, public WithImageList
	{
		public:
			static constexpr ComboBoxStyle DefaultStyle = ComboBoxStyle::ProcessEnter|ComboBoxStyle::Dropdown|ComboBoxStyle::ReadOnly;

		private:
			std::unordered_map<size_t, int> m_ImageIDs;
			std::optional<wxSize> m_DefaultBitmapSize;

		public:
			BitmapComboBox() = default;
			BitmapComboBox(wxWindow* parent,
						   wxWindowID id,
						   const String& value = {},
						   ComboBoxStyle style = DefaultStyle,
						   const wxValidator& validator = wxDefaultValidator
			)
			{
				Create(parent, id, value, style, validator);
			}
			BitmapComboBox(wxWindow* parent,
						   wxWindowID id,
						   const String& value,
						   const wxPoint& pos,
						   const wxSize& size,
						   ComboBoxStyle style = DefaultStyle,
						   const wxValidator& validator = wxDefaultValidator
			)
			{
				Create(parent, id, value, pos, size, style, validator);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& value = {},
						ComboBoxStyle style = DefaultStyle,
						const wxValidator& validator = wxDefaultValidator
			)
			{
				return Create(parent, id, value, wxDefaultPosition, wxDefaultSize, style, validator);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& value,
						const wxPoint& pos,
						const wxSize& size,
						ComboBoxStyle style = DefaultStyle,
						const wxValidator& validator = wxDefaultValidator
			);

		public:
			void SetVisibleItemsCount(size_t count);
			int InsertItem(const String& label, size_t index, int imageID = Drawing::InvalidImageIndex);
			int AddItem(const String& label, int imageID = Drawing::InvalidImageIndex);
			void RemoveItem(size_t index);
			void Clear() override;

			int GetItemImage(size_t index) const;
			void SetItemImage(size_t index, int imageID = Drawing::InvalidImageIndex);

			wxSize GetBitmapSize() const override
			{
				if (m_DefaultBitmapSize)
				{
					return *m_DefaultBitmapSize;
				}
				return wxBitmapComboBox::GetBitmapSize();
			}
			void SetDefaultBitmapSize(const wxSize& size)
			{
				m_DefaultBitmapSize = size;
			}
			void SetDefaultBitmapSize()
			{
				m_DefaultBitmapSize = std::nullopt;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(BitmapComboBox);
	};
}