#pragma once
#include "../Common.h"
#include "../XDocument.h"
#include "kxf/Core/Version.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/IO/IStream.h"

namespace kxf
{
	class INIDocument;
	class INIDocumentImpl;
	class INIDocumentSection;

	enum class INIDocumentOption: uint32_t
	{
		None = 0,

		Spaces = 1u << 0,
		Quotes = 1u << 1,
		MultiKey = 1u << 2,
		InlineComments = 1u << 3,
		IgnoreCase = 1u << 4
	};
	kxf_FlagSet_Declare(INIDocumentOption);
}

namespace kxf
{
	class KXF_API INIDocumentSection: public XDocument::XNode<INIDocumentSection>
	{
		friend class INIDocument;

		private:
			INIDocument* m_Ref = nullptr;
			AsCDATA m_AsCDATA = AsCDATA::Auto;
			String m_SectionName;
			String m_Comment;
			size_t m_Index = npos;

		protected:
			// IXNode
			std::optional<String> DoGetValue() const override;
			bool DoSetValue(const String& value, WriteEmpty writeEmpty, AsCDATA asCDATA) override;

			std::optional<String> DoGetAttribute(const String& name) const override;
			bool DoSetAttribute(const String& name, const String& value, WriteEmpty writeEmpty) override;

		public:
			INIDocumentSection() = default;
			INIDocumentSection(INIDocument& document, String sectionName, String comment = {}, size_t index = npos)
				:m_Ref(&document), m_SectionName(std::move(sectionName)), m_Comment(std::move(comment)), m_Index(index)
			{
			}
			INIDocumentSection(const INIDocumentSection&) = default;
			INIDocumentSection(INIDocumentSection&& other) noexcept
			{
				*this = std::move(other);
			}

		public:
			// IXNode
			bool IsNull() const override
			{
				return m_Ref == nullptr;
			}
			String GetXPath() const override
			{
				return m_SectionName;
			}

			bool IsCDATA() const override
			{
				return m_AsCDATA == AsCDATA::Always;
			}
			bool SetCDATA(bool value) override
			{
				m_AsCDATA = value ? AsCDATA::Always : AsCDATA::Never;
				return true;
			}
			void SetCDATA(AsCDATA asCDATA) noexcept
			{
				m_AsCDATA = asCDATA;
			}

			size_t GetIndexWithinParent() const override
			{
				return m_Index;
			}
			String GetName() const override
			{
				return m_SectionName;
			}
			bool SetName(const String& name) override
			{
				m_SectionName = name;
				return true;
			}

			size_t GetChildrenCount() const override
			{
				return 0;
			}
			bool ClearChildren() override
			{
				return false;
			}
			bool ClearNode() override;

			size_t GetAttributeCount() const override;
			CallbackResult<void> EnumAttributeNames(CallbackFunction<String> func) const override;

			bool HasAttribute(const String& name) const;
			bool RemoveAttribute(const String& name);
			bool ClearAttributes();

			// INIDocumentSection
			String GetComment() const
			{
				return m_Comment;
			}
			void SetComment(String comment)
			{
				m_Comment = std::move(comment);
			}

			CallbackResult<void> EnumKeyNames(CallbackFunction<String> func) const;

		public:
			INIDocumentSection& operator=(const INIDocumentSection&) = default;
			INIDocumentSection& operator=(INIDocumentSection&& other) noexcept
			{
				m_Ref = std::exchange(other.m_Ref, nullptr);
				m_AsCDATA = std::exchange(other.m_AsCDATA, AsCDATA::Auto);
				m_SectionName = std::move(other.m_SectionName);
				m_Comment = std::move(other.m_Comment);
				m_Index = std::exchange(other.m_Index, npos);

				return *this;
			}
	};
}

namespace kxf
{
	class KXF_API INIDocument final: public INIDocumentSection, public IObject
	{
		friend class INIDocumentSection;
		friend class XDocument::XNode<INIDocument>;

		private:
			std::unique_ptr<INIDocumentImpl> m_Document;
			FlagSet<INIDocumentOption> m_Options;

		protected:
			// IXNode
			std::optional<String> DoGetValue() const override
			{
				return IniDoGetValue(m_SectionName, {});
			}
			bool DoSetValue(const String& value, WriteEmpty writeEmpty, AsCDATA asCDATA) override
			{
				return IniDoSetValue(m_SectionName, {}, value, m_Comment, writeEmpty, asCDATA);
			}

			std::optional<String> DoGetAttribute(const String& name) const override
			{
				return IniDoGetValue(m_SectionName, name);
			}
			bool DoSetAttribute(const String& name, const String& value, WriteEmpty writeEmpty) override
			{
				return IniDoSetValue(m_SectionName, name, value, m_Comment, writeEmpty, m_AsCDATA);
			}

			// IObject
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override;

			// INIDocument
			void Init();
			bool DoLoad(const char* ini, size_t length);
			void DoUnload();

			std::optional<String> IniDoGetValue(const String& sectionName, const String& keyName, String* comment = nullptr) const;
			bool IniDoSetValue(const String& sectionName, const String& keyName, const String& value, const String& comment = {}, WriteEmpty writeEmpty = WriteEmpty::Always, AsCDATA asCDATA = AsCDATA::Auto);

			bool RemoveQuotes(String& value) const;
			bool RemoveInlineComments(String& value, String* comment = nullptr) const;
			bool StartsWithInlineComment(const String& value) const;

		public:
			INIDocument();
			INIDocument(const String& ini)
				:INIDocument()
			{
				if (!ini.IsEmpty())
				{
					Load(ini);
				}
			}
			INIDocument(IInputStream& stream)
				:INIDocument()
			{
				Load(stream);
			}
			INIDocument(const INIDocument&) = delete;
			INIDocument(INIDocument&& other) noexcept;
			~INIDocument();

		public:
			// IXNode
			bool IsNull() const override;
			String GetXPath() const override
			{
				return {};
			}

			size_t GetIndexWithinParent() const override
			{
				return npos;
			}
			String GetName() const override
			{
				return {};
			}
			bool SetName(const String& name) override
			{
				return false;
			}

			size_t GetChildrenCount() const override;
			bool HasChildren() const override;
			bool ClearChildren() override;
			bool ClearNode() override;

			// XNode
			INIDocumentSection QueryElement(const String& XPath) const override;
			INIDocumentSection ConstructElement(const String& XPath) override;

			CallbackResult<void> EnumChildren(CallbackFunction<INIDocumentSection> func) const override;
			INIDocumentSection GetFirstChild() const override;
			INIDocumentSection GetLastChild() const override;

		public:
			// INIDocument
			bool Load(const String& ini);
			bool Load(std::span<const char8_t> utf8Data);
			bool Load(IInputStream& stream);
			bool Save(IOutputStream& stream) const;
			String Save() const;
			INIDocument Clone() const;

			FlagSet<INIDocumentOption> GetOptions() const;
			void SetOptions(FlagSet<INIDocumentOption> options);

			CallbackResult<void> EnumSectionNames(CallbackFunction<String> func) const;
			CallbackResult<void> EnumKeyNames(const String& sectionName, CallbackFunction<String> func) const;
			using INIDocumentSection::EnumKeyNames;

			bool HasSection(const String& sectionName) const;
			bool HasValue(const String& sectionName, const String& keyName) const;

			bool RemoveSection(const String& sectionName, bool removeEmpty = true);
			bool RemoveValue(const String& sectionName, const String& keyName, bool removeEmpty = true);

			std::optional<String> IniQueryValue(const String& sectionName, const String& keyName) const
			{
				return IniDoGetValue(sectionName, keyName);
			}
			String IniGetValue(const String& sectionName, const String& keyName, const String& defaultValue = {}) const
			{
				return IniDoGetValue(sectionName, keyName).value_or(defaultValue);
			}
			int64_t IniGetValueInt(const String& sectionName, const String& keyName, int64_t defaultValue = 0) const
			{
				return IniGetValueIntWithBase(sectionName, keyName, 10, defaultValue);
			}
			int64_t IniGetValueIntWithBase(const String& sectionName, const String& keyName, int base, int64_t defaultValue = 0) const
			{
				if (auto value = IniDoGetValue(sectionName, keyName))
				{
					if (auto iValue = ParseInt(*value, base))
					{
						return *iValue;
					}
				}
				return defaultValue;
			}
			double IniGetValueFloat(const String& sectionName, const String& keyName, double defaultValue = 0.0) const
			{
				if (auto value = IniDoGetValue(sectionName, keyName))
				{
					if (auto iValue = ParseFloat(*value))
					{
						return *iValue;
					}
				}
				return defaultValue;
			}
			bool IniGetValueBool(const String& sectionName, const String& keyName, bool defaultValue = false) const
			{
				if (auto value = IniDoGetValue(sectionName, keyName))
				{
					if (auto bValue = ParseBool(*value))
					{
						return *bValue;
					}
				}
				return defaultValue;
			}

			bool IniSetValue(const String& sectionName, const String& keyName, const String& value, WriteEmpty writeEmpty = WriteEmpty::Always, AsCDATA asCDATA = AsCDATA::Auto)
			{
				return IniDoSetValue(sectionName, keyName, value, {}, writeEmpty, asCDATA);
			}
			bool IniSetValue(const String& sectionName, const String& keyName, const char* value, WriteEmpty writeEmpty = WriteEmpty::Always, AsCDATA asCDATA = AsCDATA::Auto)
			{
				return IniDoSetValue(sectionName, keyName, String::FromUTF8(value), {}, writeEmpty, asCDATA);
			}
			bool IniSetValue(const String& sectionName, const String& keyName, const wchar_t* value, WriteEmpty writeEmpty = WriteEmpty::Always, AsCDATA asCDATA = AsCDATA::Auto)
			{
				return IniDoSetValue(sectionName, keyName, String(value), {}, writeEmpty, asCDATA);
			}
			bool IniSetValue(const String& sectionName, const String& keyName, int64_t value)
			{
				return IniDoSetValue(sectionName, keyName, FormatInt(value), {}, WriteEmpty::Always);
			}
			bool IniSetValue(const String& sectionName, const String& keyName, int value)
			{
				return IniDoSetValue(sectionName, keyName, FormatInt(value), {}, WriteEmpty::Always);
			}
			bool IniSetValue(const String& sectionName, const String& keyName, double value, int precision = -1)
			{
				return IniDoSetValue(sectionName, keyName, FormatFloat(value, precision), {}, WriteEmpty::Always);
			}
			bool IniSetValue(const String& sectionName, const String& keyName, float value, int precision = -1)
			{
				return IniDoSetValue(sectionName, keyName, FormatFloat(static_cast<double>(value), precision), {}, WriteEmpty::Always);
			}
			bool IniSetValue(const String& sectionName, const String& keyName, bool value)
			{
				return IniDoSetValue(sectionName, keyName, FormatBool(value), {}, WriteEmpty::Always);
			}

		public:
			INIDocument& operator=(const INIDocument&) = delete;
			INIDocument& operator=(INIDocument&& other) noexcept;
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<INIDocument> final
	{
		uint64_t Serialize(IOutputStream& stream, const INIDocument& value) const
		{
			return BinarySerializer<String>().Serialize(stream, value.Save());
		}
		uint64_t Deserialize(IInputStream& stream, INIDocument& value) const
		{
			String buffer;
			auto read = BinarySerializer<String>().Deserialize(stream, buffer);

			value.Load(buffer);
			return read;
		}
	};
}
