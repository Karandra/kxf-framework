#include "kxf-pch.h"
#include "INIDocument.h"
#include "kxf/Core/ILibraryInfo.h"
#include "kxf/IO/MemoryStream.h"
#include "kxf/Utility/SoftwareLicenseDB.h"

namespace SimpleINI
{
	#include "SimpleIni.h"
}
namespace
{
	constexpr char g_Copyright[] = "Copyright© 2006-2012 Brodie Thiesfield";
	constexpr int g_VersionMajor = 4;
	constexpr int g_VersionMinor = 19;
	constexpr int g_VersionPatch = 0;
}

namespace kxf
{
	class INIDocumentImpl final: public SimpleINI::CSimpleIniA
	{
		public:
			using SimpleINI::CSimpleIniA::CSimpleIniA;

		public:
			static INIDocumentSection ToSection(const INIDocument& document, const Entry& entry)
			{
				return INIDocumentSection(const_cast<INIDocument&>(document), String::FromUTF8(entry.pItem), String::FromUTF8(entry.pComment), entry.nOrder);
			}

			CallbackResult<void> ForEach(TNamesDepend& items, CallbackFunction<Entry&> func, SortOrder order = SortOrder::None) const
			{
				if (order != SortOrder::None)
				{
					items.sort(INIDocumentImpl::Entry::LoadOrder());
				}

				if (order == SortOrder::Ascending || order == SortOrder::None)
				{
					for (auto it = items.begin(); it != items.end(); ++it)
					{
						if (func.Invoke(*it).ShouldTerminate())
						{
							break;
						}
					}
				}
				else if (order == SortOrder::Descending)
				{
					for (auto it = items.rbegin(); it != items.rend(); ++it)
					{
						if (func.Invoke(*it).ShouldTerminate())
						{
							break;
						}
					}
				}
				return func.Finalize();
			}
			CallbackResult<void> ForEachSection(CallbackFunction<Entry&> func, SortOrder order = SortOrder::None) const
			{
				TNamesDepend items;
				GetAllSections(items);

				return ForEach(items, std::move(func), order);
			}
			CallbackResult<void> ForEachKey(CallbackFunction<Entry&> func, const String& sectionName, SortOrder order = SortOrder::None) const
			{
				TNamesDepend items;
				if (GetAllKeys(sectionName.utf8_str(), items))
				{
					return ForEach(items, std::move(func), order);
				}
				return {};
			}
			CallbackResult<void> ForEachValue(CallbackFunction<Entry&> func, const String& sectionName, const String& keyName, SortOrder order = SortOrder::None) const
			{
				TNamesDepend items;
				if (GetAllValues(sectionName.utf8_str(), keyName.utf8_str(), items))
				{
					return ForEach(items, std::move(func), order);
				}
				return {};
			}
	};
}

namespace kxf
{
	// IXNode
	std::optional<String> INIDocumentSection::DoGetValue() const
	{
		return m_Ref->IniDoGetValue(m_SectionName, {});
	}
	bool INIDocumentSection::DoSetValue(const String& value, WriteEmpty writeEmpty, AsCDATA asCDATA)
	{
		return m_Ref->IniDoSetValue(m_SectionName, {}, value, {}, writeEmpty, asCDATA);
	}

	std::optional<String> INIDocumentSection::DoGetAttribute(const String& name) const
	{
		return m_Ref->IniDoGetValue(m_SectionName, name);
	}
	bool INIDocumentSection::DoSetAttribute(const String& name, const String& value, WriteEmpty writeEmpty)
	{
		return m_Ref->IniDoSetValue(m_SectionName, name, value, {}, writeEmpty, m_AsCDATA);
	}

	bool INIDocumentSection::ClearNode()
	{
		if (m_Ref)
		{
			return m_Ref->RemoveSection(m_SectionName, true);
		}
		return false;
	}
	size_t INIDocumentSection::GetAttributeCount() const
	{
		if (m_Ref && m_Ref->m_Document)
		{
			auto count = m_Ref->m_Document->GetSectionSize(m_SectionName.utf8_str());
			return count >= 0 ? count : 0;
		}
		return 0;
	}
	CallbackResult<void> INIDocumentSection::EnumAttributeNames(CallbackFunction<String> func) const
	{
		if (m_Ref)
		{
			return m_Ref->EnumKeyNames(m_SectionName, std::move(func));
		}
		return {};
	}

	bool INIDocumentSection::HasAttribute(const String& name) const
	{
		if (m_Ref)
		{
			return m_Ref->HasValue(m_SectionName, name);
		}
		return false;
	}
	bool INIDocumentSection::RemoveAttribute(const String& name)
	{
		if (m_Ref)
		{
			return m_Ref->RemoveValue(m_SectionName, name);
		}
		return false;
	}
	bool INIDocumentSection::ClearAttributes()
	{
		if (m_Ref)
		{
			return m_Ref->RemoveSection(m_SectionName, false);
		}
		return false;
	}

	// INIDocumentSection
	CallbackResult<void> INIDocumentSection::EnumKeyNames(CallbackFunction<String> func) const
	{
		if (m_Ref)
		{
			return m_Ref->EnumKeyNames(m_SectionName, std::move(func));
		}
		return {};
	}
}

namespace kxf
{
	// IObject
	RTTI::QueryInfo INIDocument::DoQueryInterface(const IID& iid) noexcept
	{
		if (iid.IsOfType<ILibraryInfo>())
		{
			class INIDocumentLibraryInfo final: public ILibraryInfo
			{
				public:
					// ILibraryInfo
					String GetName() const override
					{
						return "SimpleINI";
					}
					Version GetVersion() const override
					{
						return {g_VersionMajor, g_VersionMinor, g_VersionPatch};
					}
					uint32_t GetAPILevel() const override
					{
						return g_VersionMajor * 1000 + g_VersionMinor * 100 + g_VersionPatch * 10;
					}
					URI GetHomePage() const override
					{
						return "https://github.com/brofield/simpleini";
					}

					String GetLicense() const override
					{
						return SoftwareLicenseDB::Get().GetText(SoftwareLicenseType::MIT, g_Copyright);
					}
					String GetLicenseName() const override
					{
						return SoftwareLicenseDB::Get().GetName(SoftwareLicenseType::MIT);
					}
					String GetCopyright() const override
					{
						return g_Copyright;
					}
			};

			static INIDocumentLibraryInfo libraryInfo;
			return static_cast<ILibraryInfo&>(libraryInfo);
		}
		else if (iid.IsOfType<INIDocument>())
		{
			return *this;
		}
		return IObject::DoQueryInterface(iid);
	}

	// INIDocument
	void INIDocument::Init()
	{
		m_Document = std::make_unique<INIDocumentImpl>();
		m_Document->SetAllowKeyOnly(false);
		m_Document->SetUnicode(true);
		m_Document->SetMultiLine(false);
		SetOptions(m_Options);
	}
	bool INIDocument::DoLoad(const char* ini, size_t length)
	{
		if (!m_Document)
		{
			Init();
		}
		return m_Document->LoadData(ini, length) == SimpleINI::SI_OK;
	}
	void INIDocument::DoUnload()
	{
		if (m_Document)
		{
			m_Document->Reset();
		}
	}

	std::optional<String> INIDocument::IniDoGetValue(const String& sectionName, const String& keyName, String* comment) const
	{
		if (m_Document && !keyName.IsEmptyOrWhitespace())
		{
			const auto options = GetOptions();
			if (options.Contains(INIDocumentOption::InlineComments) && StartsWithInlineComment(keyName))
			{
				return {};
			}

			String keyName2 = keyName;
			keyName2.TrimBoth();
			if (options.Contains(INIDocumentOption::Quotes))
			{
				RemoveQuotes(keyName2);
			}

			std::optional<String> value;
			if (comment)
			{
				// Single key only for now
				m_Document->ForEachValue([&](const INIDocumentImpl::Entry& entry)
				{
					value = String::FromUTF8(entry.pItem);
					*comment = String::FromUTF8(entry.pComment);

					return CallbackCommand::Terminate;
				}, sectionName, keyName2);
			}
			else
			{
				if (const char* ptr = m_Document->GetValue(sectionName.utf8_str(), keyName2.utf8_str(), nullptr))
				{
					value = String::FromUTF8(ptr);
				}
			}

			if (value && !value->IsEmpty())
			{
				if (options.Contains(INIDocumentOption::InlineComments))
				{
					RemoveInlineComments(*value, comment);
				}
				if (options.Contains(INIDocumentOption::Quotes))
				{
					RemoveQuotes(*value);
				}
			}
			return value;
		}
		return {};
	}
	bool INIDocument::IniDoSetValue(const String& sectionName, const String& keyName, const String& value, const String& comment, WriteEmpty writeEmpty, AsCDATA asCDATA)
	{
		if (writeEmpty == WriteEmpty::Never && value.IsEmpty())
		{
			return false;
		}
		else if (keyName.IsEmptyOrWhitespace())
		{
			return false;
		}
		else
		{
			if (!m_Document)
			{
				Init();
			}

			String keyName2 = keyName;
			keyName2.TrimBoth();

			auto status = m_Document->SetValue(sectionName.utf8_str(), keyName2.utf8_str(), value.utf8_str(), !comment.IsEmpty() ? comment.utf8_str() : nullptr, true);
			return status == SimpleINI::SI_UPDATED || status == SimpleINI::SI_INSERTED;
		}
	}

	bool INIDocument::RemoveQuotes(String& value) const
	{
		constexpr auto quote1 = '\"';
		constexpr auto quote2 = '\'';

		size_t count = 0;
		auto itL = value.begin();
		auto itR = value.rbegin();
		for (; itL != value.end() && itR != value.rend(); ++itL, ++itR)
		{
			if ((*itL == quote1 && *itR == quote1) || (*itL == quote2 && *itR == quote2))
			{
				count++;
			}
			else
			{
				break;
			}
		}

		if (count != 0)
		{
			value.Remove(0, count);
			value.RemoveRight(count);

			return true;
		}
		return false;
	}
	bool INIDocument::RemoveInlineComments(String& value, String* comment) const
	{
		size_t length = 1;
		size_t index = value.ReverseFind(';');
		if (index == npos)
		{
			index = value.ReverseFind('#');
			length = 1;
		}
		if (index == npos)
		{
			index = value.ReverseFind(kxfS("//"));
			length = 2;
		}

		if (index != npos)
		{
			if (comment)
			{
				*comment = value.SubMid(index + length);
			}
			value.Truncate(index);
			value.TrimRight();

			return true;
		}
		return false;
	}
	bool INIDocument::StartsWithInlineComment(const String& value) const
	{
		if (!value.IsEmpty())
		{
			auto c = value.front();
			if (c == ';' || c == '#')
			{
				return true;
			}
			else if (value.length() >= 2 && c == '/' && value[1] == '/')
			{
				return true;
			}
		}
		return false;
	}

	INIDocument::INIDocument()
		:INIDocumentSection(*this, {})
	{
		Init();
	}
	INIDocument::INIDocument(INIDocument&& other) noexcept
	{
		*this = std::move(*this);
	}
	INIDocument::~INIDocument() = default;

	// IXNode
	bool INIDocument::IsNull() const
	{
		return !m_Document || m_Document->IsEmpty();
	}

	size_t INIDocument::GetChildrenCount() const
	{
		if (m_Document)
		{
			INIDocumentImpl::TNamesDepend sections;
			m_Document->GetAllSections(sections);

			return sections.size();
		}
		return 0;
	}
	bool INIDocument::HasChildren() const
	{
		return m_Document && !m_Document->IsEmpty();
	}
	bool INIDocument::ClearChildren()
	{
		if (m_Document)
		{
			m_Document->ForEachSection([&](const INIDocumentImpl::Entry& entry)
			{
				m_Document->Delete(entry.pItem, nullptr, true);
				return CallbackCommand::Continue;
			});
			return true;
		}
		return {};
	}
	bool INIDocument::ClearNode()
	{
		if (m_Document)
		{
			m_Document->Reset();
			return true;
		}
		return false;
	}

	// XNode
	INIDocumentSection INIDocument::QueryElement(const String& XPath) const
	{
		if (m_Document)
		{
			INIDocumentSection section;
			m_Document->ForEachSection([&](const INIDocumentImpl::Entry& entry)
			{
				if (entry.pItem == XPath)
				{
					section = INIDocumentImpl::ToSection(*this, entry);
					return CallbackCommand::Terminate;
				}
				return CallbackCommand::Continue;
			});
			return section;
		}
		return {};
	}
	INIDocumentSection INIDocument::ConstructElement(const String& XPath)
	{
		return INIDocumentSection(*this, XPath);
	}

	CallbackResult<void> INIDocument::EnumChildren(CallbackFunction<INIDocumentSection> func) const
	{
		if (m_Document)
		{
			return m_Document->ForEachSection([&](const INIDocumentImpl::Entry& entry)
			{
				return func.Invoke(INIDocumentImpl::ToSection(*this, entry)).GetLastCommand();
			}, SortOrder::Ascending);
		}
		return {};
	}
	INIDocumentSection INIDocument::GetFirstChild() const
	{
		if (m_Document)
		{
			INIDocumentSection section;
			m_Document->ForEachSection([&](const INIDocumentImpl::Entry& entry)
			{
				section = INIDocumentImpl::ToSection(*this, entry);
				return CallbackCommand::Terminate;
			}, SortOrder::Ascending);
			return section;
		}
		return {};
	}
	INIDocumentSection INIDocument::GetLastChild() const
	{
		if (m_Document)
		{
			INIDocumentSection section;
			m_Document->ForEachSection([&](const INIDocumentImpl::Entry& entry)
			{
				section = INIDocumentImpl::ToSection(*this, entry);
				return CallbackCommand::Terminate;
			}, SortOrder::Descending);
			return section;
		}
		return {};
	}

	// INIDocument
	bool INIDocument::Load(const String& ini)
	{
		DoUnload();

		if (!ini.IsEmpty())
		{
			auto utf8 = ini.ToUTF8();
			return DoLoad(utf8.data(), utf8.size());
		}
		return false;
	}
	bool INIDocument::Load(IInputStream& stream)
	{
		DoUnload();

		MemoryOutputStream memoryStream;
		memoryStream.Write(stream);
		auto& buffer = memoryStream.GetStreamBuffer();

		return DoLoad(reinterpret_cast<const char*>(buffer.GetBufferStart()), buffer.GetBufferSize());
	}
	bool INIDocument::Load(std::span<const char8_t> utf8Data)
	{
		DoUnload();

		if (!utf8Data.empty())
		{
			return DoLoad(reinterpret_cast<const char*>(utf8Data.data()), utf8Data.size_bytes());
		}
		return false;
	}
	bool INIDocument::Save(IOutputStream& stream) const
	{
		if (m_Document)
		{
			std::string buffer;
			m_Document->Save(buffer, false);

			return stream.WriteAll(buffer.data(), buffer.size());
		}
		return false;
	}
	String INIDocument::Save() const
	{
		if (m_Document)
		{
			std::string buffer;
			m_Document->Save(buffer, false);

			return String::FromUTF8(buffer);
		}
		return {};
	}
	INIDocument INIDocument::Clone() const
	{
		if (m_Document)
		{
			std::string buffer;
			m_Document->Save(buffer, false);

			INIDocument document;
			document.DoLoad(buffer.data(), buffer.size());
			return document;
		}
		return {};
	}

	FlagSet<INIDocumentOption> INIDocument::GetOptions() const
	{
		if (m_Document)
		{
			FlagSet<INIDocumentOption> options = m_Options;
			options.Add(INIDocumentOption::IgnoreCase);
			options.Mod(INIDocumentOption::Spaces, m_Document->UsingSpaces());
			options.Mod(INIDocumentOption::Quotes, m_Document->UsingQuotes());
			options.Mod(INIDocumentOption::MultiKey, m_Document->IsMultiKey());

			return options;
		}
		return {};
	}
	void INIDocument::SetOptions(FlagSet<INIDocumentOption> options)
	{
		if (m_Document)
		{
			m_Options = options;
			m_Document->SetSpaces(options.Contains(INIDocumentOption::Spaces));
			m_Document->SetQuotes(options.Contains(INIDocumentOption::Quotes));
			m_Document->SetMultiKey(options.Contains(INIDocumentOption::MultiKey));
		}
	}

	CallbackResult<void> INIDocument::EnumSectionNames(CallbackFunction<String> func) const
	{
		if (m_Document)
		{
			return m_Document->ForEachSection([&, options = GetOptions()](const INIDocumentImpl::Entry& entry)
			{
				return func.Invoke(String::FromUTF8(entry.pItem)).GetLastCommand();
			}, SortOrder::Ascending);
		}
		return {};
	}
	CallbackResult<void> INIDocument::EnumKeyNames(const String& sectionName, CallbackFunction<String> func) const
	{
		if (m_Document)
		{
			return m_Document->ForEachKey([&, options = GetOptions()](const INIDocumentImpl::Entry& entry)
			{
				auto keyName = String::FromUTF8(entry.pItem);
				if (options.Contains(INIDocumentOption::InlineComments) && StartsWithInlineComment(keyName))
				{
					return CallbackCommand::Discard;
				}
				if (options.Contains(INIDocumentOption::Quotes))
				{
					RemoveQuotes(keyName);
				}

				return func.Invoke(std::move(keyName)).GetLastCommand();
			}, sectionName, SortOrder::Ascending);
		}
		return {};
	}

	bool INIDocument::HasSection(const String& sectionName) const
	{
		if (m_Document)
		{
			return m_Document->GetSection(sectionName.utf8_str()) != nullptr;
		}
		return false;
	}
	bool INIDocument::HasValue(const String& sectionName, const String& keyName)  const
	{
		if (m_Document)
		{
			return m_Document->GetValue(sectionName.utf8_str(), keyName.utf8_str()) != nullptr;
		}
		return false;
	}

	bool INIDocument::RemoveSection(const String& sectionName, bool removeEmpty)
	{
		if (m_Document)
		{
			return m_Document->Delete(sectionName.utf8_str(), nullptr, removeEmpty);
		}
		return false;
	}
	bool INIDocument::RemoveValue(const String& sectionName, const String& keyName, bool removeEmpty)
	{
		if (m_Document)
		{
			return m_Document->Delete(sectionName.utf8_str(), keyName.utf8_str(), removeEmpty);
		}
		return false;
	}

	INIDocument& INIDocument::operator=(INIDocument&& other) noexcept
	{
		static_cast<INIDocumentSection&>(*this) = std::move(other);
		m_Ref = this;
		m_Document = std::move(other.m_Document);

		return *this;
	}
}
