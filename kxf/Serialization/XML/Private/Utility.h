#pragma once
#include "kxf/Serialization/Common.h"

namespace kxf::XML::Private
{
	constexpr char DefaultDeclaredEncoding[] = "utf-8";

	class XMLPrinterDefault: public tinyxml2::XMLPrinter
	{
		public:
			XMLPrinterDefault(FILE* file = nullptr, bool compact = false, int depth = 0)
				:XMLPrinter(file, compact, depth)
			{
			}

		protected:
			// tinyxml2::XMLPrinter
			void PrintSpace(int depth) override
			{
				for (int i = 0; i < depth; i++)
				{
					Write("\t");
				}
			}
	};

	class XMLPrinterHTML5: public XMLPrinterDefault
	{
		private:
			bool m_IsCompactMode = false;

		private:
			bool IsVoidElement(const char* name) const
			{
				// Complete list of all HTML5 "void elements": http://dev.w3.org/html5/markup/syntax.html
				constexpr const char* voidElementNames[] =
				{
					"area",
					"base",
					"br",
					"col",
					"command",
					"embed",
					"hr",
					"img",
					"input",
					"keygen",
					"link",
					"meta",
					"param",
					"source",
					"track",
					"wbr"
				};

				auto it = std::ranges::find_if(voidElementNames, [&](const char* item)
				{
					return _stricmp(item, name) == 0;
				});
				return it != std::end(voidElementNames);
			}

		public:
			XMLPrinterHTML5(FILE* file = nullptr, bool compact = false, int depth = 0)
				:XMLPrinterDefault(file, compact, depth), m_IsCompactMode(compact)
			{
			}

		protected:
			// tinyxml2::XMLPrinter
			void CloseElement(bool compactMode = false) override
			{
				if (_elementJustOpened && !IsVoidElement(_stack.PeekTop()))
				{
					SealElementIfJustOpened();
				}
				XMLPrinterDefault::CloseElement(m_IsCompactMode);
			}
	};
}

namespace kxf::XML::Private
{
	inline String ToString(const char* value, size_t length = String::npos)
	{
		return String::FromUTF8({value, length});
	}
	inline String CleanText(const tinyxml2::XMLNode& node, StringView separator = {})
	{
		String content;
		if (node.ToElement())
		{
			for (const tinyxml2::XMLNode* child = node.FirstChild(); child; child = child->NextSibling())
			{
				String text = CleanText(*child);
				if (!separator.empty() && child != &node && !text.empty())
				{
					content += separator;
				}
				content += text;
			}
		}
		else if (!node.ToDocument())
		{
			content = ToString(node.Value());
		}
		return content;
	}

	template<class TPrinter>
	String PrintDocumentUsing(const tinyxml2::XMLDocument& document)
	{
		TPrinter buffer;
		document.Print(&buffer);

		return ToString(buffer.CStr(), buffer.CStrSize() - 1);
	}

	inline String PrintDocument(const tinyxml2::XMLDocument& document, bool asHTML5)
	{
		if (asHTML5)
		{
			return PrintDocumentUsing<XMLPrinterHTML5>(document);
		}
		else
		{
			return PrintDocumentUsing<XMLPrinterDefault>(document);
		}
	}
}
