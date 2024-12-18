#include "kxf-pch.h"
#include "HTMLDocument.h"

#pragma warning(disable: 4005) // macro redefinition
#include "gumbo.h"
#include "Private/error.h"

namespace
{
	GumboOptions* GetOptions(void* options) noexcept
	{
		return reinterpret_cast<GumboOptions*>(options);
	}
	GumboOutput* GetOutput(void* output) noexcept
	{
		return reinterpret_cast<GumboOutput*>(output);
	}
}

namespace kxf
{
	void HTMLDocument::Init()
	{
		auto options = std::make_unique<GumboOptions>(kGumboDefaultOptions);
		options->userdata = this;

		m_ParserOptions = options.release();
	}
	void HTMLDocument::DoLoad()
	{
		m_ParserOutput = gumbo_parse_with_options(GetOptions(m_ParserOptions), reinterpret_cast<const char*>(m_Buffer.data()), m_Buffer.size());
		SetNode(GetOutput(m_ParserOutput)->document);
	}
	void HTMLDocument::DoUnload()
	{
		if (m_ParserOutput)
		{
			gumbo_destroy_output(GetOptions(m_ParserOptions), GetOutput(m_ParserOutput));
		}

		SetNode(nullptr);
		m_Buffer.clear();
		m_ParserOutput = nullptr;
	}
	void HTMLDocument::Destroy()
	{
		DoUnload();

		delete GetOptions(m_ParserOptions);
		m_ParserOptions = nullptr;
	}

	const void* HTMLDocument::GetNode() const
	{
		return GetOutput(m_ParserOutput)->document;
	}
	void HTMLDocument::SetNode(void* node)
	{
		// Nothing to do
		//GetOutput(m_ParserOutput)->document = reinterpret_cast<GumboNode*>(node);
	}

	bool HTMLDocument::Load(const String& htmlText)
	{
		DoUnload();

		auto utf8 = htmlText.ToUTF8();
		m_Buffer.resize(utf8.length());
		std::memcpy(m_Buffer.data(), utf8.data(), utf8.length());

		DoLoad();
		return !IsNull();
	}
	bool HTMLDocument::Load(IInputStream& stream)
	{
		DoUnload();
		m_Buffer.resize(stream.GetSize().ToBytes());
		stream.Read(m_Buffer.data(), m_Buffer.size());
		m_Buffer.resize(stream.LastRead().ToBytes());

		DoLoad();
		return !IsNull();
	}
	bool HTMLDocument::Save(IOutputStream& stream) const
	{
		auto utf8 = GetHTML().ToUTF8();
		return stream.WriteAll(utf8.data(), utf8.length());
	}

	bool HTMLDocument::IsNull() const
	{
		if (!m_Buffer.empty() && m_ParserOutput)
		{
			if (GumboError** errors = reinterpret_cast<GumboError**>(GetOutput(m_ParserOutput)->errors.data))
			{
				for (size_t i = 0; i < GetOutput(m_ParserOutput)->errors.length; i++)
				{
					if (errors[i]->type == GUMBO_ERR_PARSER)
					{
						return true;
					}
				}
			}
			return false;
		}
		return true;
	}
}
