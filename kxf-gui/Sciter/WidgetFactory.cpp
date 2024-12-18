#include "kxf-pch.h"
#include "WidgetFactory.h"
#include "Widget.h"
#include "Host.h"
#include "Element.h"
#include "Stylesheets/MasterStylesheetStorage.h"
#include "kxf/Utility/Enumerator.h"

namespace
{
	std::vector<kxf::optional_ptr<kxf::Sciter::WidgetFactory>> g_RegisteredFactories;
}

namespace kxf::Sciter
{
	std::unique_ptr<Widget> WidgetFactory::NewWidget(Host& host, const Element& element, const String& fullyQualifiedClassName)
	{
		for (WidgetFactory& factory: EnumFactories())
		{
			if (factory.GetFullyQualifiedClassName() == fullyQualifiedClassName)
			{
				return factory.CreateWidget(host, element);
			}
		}
		return nullptr;
	}

	kxf::Enumerator<WidgetFactory&> WidgetFactory::EnumFactories()
	{
		return Utility::EnumerateIndexableContainer<WidgetFactory&, Utility::ReferenceOf>(g_RegisteredFactories);
	}
	void WidgetFactory::RegisterFactory(WidgetFactory& factory)
	{
		g_RegisteredFactories.emplace_back(factory);
	}
	void WidgetFactory::RegisterFactory(std::unique_ptr<WidgetFactory> factory)
	{
		g_RegisteredFactories.emplace_back(std::move(factory));
	}
	StylesheetStorage& WidgetFactory::GetStylesheetStorage()
	{
		return MasterStylesheetStorage::GetInstance();
	}
}
