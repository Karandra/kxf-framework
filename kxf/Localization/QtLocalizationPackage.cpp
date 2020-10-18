#include "stdafx.h"
#include "QtLocalizationPackage.h"
#include "kxf/Serialization/XML.h"

namespace kxf
{
	bool QtLocalizationPackage::DoLoadXML(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme)
	{
		if (loadingScheme.ExtractConsecutive(LoadingScheme::CONSECUTIVE_MASK) == LoadingScheme::Replace)
		{
			m_Items.clear();
		}

		if (XMLNode tsNode = xml.QueryElement(wxS("TS")))
		{
			m_Locale = tsNode.GetAttribute("language");
			m_Version = tsNode.GetAttribute("version");

			size_t count = 0;
			tsNode.EnumChildElements([&](XMLNode contextNode)
			{
				String name = contextNode.GetFirstChildElement(wxS("name")).GetValue();
				contextNode.EnumChildElements([&](XMLNode messageNode)
				{
					auto AddItem = [&](ResourceID id, LocalizationItem item)
					{
						if (item)
						{
							if (auto locationNode = messageNode.GetFirstChildElement(wxS("location")))
							{
								item.SetComment(String::Format(wxS("[Context=%1][FileName=%2][Line=%3]"), name, locationNode.GetAttribute(wxS("filename")), locationNode.GetAttribute(wxS("line"))));
							}

							if (loadingScheme.Contains(LoadingScheme::OverwriteExisting))
							{
								m_Items.insert_or_assign(std::move(id), std::move(item));
								count++;
							}
							else if (m_Items.emplace(std::move(id), std::move(item)).second)
							{
								count++;
							}
							return true;
						}
						return false;
					};

					AddItem(messageNode.GetFirstChildElement(wxS("source")).GetValue(), LocalizationItem(*this, messageNode.GetFirstChildElement(wxS("translation")).GetValue(), LocalizationItemFlag::Translatable));
					return true;
				}, wxS("message"));

				return true;
			}, wxS("context"));

			return count != 0;
		}
		return false;
	}
}