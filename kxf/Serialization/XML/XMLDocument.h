#pragma once
#include "../Common.h"
#include "../XDocument.h"
#include "../BinarySerializer.h"
#include "kxf/Core/ILibraryInfo.h"
#include "kxf/IO/IStream.h"
#include "TinyXML2.h"
#include <wx/stream.h>

namespace kxf
{
	class XMLNode;
	class XMLDocument;
	class XMLAttribute;
}

namespace kxf::XML
{
	enum class NodeType
	{
		None = -1,

		Document,
		Element,
		Text,
		Declaration,
		Comment,
		Unknown,
	};
	enum class SerializationFormat
	{
		Default,
		HTML5,
	};
	enum class InsertMode
	{
		AfterChild,
		AsFirstChild,
		AsLastChild,
	};
}

namespace kxf
{
	class KX_API XMLAttribute final
	{
		friend class XMLNode;

		private:
			XMLNode* m_Node = nullptr;
			tinyxml2::XMLAttribute* m_Attribute = nullptr;

		private:
			tinyxml2::XMLAttribute* GetAttribute()
			{
				return m_Attribute;
			}

		public:
			XMLAttribute() = default;

		private:
			XMLAttribute(XMLNode& node, tinyxml2::XMLAttribute& attribute)
				:m_Node(&node), m_Attribute(&attribute)
			{
			}
			XMLAttribute(const XMLNode& node, const tinyxml2::XMLAttribute& attribute)
				:m_Node(const_cast<XMLNode*>(&node)), m_Attribute(const_cast<tinyxml2::XMLAttribute*>(&attribute))
			{
			}

		public:
			bool IsNull() const
			{
				return !m_Node || !m_Attribute;
			}
			XMLNode GetNode() const;
			XMLDocument* GetDocument() const;

			String GetName() const;
			bool SetName(const String& name) = delete;

			String GetValue() const;
			bool SetValue(const String& name) = delete;

			XMLAttribute Next() const;

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}
	};
}

namespace kxf
{
	class KX_API XMLNode: public XDocument::XNode<XMLNode>
	{
		friend class XMLDocument;
		friend class XMLAttribute;

		public:
			using NodeType = XML::NodeType;
			using InsertMode = XML::InsertMode;
			using SerializationFormat = XML::SerializationFormat;

		private:
			XMLDocument* m_Document = nullptr;
			tinyxml2::XMLNode* m_Node = nullptr;

		private:
			XMLNode ConstructOrQueryElement(const String& xPath, bool allowCreate);

		protected:
			const tinyxml2::XMLNode* GetNode() const
			{
				return m_Node;
			}
			tinyxml2::XMLNode* GetNode()
			{
				return m_Node;
			}

		protected:
			std::optional<String> DoGetValue() const override;
			std::optional<int64_t> DoGetValueIntWithBase(int base) const override;
			std::optional<double> DoGetValueFloat() const override;
			std::optional<bool> DoGetValueBool() const override;
			bool DoSetValue(const String& value, WriteEmpty writeEmpty, AsCDATA asCDATA) override;

			std::optional<String> DoGetAttribute(const String& name) const override;
			std::optional<int64_t> DoGetAttributeIntWithBase(const String& name, int base) const override;
			std::optional<double> DoGetAttributeFloat(const String& name) const override;
			std::optional<bool> DoGetAttributeBool(const String& name) const override;
			bool DoSetAttribute(const String& name, const String& value, WriteEmpty writeEmpty) override;

		protected:
			XMLNode(tinyxml2::XMLNode* node, XMLDocument& document)
				:m_Node(node), m_Document(&document)
			{
			}
			XMLNode(const tinyxml2::XMLNode* node, XMLDocument& document)
				:m_Node(const_cast<tinyxml2::XMLNode*>(node)), m_Document(&document)
			{
			}

		public:
			XMLNode() = default;
			XMLNode(const XMLNode&) = default;

		public:
			// General
			bool IsNull() const override
			{
				return !m_Node || !m_Document;
			}
			String GetXPath() const override;
			XMLNode QueryElement(const String& xPath) const override;
			XMLNode ConstructElement(const String& xPath) override;

			String GetXPathIndexSeparator() const override;
			void SetXPathIndexSeparator(const String& value) override;

			// Node
			size_t GetIndexWithinParent() const override;
			String GetName() const override;
			bool SetName(const String& name) override;

			size_t GetChildrenCount() const override;
			bool HasChildren() const override;
			bool ClearChildren() override;
			bool ClearNode() override;

			size_t EnumChildren(CallbackFunction<XMLNode> func) const override;
			size_t EnumChildElements(CallbackFunction<XMLNode> func, const String& name = {}) const;

			XMLDocument& GetDocument()
			{
				return *m_Document;
			}
			const XMLDocument& GetDocument() const
			{
				return *m_Document;
			}

			virtual String GetXML(SerializationFormat mode = SerializationFormat::Default) const;
			NodeType GetType() const;
			bool IsElement() const;
			bool IsText() const;
		
			// Value
			String GetValueText(const String& separator = {}) const;
		
			bool IsCDATA() const override;
			bool SetCDATA(bool value) override;

			// Attributes
			size_t GetAttributeCount() const override;
			bool HasAttributes() const override;
			size_t EnumAttributeNames(CallbackFunction<String> func) const override;
			size_t EnumAttributes(CallbackFunction<XMLAttribute> func) const;

			bool HasAttribute(const String& name) const override;
			bool RemoveAttribute(const String& name) override;
			bool RemoveAttribute(XMLAttribute& attribute);
			bool ClearAttributes() override;
		
			// Navigation
			XMLNode GetElementByAttribute(const String& name, const String& value) const override;
			XMLNode GetElementByTag(const String& tagName) const override;

			XMLNode GetParent() const override;
			XMLNode GetPreviousSibling() const override;
			XMLNode GetPreviousSiblingElement(const String& name = {}) const;
			XMLNode GetNextSibling() const override;
			XMLNode GetNextSiblingElement(const String& name = {}) const;
			XMLNode GetFirstChild() const override;
			XMLNode GetFirstChildElement(const String& name = {}) const;
			XMLNode GetLastChild() const override;
			XMLNode GetLastChildElement(const String& name = {}) const;

			// Insertion
			bool Insert(XMLNode& node, InsertMode insertMode);
			bool InsertAfterChild(XMLNode& newNode);
			bool InsertFirstChild(XMLNode& newNode);
			bool InsertLastChild(XMLNode& newNode);

			XMLNode NewElement(const String& name, InsertMode insertMode = InsertMode::AsLastChild);
			XMLNode NewComment(const String& value, InsertMode insertMode = InsertMode::AsLastChild);
			XMLNode NewText(const String& value, InsertMode insertMode = InsertMode::AsLastChild);
			XMLNode NewDeclaration(const String& value, InsertMode insertMode = InsertMode::AsLastChild);
			XMLNode NewUnknown(const String& value, InsertMode insertMode = InsertMode::AsLastChild);
	};
}

namespace kxf
{
	class KX_API XMLDocument final: public XMLNode, public IObject
	{
		friend class XMLNode;

		private:
			tinyxml2::XMLDocument m_Document;
			String m_DeclaredEncoding;
			String m_XPathIndexSeparator;

		private:
			tinyxml2::XMLDocument* GetDocument()
			{
				return &m_Document;
			}
			void ReplaceDeclaration();
			
			void Init();
			void DoLoad(const char* xml, size_t length);
			void DoUnload();

			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override;

		private:
			XMLNode CreateElement(const String& name);
			XMLNode CreateComment(const String& value);
			XMLNode CreateText(const String& value);
			XMLNode CreateDeclaration(const String& value);
			XMLNode CreateUnknown(const String& value);

		private:
			XMLDocument(const String& xml, const String& declaredEncoding, const String& xPathSeparator)
				:XMLDocument()
			{
				m_DeclaredEncoding = declaredEncoding;
				m_XPathIndexSeparator = xPathSeparator;
				Load(xml);
			}

		public:
			XMLDocument()
				:XMLNode(&m_Document, *this)
			{
				Init();
			}
			XMLDocument(const String& xml)
				:XMLDocument()
			{
				Load(xml);
			}
			XMLDocument(std::string_view xml)
				:XMLDocument()
			{
				Load(xml);
			}
			XMLDocument(std::wstring_view xml)
				:XMLDocument()
			{
				Load(xml);
			}
			XMLDocument(const char* xml, size_t length = String::npos)
				:XMLDocument()
			{
				Load(std::string_view(xml, length));
			}
			XMLDocument(const wchar_t* xml, size_t length = String::npos)
				:XMLDocument()
			{
				Load(std::wstring_view(xml, length));
			}
			XMLDocument(IInputStream& stream)
				:XMLDocument()
			{
				Load(stream);
			}
			XMLDocument(const XMLDocument&) = delete;
			XMLDocument(XMLDocument&&) = delete;

		public:
			// XMLNode: General
			bool IsNull() const override
			{
				return m_Document.Error() || !m_Document.FirstChild();
			}
			String GetXPath() const override
			{
				return {};
			}
			String GetXML(SerializationFormat mode = SerializationFormat::Default) const override;

			bool Load(const String& xml);
			bool Load(const char* xml, size_t length = String::npos)
			{
				return Load(std::string_view(xml, length));
			}
			bool Load(const wchar_t* xml, size_t length = String::npos)
			{
				return Load(std::wstring_view(xml, length));
			}
			bool Load(std::string_view xml);
			bool Load(std::wstring_view xml);
			bool Load(IInputStream& stream);
			bool Save(IOutputStream& stream) const;
			String Save() const
			{
				return GetXML();
			}
			XMLDocument Clone() const
			{
				return XMLDocument(GetXML(), m_DeclaredEncoding, m_XPathIndexSeparator);
			}

			String GetDeclaredEncoding() const
			{
				return m_DeclaredEncoding;
			}
			void SetDeclaredEncoding(const String& value)
			{
				m_DeclaredEncoding = value;
			}

			String GetXPathIndexSeparator() const override
			{
				return m_XPathIndexSeparator;
			}
			void SetXPathIndexSeparator(const String& value) override
			{
				m_XPathIndexSeparator = value.IsEmpty() ? IXNode::GetXPathIndexSeparator() : value;
			}

			// XMLNode: Deletion
			bool RemoveNode(XMLNode& node);

		public:
			XMLDocument& operator=(const XMLDocument&) = delete;
			XMLDocument& operator=(XMLDocument&&) = delete;
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<XMLDocument> final
	{
		uint64_t Serialize(IOutputStream& stream, const XMLDocument& value) const
		{
			return BinarySerializer<String>().Serialize(stream, value.Save());
		}
		uint64_t Deserialize(IInputStream& stream, XMLDocument& value) const
		{
			String buffer;
			auto read = BinarySerializer<String>().Deserialize(stream, buffer);

			value.Load(buffer);
			return read;
		}
	};
}
