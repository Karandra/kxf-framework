#include "kxf-pch.h"
#include "XMLDocument.h"

namespace kxf
{
	XMLNode XMLAttribute::GetNode() const
	{
		if (m_Node)
		{
			return *m_Node;
		}
		return {};
	}
	XMLDocument* XMLAttribute::GetDocument() const
	{
		if (m_Node)
		{
			return &m_Node->GetDocument();
		}
		return nullptr;
	}

	String XMLAttribute::GetName() const
	{
		if (!IsNull())
		{
			return String::FromUTF8(m_Attribute->Name());
		}
		return {};
	}
	String XMLAttribute::GetValue() const
	{
		if (!IsNull())
		{
			return String::FromUTF8(m_Attribute->Value());
		}
		return {};
	}

	XMLAttribute XMLAttribute::Next() const
	{
		if (!IsNull())
		{
			if (const tinyxml2::XMLAttribute* attribute = m_Attribute->Next())
			{
				return XMLAttribute(*m_Node, *attribute);
			}
		}
		return {};
	}
}
