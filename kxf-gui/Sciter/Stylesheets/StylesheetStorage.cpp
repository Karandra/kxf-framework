#include "kxf-pch.h"
#include "StylesheetStorage.h"
#include "../SciterAPI.h"
#include "../Host.h"
#include "../Widget.h"
#include "../Private/Conversion.h"
#include "kxf/Utility/Container.h"
#include "kxf/Utility/Enumerator.h"

namespace
{
	kxf::String MakeFullCSS(const std::vector<kxf::String>& items)
	{
		using namespace kxf;
		using namespace kxf::Sciter;

		String fullCSS;
		fullCSS.reserve([&]()
		{
			size_t length = 0;
			for (const String& stylesheet: items)
			{
				length += stylesheet.length() + 1;
			}
			return length;
		}());
		for (const String& stylesheet: items)
		{
			fullCSS += stylesheet;
			fullCSS += '\n';
		}

		return fullCSS;
	}
}

namespace kxf::Sciter
{
	bool StylesheetStorage::AddItem(String stylesheet)
	{
		if (!stylesheet.IsEmptyOrWhitespace())
		{
			auto it = Utility::Container::FindIf(m_Items, [&](const String& item)
			{
				return item == stylesheet;
			});
			if (it != m_Items.end())
			{
				m_Items.erase(it);
			}
			m_Items.emplace_back(std::move(stylesheet));
			return true;
		}
		return false;
	}
	void StylesheetStorage::ClearItems() noexcept
	{
		m_Items.clear();
	}
	size_t StylesheetStorage::CopyItems(const StylesheetStorage& other)
	{
		size_t count = 0;
		for (const String& item: other.EnumItems())
		{
			if (AddItem(item))
			{
				count++;
			}
		};
		return count;
	}
	size_t StylesheetStorage::TakeItems(StylesheetStorage&& other)
	{
		size_t count = 0;
		for (String& item: std::move(other).EnumItems())
		{
			if (AddItem(std::move(item)))
			{
				count++;
			}
		};
		return count;
	}

	Enumerator<const String&> StylesheetStorage::EnumItems() const&
	{
		return Utility::EnumerateIndexableContainer<const String&>(m_Items);
	}
	Enumerator<kxf::String> StylesheetStorage::EnumItems() &&
	{
		return Utility::EnumerateIndexableContainer<String>(std::move(m_Items));
	}

	bool StylesheetStorage::Apply(Host& host, const FSPath& basePath) const
	{
		auto utf8 = ToSciterUTF8(MakeFullCSS(m_Items));
		auto basePathString = FSPathToSciterAddress(basePath);
		return GetSciterAPI()->SciterSetCSS(host.GetWindow().GetHandle(), utf8.data(), utf8.size(), basePathString.wc_str(), L"");
	}
	bool StylesheetStorage::Apply(Host& host, const URI& baseURI) const
	{
		auto utf8 = ToSciterUTF8(MakeFullCSS(m_Items));
		auto basePathString = URIToSciterAddress(baseURI);
		return GetSciterAPI()->SciterSetCSS(host.GetWindow().GetHandle(), utf8.data(), utf8.size(), basePathString.wc_str(), L"");
	}
}
