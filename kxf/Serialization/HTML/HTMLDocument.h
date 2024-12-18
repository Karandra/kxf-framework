#pragma once
#include "../Common.h"
#include "../XDocument.h"
#include "kxf/Core/Version.h"
#include "kxf/IO/IStream.h"

namespace kxf::HTML
{
	enum class NodeType;
	enum class TagType;
}

namespace kxf
{
	class HTMLDocument;
}

namespace kxf
{
	class KXF_API HTMLNode: public XDocument::XNode<HTMLNode>
	{
		friend class HTMLDocument;

		public:
			using NodeType = HTML::NodeType;
			using TagType = HTML::TagType;

		private:
			virtual const void* GetNode() const
			{
				return m_Node;
			}
			virtual void SetNode(void* node)
			{
				m_Node = node;
			}

		private:
			const void* m_Node = nullptr;
			const HTMLDocument* m_Document = nullptr;

		protected:
			std::optional<String> DoGetValue() const override;
			bool DoSetValue(const String& value, WriteEmpty writeEmpty, AsCDATA asCDATA) override;
		
			std::optional<String> DoGetAttribute(const String& name) const override;
			bool DoSetAttribute(const String& name, const String& value, WriteEmpty writeEmpty) override;

		public:
			HTMLNode()
				:m_Document(nullptr), m_Node(nullptr)
			{
			}
			HTMLNode(const HTMLNode&) = default;

		protected:
			HTMLNode(const void* node, const HTMLDocument* document)
				:m_Node(node), m_Document(document)
			{
			}

		public:
			// General
			bool IsNull() const override
			{
				return !m_Node || !m_Document;
			}
			HTMLNode QueryElement(const String& XPath) const override;
			HTMLNode ConstructElement(const String& XPath) override;

			// Node
			size_t GetIndexWithinParent() const override;
			String GetName() const override;

			size_t GetChildrenCount() const override;
			CallbackResult<void> EnumChildren(CallbackFunction<HTMLNode> func) const override;

			virtual String GetHTML() const;
			NodeType GetType() const;
			TagType GetTagType() const;

			const HTMLDocument& GetDocumentNode() const
			{
				return *m_Document;
			}
			bool IsFullNode() const;

			// Value
			String GetValueText() const;

			// Attributes
			size_t GetAttributeCount() const override;
			CallbackResult<void> EnumAttributeNames(CallbackFunction<String> func) const override;
			bool HasAttribute(const String& name) const override;
		
			// Navigation
			HTMLNode GetElementByAttribute(const String& name, const String& value) const override;
			HTMLNode GetElementByID(const String& id) const
			{
				return GetElementByAttribute("id", id);
			}
			HTMLNode GetElementByClass(const String & className) const
			{
				return GetElementByAttribute("class", className);
			}
			HTMLNode GetElementByTag(TagType tagType) const;
			HTMLNode GetElementByTag(const String& tagName) const override;
		
			HTMLNode GetParent() const override;
			HTMLNode GetPreviousSibling() const override;
			HTMLNode GetNextSibling() const override;
			HTMLNode GetFirstChild() const override;
			HTMLNode GetLastChild() const override;
	};
}

namespace kxf
{
	class KXF_API HTMLDocument: public HTMLNode
	{
		private:
			std::vector<uint8_t> m_Buffer;
			void* m_ParserOutput = nullptr;
			void* m_ParserOptions = nullptr;

		private:
			void Init();
			void DoLoad();
			void DoUnload();
			void Destroy();

			const void* GetNode() const override;
			void SetNode(void* node) override;

		public:
			HTMLDocument()
				:HTMLNode(nullptr, this)
			{
				Init();
			}
			HTMLDocument(const String& html)
				:HTMLDocument()
			{
				if (!html.IsEmpty())
				{
					Load(html);
				}
			}
			HTMLDocument(IInputStream& stream)
				:HTMLDocument()
			{
				Load(stream);
			}
			HTMLDocument(const HTMLDocument&) = delete;
			HTMLDocument(HTMLDocument&& other) noexcept
				:HTMLDocument()
			{
				*this = std::move(other);
			}
			~HTMLDocument()
			{
				Destroy();
			}

		public:
			bool IsNull() const override;

			bool Load(const String& htmlText);
			bool Load(IInputStream& stream);
			bool Save(IOutputStream& stream) const;
			String Save() const
			{
				return GetHTML();
			}
			HTMLDocument Clone() const
			{
				return HTMLDocument(GetHTML());
			}

		public:
			HTMLDocument& operator=(const HTMLDocument&) = delete;
			HTMLDocument& operator=(HTMLDocument&& other) noexcept
			{
				m_Buffer = std::move(other.m_Buffer);

				m_ParserOutput = other.m_ParserOutput;
				other.m_ParserOutput = nullptr;

				m_ParserOptions = other.m_ParserOptions;
				other.m_ParserOptions = nullptr;

				return *this;
			}
	};
}

namespace kxf::HTML
{
	enum class NodeType
	{
		None = -1,

		Document = 0, // GumboNodeType::GUMBO_NODE_DOCUMENT,
		Element = 1, // GumboNodeType::GUMBO_NODE_ELEMENT,
		Text = 2, // GumboNodeType::GUMBO_NODE_TEXT,
		CData = 3, // GumboNodeType::GUMBO_NODE_CDATA,
		Comment = 4, // GumboNodeType::GUMBO_NODE_COMMENT,
		NodeTemplate = 5, // GumboNodeType::GUMBO_NODE_TEMPLATE,
		NodeWhitespace = 6, // GumboNodeType::GUMBO_NODE_WHITESPACE,
	};
	enum class TagType
	{
		HTML,
		HEAD,
		TITLE,
		BASE,
		LINK,
		META,
		STYLE,
		SCRIPT,
		NOSCRIPT,
		TEMPLATE,
		BODY,
		ARTICLE,
		SECTION,
		NAV,
		ASIDE,
		H1,
		H2,
		H3,
		H4,
		H5,
		H6,
		HGROUP,
		HEADER,
		FOOTER,
		ADDRESS,
		P,
		HR,
		PRE,
		BLOCKQUOTE,
		OL,
		UL,
		LI,
		DL,
		DT,
		DD,
		FIGURE,
		FIGCAPTION,
		MAIN,
		DIV,
		A,
		EM,
		STRONG,
		SMALL,
		S,
		CITE,
		Q,
		DFN,
		ABBR,
		DATA,
		TIME,
		CODE,
		VAR,
		SAMP,
		KBD,
		SUB,
		SUP,
		I,
		B,
		U,
		MARK,
		RUBY,
		RT,
		RP,
		BDI,
		BDO,
		SPAN,
		BR,
		WBR,
		INS,
		DEL,
		IMAGE,
		IMG,
		IFRAME,
		EMBED,
		OBJECT,
		PARAM,
		VIDEO,
		AUDIO,
		SOURCE,
		TRACK,
		CANVAS,
		MAP,
		AREA,
		MATH,
		MI,
		MO,
		MN,
		MS,
		MTEXT,
		MGLYPH,
		MALIGNMARK,
		ANNOTATION_XML,
		SVG,
		FOREIGNOBJECT,
		DESC,
		TABLE,
		CAPTION,
		COLGROUP,
		COL,
		TBODY,
		THEAD,
		TFOOT,
		TR,
		TD,
		TH,
		FORM,
		FIELDSET,
		LEGEND,
		LABEL,
		INPUT,
		BUTTON,
		SELECT,
		DATALIST,
		OPTGROUP,
		OPTION,
		TEXTAREA,
		KEYGEN,
		OUTPUT,
		PROGRESS,
		METER,
		DETAILS,
		SUMMARY,
		MENU,
		MENUITEM,
		APPLET,
		ACRONYM,
		BGSOUND,
		DIR,
		FRAME,
		FRAMESET,
		NOFRAMES,
		ISINDEX,
		LISTING,
		XMP,
		NEXTID,
		NOEMBED,
		PLAINTEXT,
		RB,
		STRIKE,
		BASEFONT,
		BIG,
		BLINK,
		CENTER,
		FONT,
		MARQUEE,
		MULTICOL,
		NOBR,
		SPACER,
		TT,
		RTC,

		UNKNOWN,
		LAST,
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<HTMLDocument> final
	{
		uint64_t Serialize(IOutputStream& stream, const HTMLDocument& value) const
		{
			return BinarySerializer<String>().Serialize(stream, value.Save());
		}
		uint64_t Deserialize(IInputStream& stream, HTMLDocument& value) const
		{
			String buffer;
			auto read = BinarySerializer<String>().Deserialize(stream, buffer);

			value.Load(buffer);
			return read;
		}
	};
}
