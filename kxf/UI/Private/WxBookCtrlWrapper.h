#pragma once
#include "Common.h"
#include "../IBookWidget.h"
class wxBookCtrlBase;

namespace kxf::WXUI::Private
{
	class KXF_API WxBookCtrlWrapper
	{
		protected:
			wxBookCtrlBase* m_BookCtrl = nullptr;

		public:
			WxBookCtrlWrapper() noexcept = default;
			WxBookCtrlWrapper(wxBookCtrlBase& bookCtrl) noexcept
			{
				Initialize(bookCtrl);
			}

		public:
			void Initialize(wxBookCtrlBase& bookCtrl) noexcept;

		public:
			size_t InsertPage(IWidget& widget, const String& label, size_t index = IBookWidget::npos);
			bool RemovePage(size_t index);
			std::shared_ptr<IWidget> GetPage(size_t index) const;
			size_t GetPageCount() const;

			String GetPageLabel(size_t index) const;
			void SetPageLabel(size_t index, const String& label) const;

			size_t GetSelectedPage() const;
			size_t SetSelectedPage(size_t index);

			std::shared_ptr<IWidget> GetSelectedWidget() const;
			size_t GetSelectedPageByWidget(const IWidget& widget) const;
	};
}
