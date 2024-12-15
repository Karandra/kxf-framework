#include "KxfPCH.h"
#include "AndroidLocalizationPackage.h"
#include "kxf/Serialization/XML.h"

namespace kxf
{
	bool AndroidLocalizationPackage::DoLoadXML(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme)
	{
		if (loadingScheme.ExtractConsecutive(LoadingScheme::CONSECUTIVE_MASK) == LoadingScheme::Replace)
		{
			m_Items.clear();
		}

		if (XMLNode resourcesNode = xml.QueryElement("resources"))
		{
			m_Items.reserve(resourcesNode.GetChildrenCount());

			size_t count = 0;
			auto AddItem = [&](const XMLNode& itemNode, LocalizationItem item)
			{
				if (item)
				{
					ResourceID id = itemNode.GetAttribute("name");

					if (auto maxLength = itemNode.QueryAttributeInt("maxLength"))
					{
						item.SetMaxLength(*maxLength);
					}
					if (auto comment = itemNode.QueryAttribute("comment"))
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

			resourcesNode.EnumChildElements([&](const XMLNode& itemNode)
			{
				FlagSet<LocalizationItemFlag> flags;
				flags.Mod(LocalizationItemFlag::Translatable, itemNode.GetAttributeBool("translatable", true));

				auto itemName = itemNode.GetName();
				if (itemName == "string")
				{
					AddItem(itemNode, LocalizationItem(*this, itemNode.GetValue(), flags));
				}
				else if (itemName == "string-array")
				{
					LocalizationItem::TMultipleItems items;
					items.reserve(itemNode.GetChildrenCount());

					itemNode.EnumChildElements([&](XMLNode node)
					{
						if (auto value = node.GetValue(); !value.IsEmpty())
						{
							items.emplace_back(std::move(value));
						}
					}, "item");
					AddItem(itemNode, LocalizationItem(*this, std::move(items), flags));
				}
				else if (itemName == "plurals")
				{
					LocalizationItem::TPlurals plurals;
					itemNode.EnumChildElements([&](XMLNode node)
					{
						const String name = node.GetAttribute("quantity");
						if (name == "one")
						{
							plurals.emplace(LocalizationItemQuantity::One, node.GetValue());
							return CallbackCommand::Continue;
						}
						else if (name == "few")
						{
							plurals.emplace(LocalizationItemQuantity::Few, node.GetValue());
							return CallbackCommand::Continue;
						}
						else if (name == "many")
						{
							plurals.emplace(LocalizationItemQuantity::Many, node.GetValue());
							return CallbackCommand::Continue;
						}
						else if (name == "other")
						{
							plurals.emplace(LocalizationItemQuantity::Other, node.GetValue());
							return CallbackCommand::Continue;
						}
						return CallbackCommand::Discard;
					}, "item");
					AddItem(itemNode, LocalizationItem(*this, std::move(plurals), flags));
				}
			});
			return count != 0;
		}
		return false;
	}

	std::vector<String> AndroidLocalizationPackage::GetFileExtensions() const
	{
		return {"xml"};
	}
}
