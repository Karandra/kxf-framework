#include "kxf-pch.h"
#include "WxScrolledWrapper.h"
#include <wx/scrolwin.h>

namespace kxf::WXUI::Private
{
	void WxScrolledWrapper::Initialize(wxScrollHelper& scrollHelper) noexcept
	{
		m_ScrollHelper = &scrollHelper;
		m_IsKeyboardScrollingEnabled = true;
	}

	bool WxScrolledWrapper::IsKeyboardScrollingEnabled() const
	{
		return m_IsKeyboardScrollingEnabled;
	}
	void WxScrolledWrapper::SetKeyboardScrollingEnabled(bool enabled)
	{
		if (!enabled)
		{
			m_ScrollHelper->DisableKeyboardScrolling();
			m_IsKeyboardScrollingEnabled = false;
		}
	}

	Point WxScrolledWrapper::CalcScrolledPosition(const Point& point) const
	{
		return Point(m_ScrollHelper->CalcScrolledPosition(point));
	}
	Point WxScrolledWrapper::CalcUnscrolledPosition(const Point& point) const
	{
		return Point(m_ScrollHelper->CalcUnscrolledPosition(point));
	}

	void WxScrolledWrapper::SetupScrollbars(const Size& pixelPerUnit, const Size& unitCount)
	{
		m_ScrollHelper->SetScrollbars(pixelPerUnit.GetWidth(), pixelPerUnit.GetHeight(), unitCount.GetWidth(), unitCount.GetHeight());
	}
	void WxScrolledWrapper::ShowScrollbars(ScrollbarVisibility x, ScrollbarVisibility y)
	{
		auto Map = [](ScrollbarVisibility value) noexcept
		{
			switch (value)
			{
				case ScrollbarVisibility::ShowAlways:
				{
					return wxScrollbarVisibility::wxSHOW_SB_ALWAYS;
				}
				case ScrollbarVisibility::ShowNever:
				{
					return wxScrollbarVisibility::wxSHOW_SB_NEVER;
				}
			};
			return wxScrollbarVisibility::wxSHOW_SB_DEFAULT;
		};

		m_ScrollHelper->ShowScrollbars(Map(x), Map(y));
	}

	Size WxScrolledWrapper::GetScrollRate() const
	{
		Size rate;
		m_ScrollHelper->GetScrollPixelsPerUnit(&rate.Width(), &rate.Height());

		return rate;
	}
	void WxScrolledWrapper::SetScrollRate(const Size& scrollRate)
	{
		m_ScrollHelper->SetScrollRate(scrollRate.GetWidth(), scrollRate.GetHeight());
	}

	Point WxScrolledWrapper::GetViewStart() const
	{
		return Point(m_ScrollHelper->GetViewStart());
	}
	void WxScrolledWrapper::ScrollTo(const Point& point)
	{
		m_ScrollHelper->Scroll(point);
	}

	SizeF WxScrolledWrapper::GetScrollScale() const
	{
		return SizeD(m_ScrollHelper->GetScaleX(), m_ScrollHelper->GetScaleY());
	}
	void WxScrolledWrapper::SetScrollScale(const SizeF& scale)
	{
		m_ScrollHelper->SetScale(scale.GetWidth(), scale.GetHeight());
	}
}
