#include "kxf-pch.h"
#include "Widget.h"
#include "kxf/Application/ICoreApplication.h"
#include "Widgets/TextBoxWidget.h"

namespace
{
	namespace Attribute
	{
		constexpr auto Widget = "Widget";
	}
}

namespace kxf::Sciter
{
	Widget* Widget::DoFromElement(const Element& element)
	{
		if (auto address = element.GetAttribute(Attribute::Widget).ToInteger<size_t>())
		{
			return reinterpret_cast<Widget*>(*address);
		}
		return nullptr;
	}

	// Widget
	void Widget::OnAttached()
	{
		m_Element.SetAttribute(Attribute::Widget, std::to_wstring(reinterpret_cast<size_t>(this)));
	}
	void Widget::OnDetached()
	{
		m_Element.RemoveAttribute(Attribute::Widget);

		// In Sciter example we delete the object here
		ICoreApplication::GetInstance()->ScheduleForDestruction(std::unique_ptr<IObject>(this));
	}

	// Layout
	LayoutFlow Widget::GetLayoutFlow() const
	{
		String flow = m_Element.GetStyleAttribute("flow");
		if (flow == "horizontal")
		{
			return LayoutFlow::Horizontal;
		}
		else if (flow == "vertical")
		{
			return LayoutFlow::Vertical;
		}
		else if (flow == "text")
		{
			return LayoutFlow::Text;
		}
		return LayoutFlow::None;
	}
	void Widget::SetLayoutFlow(LayoutFlow flow)
	{
		switch (flow)
		{
			case LayoutFlow::Horizontal:
			{
				m_Element.SetStyleAttribute("flow", "horizontal");
				break;
			}
			case LayoutFlow::Vertical:
			{
				m_Element.SetStyleAttribute("flow", "vertical");
				break;
			}
			case LayoutFlow::Text:
			{
				m_Element.SetStyleAttribute("flow", "text");
				break;
			}
			default:
			{
				m_Element.RemoveStyleAttribute("flow");
				break;
			}
		};
	}

	// Size and position
	void Widget::SetSize(const Size& size)
	{
		if (size.GetWidth() != wxDefaultCoord)
		{
			m_Element.SetStyleAttribute("width", size.GetWidth(), SizeUnit::dip);
		}
		else
		{
			m_Element.RemoveStyleAttribute("width");
		}

		if (size.GetHeight() != wxDefaultCoord)
		{
			m_Element.SetStyleAttribute("height", size.GetHeight(), SizeUnit::dip);
		}
		else
		{
			m_Element.RemoveStyleAttribute("height");
		}
	}
	void Widget::SetPosition(const Point& pos)
	{
		m_Element.SetStyleAttribute("position", "relative");
		if (pos.GetX() != wxDefaultCoord)
		{
			m_Element.SetStyleAttribute("left", pos.GetX(), SizeUnit::dip);
		}
		else
		{
			m_Element.RemoveStyleAttribute("left");
		}

		if (pos.GetY() != wxDefaultCoord)
		{
			m_Element.SetStyleAttribute("top", pos.GetY(), SizeUnit::dip);
		}
		else
		{
			m_Element.RemoveStyleAttribute("top");
		}
	}
}
