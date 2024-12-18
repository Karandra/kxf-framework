#include "kxf-pch.h"
#include "XMLDocument.h"
#include "Private/Utility.h"
#include "kxf/Network/URI.h"
#include "kxf/Utility/SoftwareLicenseDB.h"

namespace
{
	constexpr char g_Copyright[] = "Copyright© Lee Thomason";
}

namespace kxf
{
	void XMLDocument::ReplaceDeclaration()
	{
		if (m_Document.FirstChild() && m_Document.FirstChild()->ToDeclaration())
		{
			m_Document.DeleteNode(m_Document.FirstChild());
		}

		String declaration = Format(R"(xml version="1.0" encoding="{}")", m_DeclaredEncoding);
		auto utf8 = declaration.ToUTF8();
		m_Document.InsertFirstChild(m_Document.NewDeclaration(utf8.data()));
	}

	void XMLDocument::Init()
	{
		m_DeclaredEncoding = XML::Private::DefaultDeclaredEncoding;
		m_XPathIndexSeparator = XNode::GetXPathIndexSeparator();
		m_Document.SetBOM(false);
	}
	void XMLDocument::DoLoad(const char* xml, size_t length)
	{
		DoUnload();

		m_Document.Parse(xml, length);
		ReplaceDeclaration();
	}
	void XMLDocument::DoUnload()
	{
		m_Document.Clear();
	}

	RTTI::QueryInfo XMLDocument::DoQueryInterface(const IID& iid) noexcept
	{
		if (iid.IsOfType<ILibraryInfo>())
		{
			class XMLDocumentLibraryInfo final: public ILibraryInfo
			{
				public:
					// ILibraryInfo
					String GetName() const override
					{
						return "TinyXML2";
					}
					Version GetVersion() const override
					{
						return {TIXML2_MAJOR_VERSION, TIXML2_MINOR_VERSION, TIXML2_PATCH_VERSION};
					}
					URI GetHomePage() const override
					{
						return "https://github.com/leethomason/tinyxml2";
					}
					uint32_t GetAPILevel() const override
					{
						return TIXML2_MAJOR_VERSION * 1000 + TIXML2_MINOR_VERSION * 100 + TIXML2_PATCH_VERSION * 10;
					}

					String GetLicense() const override
					{
						return SoftwareLicenseDB::Get().GetText(SoftwareLicenseType::ZLib, g_Copyright);
					}
					String GetLicenseName() const override
					{
						return SoftwareLicenseDB::Get().GetName(SoftwareLicenseType::ZLib);
					}
					String GetCopyright() const override
					{
						return g_Copyright;
					}
			};

			static XMLDocumentLibraryInfo libraryInfo;
			return static_cast<ILibraryInfo&>(libraryInfo);
		}
		else if (iid.IsOfType<XMLDocument>())
		{
			return *this;
		}
		return IObject::DoQueryInterface(iid);
	}

	XMLNode XMLDocument::CreateElement(const String& name)
	{
		auto utf8 = name.ToUTF8();
		return XMLNode(m_Document.NewElement(utf8.data()), *this);
	}
	XMLNode XMLDocument::CreateComment(const String& value)
	{
		auto utf8 = value.ToUTF8();
		return XMLNode(m_Document.NewComment(utf8.data()), *this);
	}
	XMLNode XMLDocument::CreateText(const String& value)
	{
		auto utf8 = value.ToUTF8();
		return XMLNode(m_Document.NewText(utf8.data()), *this);
	}
	XMLNode XMLDocument::CreateDeclaration(const String& value)
	{
		if (!value.IsEmpty())
		{
			auto utf8 = value.ToUTF8();
			return XMLNode(m_Document.NewDeclaration(utf8.data()), *this);
		}
		else
		{
			return XMLNode(m_Document.NewDeclaration(), *this);
		}
	}
	XMLNode XMLDocument::CreateUnknown(const String& value)
	{
		auto utf8 = value.ToUTF8();
		return XMLNode(m_Document.NewUnknown(utf8.data()), *this);
	}

	// XMLNode: General
	String XMLDocument::GetXML(SerializationFormat mode) const
	{
		return XML::Private::PrintDocument(m_Document, mode == SerializationFormat::HTML5);
	}

	bool XMLDocument::Load(const String& xml)
	{
		auto utf8 = xml.ToUTF8();
		DoLoad(utf8.data(), utf8.length());
		return !IsNull();
	}
	bool XMLDocument::Load(std::string_view xml)
	{
		DoLoad(xml.data(), xml.length());
		return !IsNull();
	}
	bool XMLDocument::Load(std::wstring_view xml)
	{
		Load(String(xml));
		return !IsNull();
	}
	bool XMLDocument::Load(IInputStream& stream)
	{
		wxMemoryBuffer buffer;
		buffer.SetBufSize(stream.GetSize().ToBytes());
		stream.ReadAll(buffer.GetData(), buffer.GetBufSize());
		buffer.SetDataLen(stream.LastRead().ToBytes());

		DoLoad(reinterpret_cast<const char*>(buffer.GetData()), buffer.GetDataLen());
		return !IsNull();
	}
	bool XMLDocument::Save(IOutputStream& stream) const
	{
		XML::Private::XMLPrinterDefault buffer;
		m_Document.Print(&buffer);
		return stream.WriteAll(buffer.CStr(), buffer.CStrSize() - 1);
	}

	// XMLNode: Deletion
	bool XMLDocument::RemoveNode(XMLNode& node)
	{
		if (!IsNull() && node)
		{
			m_Document.DeleteNode(node.GetNode());
			return true;
		}
		return false;
	}
}
