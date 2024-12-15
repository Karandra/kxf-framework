#include "KxfPCH.h"
#include "WindowsLocalizationPackage.h"
#include "kxf/Serialization/XML.h"

namespace kxf
{
	bool WindowsLocalizationPackage::DoLoadXML(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme)
	{
		if (loadingScheme.ExtractConsecutive(LoadingScheme::CONSECUTIVE_MASK) == LoadingScheme::Replace)
		{
			m_Items.clear();
		}

		if (XMLNode rootNode = xml.QueryElement("root"))
		{
			m_Items.reserve(rootNode.GetChildrenCount());

			size_t count = 0;
			auto AddItem = [&](const XMLNode& itemNode, ResourceID id, LocalizationItem item)
			{
				if (item)
				{
					if (auto comment = itemNode.GetFirstChildElement("comment").QueryValue())
					{
						item.SetComment(std::move(*comment));
					}

					if (loadingScheme.Contains(LoadingScheme::OverwriteExisting))
					{
						m_Items.insert_or_assign(std::move(id), std::move(item));
						count++;
					}
					else
					{
						if (m_Items.emplace(std::move(id), std::move(item)).second)
						{
							count++;
						}
					}
					return true;
				}
				return false;
			};

			rootNode.EnumChildElements([&](XMLNode itemNode)
			{
				auto name = itemNode.GetAttribute("name");
				auto value = itemNode.GetFirstChildElement("value").GetValue();

				AddItem(itemNode, std::move(name), LocalizationItem(*this, std::move(value), LocalizationItemFlag::Translatable));
			}, "data");
			return count != 0;
		}
		return false;
	}

	std::vector<String> WindowsLocalizationPackage::GetFileExtensions() const
	{
		return {"resx", "resw"};
	}
}
