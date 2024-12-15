#include "KxfPCH.h"
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

		if (XMLNode tsNode = xml.QueryElement("TS"))
		{
			m_Locale = tsNode.GetAttribute("language");
			m_Version = tsNode.GetAttribute("version");

			size_t count = 0;
			auto AddItem = [&](const XMLNode& messageNode, const String& contextName, ResourceID id, LocalizationItem item)
			{
				if (item)
				{
					if (auto locationNode = messageNode.GetFirstChildElement("location"))
					{
						item.SetComment(Format("[Context={}][FileName={}][Line={}]", contextName, locationNode.GetAttribute("filename"), locationNode.GetAttribute("line")));
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

			tsNode.EnumChildElements([&](XMLNode contextNode)
			{
				auto contextName = contextNode.GetFirstChildElement("name").GetValue();

				contextNode.EnumChildElements([&](XMLNode messageNode)
				{
					auto id = messageNode.GetFirstChildElement("source").GetValue();
					auto value = messageNode.GetFirstChildElement("translation").GetValue();

					AddItem(messageNode, contextName, std::move(id), LocalizationItem(*this, std::move(value), LocalizationItemFlag::Translatable));
				}, "message");
			}, "context");
			return count != 0;
		}
		return false;
	}

	std::vector<String> QtLocalizationPackage::GetFileExtensions() const
	{
		return {"ts"};
	}
}
