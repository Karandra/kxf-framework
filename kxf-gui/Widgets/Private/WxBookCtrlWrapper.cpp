#include "kxf-pch.h"
#include "WxBookCtrlWrapper.h"
#include "../IWidget.h"
#include <wx/bookctrl.h>

namespace kxf::WXUI::Private
{
	void WxBookCtrlWrapper::Initialize(wxBookCtrlBase& bookCtrl) noexcept
	{
		m_BookCtrl = &bookCtrl;
	}

	size_t WxBookCtrlWrapper::InsertPage(IWidget& widget, const String& label, size_t index)
	{
		if (auto window = widget.GetWxWindow())
		{
			if (m_BookCtrl->InsertPage(index, widget.GetWxWindow(), label, false))
			{
				return index;
			}
		}
		return IBookWidget::npos;
	}
	bool WxBookCtrlWrapper::RemovePage(size_t index)
	{
		return m_BookCtrl->RemovePage(index);
	}
	std::shared_ptr<IWidget> WxBookCtrlWrapper::GetPage(size_t index) const
	{
		if (auto window = m_BookCtrl->GetPage(index))
		{
			return kxf::Private::FindByWXObject(*window);
		}
		return nullptr;
	}
	size_t WxBookCtrlWrapper::GetPageCount() const
	{
		return m_BookCtrl->GetPageCount();
	}

	String WxBookCtrlWrapper::GetPageLabel(size_t index) const
	{
		return m_BookCtrl->GetPageText(index);
	}
	void WxBookCtrlWrapper::SetPageLabel(size_t index, const String& label) const
	{
		m_BookCtrl->SetPageText(index, label);
	}

	size_t WxBookCtrlWrapper::GetSelectedPage() const
	{
		int index = m_BookCtrl->GetSelection();
		return index >= 0 ? index : IBookWidget::npos;
	}
	size_t WxBookCtrlWrapper::SetSelectedPage(size_t index)
	{
		int oldIndex = m_BookCtrl->ChangeSelection(index);
		return oldIndex >= 0 ? oldIndex : IBookWidget::npos;
	}

	std::shared_ptr<IWidget> WxBookCtrlWrapper::GetSelectedWidget() const
	{
		if (auto window = m_BookCtrl->GetCurrentPage())
		{
			return kxf::Private::FindByWXObject(*window);
		}
		return nullptr;
	}
	size_t WxBookCtrlWrapper::GetSelectedPageByWidget(const IWidget& widget) const
	{
		int index = m_BookCtrl->FindPage(widget.GetWxWindow());
		return index >= 0 ? index : IBookWidget::npos;
	}
}
