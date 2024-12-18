#pragma once
#include "Common.h"
#include "../IScrollableWidget.h"
class wxScrollHelper;

namespace kxf::WXUI::Private
{
	class KXF_API WxScrolledWrapper
	{
		protected:
			wxScrollHelper* m_ScrollHelper = nullptr;
			bool m_IsKeyboardScrollingEnabled = false;

		public:
			WxScrolledWrapper() noexcept = default;
			WxScrolledWrapper(wxScrollHelper& scrollHelper) noexcept
			{
				Initialize(scrollHelper);
			}

		public:
			void Initialize(wxScrollHelper& scrollHelper) noexcept;

		public:
			bool IsKeyboardScrollingEnabled() const;
			void SetKeyboardScrollingEnabled(bool enabled = true);

			Point CalcScrolledPosition(const Point& point) const;
			Point CalcUnscrolledPosition(const Point& point) const;

			void SetupScrollbars(const Size& pixelPerUnit, const Size& unitCount);
			void ShowScrollbars(ScrollbarVisibility x, ScrollbarVisibility y);

			Size GetScrollRate() const;
			void SetScrollRate(const Size& scrollRate);

			Point GetViewStart() const;
			void ScrollTo(const Point& point);

			SizeF GetScrollScale() const;
			void SetScrollScale(const SizeF& scale);
	};
}
