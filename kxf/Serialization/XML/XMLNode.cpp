#include "kxf-pch.h"
#include "XMLDocument.h"
#include "Private/Utility.h"
#include "kxf/Core/IEncodingConverter.h"

namespace kxf
{
	XMLNode XMLNode::ConstructOrQueryElement(const String& xPath, bool allowCreate)
	{
		if (!IsNull())
		{
			tinyxml2::XMLDocument& document = GetDocument().m_Document;
			tinyxml2::XMLNode* currentNode = GetNode();
			tinyxml2::XMLNode* previousNode = currentNode;

			size_t itemCount = xPath.SplitBySeparator(GetXPathSeparator(), [&](StringView name)
			{
				// Save previous element
				if (!currentNode)
				{
					return false;
				}
				previousNode = currentNode;

				// Extract index from name and remove it from path, zero-based
				// point/x -> 0, point/x::1 -> 1, point/y::0 -> 0, point/z::-7 -> 0
				auto [elementName, index] = XNode::ExtractIndexFromName(name, StringViewOf(GetDocument().m_XPathIndexSeparator));
				auto elementNameUTF8 = EncodingConverter_UTF8.ToMultiByte(name);

				// Get level 0
				currentNode = previousNode->FirstChildElement(elementNameUTF8.data());
				if (!currentNode)
				{
					if (allowCreate)
					{
						currentNode = document.NewElement(elementNameUTF8.data());
						previousNode->InsertEndChild(currentNode);
					}
					else
					{
						return false;
					}
				}

				// We need to go down by 'index' more elements
				for (int level = 1; level <= index; level++)
				{
					// Get next level
					currentNode = currentNode->NextSiblingElement(elementNameUTF8.data());
					if (!currentNode)
					{
						if (allowCreate)
						{
							currentNode = document.NewElement(elementNameUTF8.data());
							previousNode->InsertAfterChild(previousNode, currentNode);
						}
						else
						{
							return false;
						}
					}
				}
				return true;
			});

			if (currentNode && itemCount != 0)
			{
				return XMLNode(currentNode, GetDocument());
			}
		}
		return {};
	}

	std::optional<String> XMLNode::DoGetValue() const
	{
		if (auto node = GetNode())
		{
			if (auto text = node->ToText())
			{
				return XML::Private::ToString(text->Value());
			}
			else if (auto element = node->ToElement())
			{
				if (const char* text = element->GetText())
				{
					return XML::Private::ToString(text);
				}
			}
			else if (const char* value = node->Value())
			{
				return XML::Private::ToString(value);
			}
		}
		return {};
	}
	std::optional<int64_t> XMLNode::DoGetValueIntWithBase(int base) const
	{
		if (base == 10)
		{
			if (auto node = GetNode())
			{
				if (auto element = node->ToElement())
				{
					int64_t value = 0;
					if (element->QueryInt64Text(&value) == tinyxml2::XML_SUCCESS)
					{
						return value;
					}
				}
			}
			return {};
		}
		return XNode::DoGetValueIntWithBase(base);
	}
	std::optional<double> XMLNode::DoGetValueFloat() const
	{
		if (auto node = GetNode())
		{
			if (auto element = node->ToElement())
			{
				double value = 0;
				if (element->QueryDoubleText(&value) == tinyxml2::XML_SUCCESS)
				{
					return value;
				}
			}
		}
		return {};
	}
	std::optional<bool> XMLNode::DoGetValueBool() const
	{
		if (auto node = GetNode())
		{
			if (auto element = node->ToElement())
			{
				bool value = false;
				if (element->QueryBoolText(&value) == tinyxml2::XML_SUCCESS)
				{
					return value;
				}
			}
		}
		return {};
	}
	bool XMLNode::DoSetValue(const String& value, WriteEmpty writeEmpty, AsCDATA asCDATA)
	{
		if (writeEmpty == WriteEmpty::Never && value.IsEmpty())
		{
			return false;
		}

		if (auto node = GetNode())
		{
			if (node->ToElement())
			{
				if (!value.IsEmpty())
				{
					auto utf8 = value.ToUTF8();
					tinyxml2::XMLText* textNode = m_Document->GetDocument()->NewText(utf8.data());

					switch (asCDATA)
					{
						case AsCDATA::Always:
						{
							textNode->SetCData(true);
							break;
						}
						case AsCDATA::Never:
						{
							textNode->SetCData(false);
							break;
						}
						default:
						{
							textNode->SetCData(ContainsValueForbiddenCharacters(value));
							break;
						}
					};

					node->DeleteChildren();
					node->InsertFirstChild(textNode);

					return true;
				}
			}
			else if (node->ToText() || node->ToComment() || node->ToDeclaration() || node->ToUnknown())
			{
				auto utf8 = value.ToUTF8();
				node->SetValue(utf8.data());
				return true;
			}
		}
		return false;
	}

	std::optional<String> XMLNode::DoGetAttribute(const String& name) const
	{
		if (GetNode())
		{
			if (auto node = GetNode()->ToElement())
			{
				auto utf8 = name.ToUTF8();
				if (const char* value = node->Attribute(utf8.data()))
				{
					return XML::Private::ToString(value);
				}
			}
		}
		return {};
	}
	std::optional<int64_t> XMLNode::DoGetAttributeIntWithBase(const String& name, int base) const
	{
		if (base == 10)
		{
			if (GetNode())
			{
				if (auto node = GetNode()->ToElement())
				{
					auto utf8 = name.ToUTF8();

					int64_t value = 0;
					if (node->QueryInt64Attribute(utf8.data(), &value) == tinyxml2::XML_SUCCESS)
					{
						return value;
					}
				}
			}
			return {};
		}
		return XNode::DoGetAttributeIntWithBase(name, base);
	}
	std::optional<double> XMLNode::DoGetAttributeFloat(const String& name) const
	{
		if (GetNode())
		{
			if (auto node = GetNode()->ToElement())
			{
				auto utf8 = name.ToUTF8();

				double value = 0;
				if (node->QueryDoubleAttribute(utf8.data(), &value) == tinyxml2::XML_SUCCESS)
				{
					return value;
				}
			}
		}
		return {};
	}
	std::optional<bool> XMLNode::DoGetAttributeBool(const String& name) const
	{
		if (GetNode())
		{
			if (auto node = GetNode()->ToElement())
			{
				auto utf8 = name.ToUTF8();

				bool value = false;
				if (node->QueryBoolAttribute(utf8.data(), &value) == tinyxml2::XML_SUCCESS)
				{
					return value;
				}
			}
		}
		return {};
	}
	bool XMLNode::DoSetAttribute(const String& name, const String& value, WriteEmpty writeEmpty)
	{
		if (writeEmpty == WriteEmpty::Never && value.IsEmpty())
		{
			return false;
		}

		if (GetNode())
		{
			if (auto node = GetNode()->ToElement())
			{
				auto nameUTF8 = name.ToUTF8();
				auto valueUTF8 = value.ToUTF8();
				node->SetAttribute(nameUTF8.data(), valueUTF8.data());
				return true;
			}
		}
		return false;
	}

	// General
	String XMLNode::GetXPath() const
	{
		return ConstructXPath(*this);
	}
	XMLNode XMLNode::QueryElement(const String& xPath) const
	{
		return const_cast<XMLNode&>(*this).ConstructOrQueryElement(xPath, false);
	}
	XMLNode XMLNode::ConstructElement(const String& xPath)
	{
		return const_cast<XMLNode&>(*this).ConstructOrQueryElement(xPath, true);
	}

	String XMLNode::GetXPathIndexSeparator() const
	{
		return m_Document ? m_Document->GetXPathIndexSeparator() : NullString;
	}
	void XMLNode::SetXPathIndexSeparator(const String& value)
	{
		if (m_Document)
		{
			m_Document->SetXPathIndexSeparator(value);
		}
	}

	// Node
	size_t XMLNode::GetIndexWithinParent() const
	{
		if (auto node = GetNode(); node && node->ToElement())
		{
			if (auto parentElement = node->Parent() ? node->Parent()->ToElement() : nullptr)
			{
				size_t index = 0;
				for (const tinyxml2::XMLElement* element = parentElement->FirstChildElement(); element; element = element->NextSiblingElement())
				{
					index++;
					if (element == node->ToElement())
					{
						break;
					}
				}
				return index;
			}
		}
		return 0;
	}
	String XMLNode::GetName() const
	{
		if (GetNode())
		{
			if (auto node = GetNode()->ToElement())
			{
				return XML::Private::ToString(node->Name());
			}
		}
		return {};
	}
	bool XMLNode::SetName(const String& name)
	{
		if (GetNode())
		{
			if (auto node = GetNode()->ToElement())
			{
				auto utf8 = name.ToUTF8();
				node->SetName(utf8.data());
				return true;
			}
		}
		return false;
	}

	size_t XMLNode::GetChildrenCount() const
	{
		size_t count = 0;
		if (auto node = GetNode())
		{
			for (auto child = node->FirstChild(); child ; child = child->NextSibling())
			{
				count++;
			}
		}
		return count;
	}
	bool XMLNode::HasChildren() const
	{
		auto node = GetNode();
		if (node)
		{
			return !node->NoChildren();
		}
		return false;
	}
	bool XMLNode::ClearChildren()
	{
		if (auto node = GetNode())
		{
			node->DeleteChildren();
			return true;
		}
		return false;
	}
	bool XMLNode::ClearNode()
	{
		ClearChildren();
		ClearAttributes();
		return true;
	}

	CallbackResult<void> XMLNode::EnumChildren(CallbackFunction<XMLNode> func) const
	{
		if (auto node = GetNode())
		{
			for (auto child = node->FirstChild(); child; child = child->NextSibling())
			{
				if (func.Invoke(XMLNode(child, *m_Document)).ShouldTerminate())
				{
					break;
				}
			}

			return func.Finalize();
		}
		return {};
	}
	CallbackResult<void> XMLNode::EnumChildElements(CallbackFunction<XMLNode> func, const String& name) const
	{
		if (auto node = GetNode())
		{
			auto utf8 = name.ToUTF8();
			auto namePtr = !name.IsEmpty() ? utf8.data() : nullptr;

			for (auto child = node->FirstChildElement(namePtr); child; child = child->NextSiblingElement(namePtr))
			{
				if (func.Invoke(XMLNode(child, *m_Document)).ShouldTerminate())
				{
					break;
				}
			}

			return func.Finalize();
		}
		return {};
	}

	String XMLNode::GetXML(SerializationFormat mode) const
	{
		if (auto node = GetNode())
		{
			tinyxml2::XMLDocument subTree;
			node->DeepClone(&subTree);
			return XML::Private::PrintDocument(subTree, mode == SerializationFormat::HTML5);
		}
		return {};
	}
	XML::NodeType XMLNode::GetType() const
	{
		auto node = GetNode();
		if (node)
		{
			if (node->ToDocument())
			{
				return NodeType::Document;
			}
			else if (node->ToElement())
			{
				return NodeType::Element;
			}
			else if (node->ToText())
			{
				return NodeType::Text;
			}
			else if (node->ToComment())
			{
				return NodeType::Comment;
			}
			else if (node->ToDeclaration())
			{
				return NodeType::Declaration;
			}
			else if (node->ToUnknown())
			{
				return NodeType::Unknown;
			}
		}
		return NodeType::None;
	}
	bool XMLNode::IsElement() const
	{
		if (GetNode())
		{
			return GetNode()->ToElement() != nullptr;
		}
		return false;
	}
	bool XMLNode::IsText() const
	{
		if (GetNode())
		{
			return GetNode()->ToText() != nullptr;
		}
		return false;
	}

	// Value
	String XMLNode::GetValueText(const String& separator) const
	{
		if (auto node = GetNode())
		{
			return XML::Private::CleanText(*node, StringViewOf(separator));
		}
		return {};
	}
	bool XMLNode::IsCDATA() const
	{
		if (GetNode())
		{
			if (auto node = GetNode()->ToText())
			{
				return node->CData();
			}
		}
		return false;
	}
	bool XMLNode::SetCDATA(bool value)
	{
		if (GetNode())
		{
			if (auto node = GetNode()->ToText())
			{
				node->SetCData(value);
				return true;
			}
		}
		return false;
	}

	// Attributes
	size_t XMLNode::GetAttributeCount() const
	{
		size_t count = 0;
		if (GetNode())
		{
			if (auto node = GetNode()->ToElement())
			{
				for (const tinyxml2::XMLAttribute* attribute = node->FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
				{
					count++;
				}
			}
		}
		return count;
	}
	bool XMLNode::HasAttributes() const
	{
		if (auto node = GetNode())
		{
			if (auto element = node->ToElement())
			{
				return element->FirstAttribute() != nullptr;
			}
		}
		return false;
	}
	CallbackResult<void> XMLNode::EnumAttributeNames(CallbackFunction<String> func) const
	{
		return EnumAttributes([&](XMLAttribute attribute)
		{
			return func.Invoke(attribute.GetName()).GetLastCommand();
		});
	}
	CallbackResult<void> XMLNode::EnumAttributes(CallbackFunction<XMLAttribute> func) const
	{
		if (GetNode())
		{
			if (auto node = GetNode()->ToElement())
			{
				for (auto attribute = node->FirstAttribute(); attribute; attribute = attribute->Next())
				{
					if (func.Invoke(XMLAttribute(*this, *attribute)).ShouldTerminate())
					{
						break;
					}
				}
				return func.Finalize();
			}
		}
		return {};
	}

	bool XMLNode::HasAttribute(const String& name) const
	{
		if (GetNode())
		{
			if (auto node = GetNode()->ToElement())
			{
				auto utf8 = name.ToUTF8();
				return node->Attribute(utf8.data()) != nullptr;
			}
		}
		return false;
	}
	bool XMLNode::RemoveAttribute(const String& name)
	{
		if (GetNode())
		{
			if (auto node = GetNode()->ToElement())
			{
				auto tName = name.ToUTF8();
				node->DeleteAttribute(tName.data());
				return true;
			}
		}
		return false;
	}
	bool XMLNode::RemoveAttribute(XMLAttribute& attribute)
	{
		if (GetNode())
		{
			if (auto node = GetNode()->ToElement())
			{
				node->DeleteAttribute(attribute.GetAttribute()->Name());
				return true;
			}
		}
		return false;
	}
	bool XMLNode::ClearAttributes()
	{
		if (GetNode())
		{
			if (auto node = GetNode()->ToElement())
			{
				for (const tinyxml2::XMLAttribute* attribute = node->FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
				{
					node->DeleteAttribute(attribute->Name());
				}
				return true;
			}
		}
		return false;
	}

	// Navigation
	XMLNode XMLNode::GetElementByAttribute(const String& name, const String& value) const
	{
		if (auto node = GetNode())
		{
			if (GetAttribute(name) == value)
			{
				return XMLNode(node, *m_Document);
			}
			else
			{
				for (auto child = GetFirstChildElement(); child; child = child.GetNextSiblingElement())
				{
					if (auto foundElement = child.GetElementByAttribute(name, value))
					{
						return foundElement;
					}
				}
			}
		}
		return {};
	}
	XMLNode XMLNode::GetElementByTag(const String& tagName) const
	{
		auto node = GetNode();
		if (node && node->ToElement())
		{
			if (GetName() == tagName)
			{
				return XMLNode(node, *m_Document);
			}
			else
			{
				for (auto child = GetFirstChildElement(); child; child = child.GetNextSiblingElement())
				{
					if (child.IsElement())
					{
						if (XMLNode foundElement = child.GetElementByTag(tagName))
						{
							return foundElement;
						}
					}
				}
			}
		}
		return {};
	}
	
	XMLNode XMLNode::GetParent() const
	{
		if (auto node = GetNode())
		{
			return XMLNode(node->Parent(), *m_Document);
		}
		return {};
	}
	XMLNode XMLNode::GetPreviousSibling() const
	{
		if (auto node = GetNode())
		{
			return XMLNode(node->PreviousSibling(), *m_Document);
		}
		return {};
	}
	XMLNode XMLNode::GetPreviousSiblingElement(const String& name) const
	{
		if (auto node = GetNode())
		{
			if (name.IsEmpty())
			{
				return XMLNode(node->PreviousSiblingElement(), *m_Document);
			}
			else
			{
				auto utf8 = name.ToUTF8();
				return XMLNode(node->PreviousSiblingElement(utf8.data()), *m_Document);
			}
		}
		return {};
	}
	XMLNode XMLNode::GetNextSibling() const
	{
		if (auto node = GetNode())
		{
			return XMLNode(node->NextSibling(), *m_Document);
		}
		return {};
	}
	XMLNode XMLNode::GetNextSiblingElement(const String& name) const
	{
		if (auto node = GetNode())
		{
			if (name.IsEmpty())
			{
				return XMLNode(node->NextSiblingElement(), *m_Document);
			}
			else
			{
				auto utf8 = name.ToUTF8();
				return XMLNode(node->NextSiblingElement(utf8.data()), *m_Document);
			}
		}
		return {};
	}
	XMLNode XMLNode::GetFirstChild() const
	{
		if (auto node = GetNode())
		{
			return XMLNode(node->FirstChild(), *m_Document);
		}
		return {};
	}
	XMLNode XMLNode::GetFirstChildElement(const String& name) const
	{
		if (auto node = GetNode())
		{
			if (name.IsEmpty())
			{
				return XMLNode(node->FirstChildElement(), *m_Document);
			}
			else
			{
				auto utf8 = name.ToUTF8();
				return XMLNode(node->FirstChildElement(utf8.data()), *m_Document);
			}
		}
		return {};
	}
	XMLNode XMLNode::GetLastChild() const
	{
		if (auto node = GetNode())
		{
			return XMLNode(node->LastChild(), *m_Document);
		}
		return {};
	}
	XMLNode XMLNode::GetLastChildElement(const String& name) const
	{
		if (auto node = GetNode())
		{
			if (name.IsEmpty())
			{
				return XMLNode(node->LastChildElement(), *m_Document);
			}
			else
			{
				auto utf8 = name.ToUTF8();
				return XMLNode(node->LastChildElement(utf8.data()), *m_Document);
			}
		}
		return {};
	}

	// Insertion
	bool XMLNode::Insert(XMLNode& node, InsertMode insertMode)
	{
		switch (insertMode)
		{
			case InsertMode::AfterChild:
			{
				return InsertAfterChild(node);
			}
			case InsertMode::AsFirstChild:
			{
				return InsertFirstChild(node);
			}
			case InsertMode::AsLastChild:
			{
				return InsertLastChild(node);
			}
		};
		return false;
	}
	bool XMLNode::InsertAfterChild(XMLNode& newNode)
	{
		auto thisTxNode = GetNode();
		auto newTxNode = newNode.GetNode();

		if (thisTxNode && newTxNode)
		{
			return thisTxNode->InsertAfterChild(thisTxNode, newTxNode) != nullptr;
		}
		return false;
	}
	bool XMLNode::InsertFirstChild(XMLNode& newNode)
	{
		auto thisTxNode = GetNode();
		auto newTxNode = newNode.GetNode();

		if (thisTxNode && newTxNode)
		{
			return thisTxNode->InsertFirstChild(newTxNode) != nullptr;
		}
		return false;
	}
	bool XMLNode::InsertLastChild(XMLNode& newNode)
	{
		auto thisTxNode = GetNode();
		auto newTxNode = newNode.GetNode();

		if (thisTxNode && newTxNode)
		{
			return thisTxNode->InsertEndChild(newTxNode) != nullptr;
		}
		return false;
	}

	XMLNode XMLNode::NewElement(const String& name, InsertMode insertMode)
	{
		if (m_Document)
		{
			XMLNode node = m_Document->CreateElement(name);
			if (node && Insert(node, insertMode))
			{
				return node;
			}
		}
		return {};
	}
	XMLNode XMLNode::NewComment(const String& value, InsertMode insertMode)
	{
		if (m_Document)
		{
			XMLNode node = m_Document->CreateComment(value);
			if (node && Insert(node, insertMode))
			{
				return node;
			}
		}
		return {};
	}
	XMLNode XMLNode::NewText(const String& value, InsertMode insertMode)
	{
		if (m_Document)
		{
			XMLNode node = m_Document->CreateText(value);
			if (node && Insert(node, insertMode))
			{
				return node;
			}
		}
		return {};
	}
	XMLNode XMLNode::NewDeclaration(const String& value, InsertMode insertMode)
	{
		if (m_Document)
		{
			XMLNode node = m_Document->CreateDeclaration(value);
			if (node && Insert(node, insertMode))
			{
				return node;
			}
		}
		return {};
	}
	XMLNode XMLNode::NewUnknown(const String& value, InsertMode insertMode)
	{
		if (m_Document)
		{
			XMLNode node = m_Document->CreateUnknown(value);
			if (node && Insert(node, insertMode))
			{
				return node;
			}
		}
		return {};
	}
}
