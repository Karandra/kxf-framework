#include "kxf-pch.h"
#include "Common.h"
#include "../IWidget.h"
#include <wx/window.h>

namespace
{
	std::unordered_map<const wxObject*, std::weak_ptr<kxf::IWidget>> g_WXObjectMap;
}

namespace kxf::Private
{
	void AssociateWXObject(wxObject& object, IWidget& widget) noexcept
	{
		g_WXObjectMap.insert_or_assign(&object, widget.QueryInterface<IWidget>());
	}
	void AssociateWXObject(wxWindow& window, IWidget& widget) noexcept
	{
		AssociateWXObject(static_cast<wxEvtHandler&>(window), widget);
	}
	void AssociateWXObject(wxEvtHandler& evtHandler, IWidget& widget) noexcept
	{
		evtHandler.SetClientData(static_cast<IWidget*>(&widget));
	}

	void DissociateWXObject(wxObject& object) noexcept
	{
		g_WXObjectMap.erase(&object);
	}
	void DissociateWXObject(wxWindow& window) noexcept
	{
		DissociateWXObject(static_cast<wxEvtHandler&>(window));
	}
	void DissociateWXObject(wxEvtHandler& evtHandler) noexcept
	{
		evtHandler.SetClientData(nullptr);
	}

	std::shared_ptr<IWidget> FindByWXObject(const wxObject& object) noexcept
	{
		auto it = g_WXObjectMap.find(&object);
		if (it != g_WXObjectMap.end())
		{
			return it->second.lock();
		}
		return nullptr;
	}
	std::shared_ptr<IWidget> FindByWXObject(const wxWindow& window) noexcept
	{
		return FindByWXObject(static_cast<const wxEvtHandler&>(window));
	}
	std::shared_ptr<IWidget> FindByWXObject(const wxEvtHandler& evtHandler) noexcept
	{
		if (auto widget = static_cast<IWidget*>(evtHandler.GetClientData()))
		{
			return widget->QueryInterface<IWidget>();
		}
		return nullptr;
	}
}
