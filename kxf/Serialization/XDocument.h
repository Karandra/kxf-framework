#pragma once
#include "Common.h"
#include "String.h"
#include "kxf/Core/CallbackFunction.h"
#include "kxf/Utility/ScopeGuard.h"
#include "Private/XDocument.h"

namespace kxf::XDocument
{
	enum class AsCDATA
	{
		Auto = -1,
		Always = 1,
		Never = 0,
	};
	enum class WriteEmpty
	{
		Always = 1,
		Never = 0,
	};
}

namespace kxf::XDocument
{
	class KX_API IXNode
	{
		public:
			using AsCDATA = XDocument::AsCDATA;
			using WriteEmpty = XDocument::WriteEmpty;

		public:
			static constexpr size_t npos = String::npos;

		public:
			static std::pair<StringView, int> ExtractIndexFromName(StringView elementName, StringView xPathSeparator);
			static bool ContainsValueForbiddenCharacters(const String& value);
			
			template<class TNode>
			static String ConstructXPath(const TNode& thisNode)
			{
				String xPath;
				const String xPathSep = thisNode.GetXPathSeparator();
				const String xPathIndexSep = thisNode.GetXPathIndexSeparator();
				bool isFirst = true;

				for (TNode node = thisNode; node; node = node.GetParent())
				{
					const size_t index = node.GetIndexWithinParent();
					if (index > 1)
					{
						xPath.Prepend(Format("{}{}{}", node.GetName(), xPathIndexSep, index));
					}
					else
					{
						xPath.Prepend(node.GetName());
					}

					if (!isFirst)
					{
						xPath.Prepend(xPathSep);
					}
					isFirst = false;
				}
				return xPath;
			}

		protected:
			virtual String FormatInt(int64_t value, int base = 10) const;
			virtual String FormatPointer(const void* value) const;
			virtual String FormatFloat(double value, int precision = -1) const;
			virtual String FormatBool(bool value) const;

			virtual std::optional<int64_t> ParseInt(const String& value, int base = 10) const;
			virtual std::optional<void*> ParsePointer(const String& value) const;
			virtual std::optional<double> ParseFloat(const String& value) const;
			virtual std::optional<bool> ParseBool(const String& value) const;

		protected:
			virtual std::optional<String> DoGetValue() const = 0;
			virtual std::optional<int64_t> DoGetValueIntWithBase(int base) const
			{
				if (auto value = DoGetValue())
				{
					return ParseInt(*value, base);
				}
				return {};
			}
			virtual std::optional<void*> DoGetValuePointer() const
			{
				if (auto value = DoGetValue())
				{
					return ParsePointer(*value);
				}
				return {};
			}
			virtual std::optional<double> DoGetValueFloat() const
			{
				if (auto value = DoGetValue())
				{
					return ParseFloat(*value);
				}
				return {};
			}
			virtual std::optional<bool> DoGetValueBool() const
			{
				if (auto value = DoGetValue())
				{
					return ParseBool(*value);
				}
				return {};
			}
			virtual bool DoSetValue(const String& value, WriteEmpty writeEmpty, AsCDATA asCDATA) = 0;

			virtual std::optional<String> DoGetAttribute(const String& name) const = 0;
			virtual std::optional<int64_t> DoGetAttributeIntWithBase(const String& name, int base) const
			{
				if (auto value = DoGetAttribute(name))
				{
					return ParseInt(*value, base);
				}
				return {};
			}
			virtual std::optional<void*> DoGetAttributePointer(const String& name) const
			{
				if (auto value = DoGetAttribute(name))
				{
					return ParsePointer(*value);
				}
				return {};
			}
			virtual std::optional<double> DoGetAttributeFloat(const String& name) const
			{
				if (auto value = DoGetAttribute(name))
				{
					return ParseFloat(*value);
				}
				return {};
			}
			virtual std::optional<bool> DoGetAttributeBool(const String& name) const
			{
				if (auto value = DoGetAttribute(name))
				{
					return ParseBool(*value);
				}
				return {};
			}
			virtual bool DoSetAttribute(const String& name, const String& value, WriteEmpty writeEmpty) = 0;

		public:
			virtual ~IXNode() = default;

		public:
			// General
			virtual bool IsNull() const = 0;
			virtual String GetXPath() const
			{
				return {};
			}

			XChar GetXPathSeparator() const
			{
				return '/';
			}
			virtual String GetXPathIndexSeparator() const
			{
				return "::";
			}
			virtual void SetXPathIndexSeparator(const String& value)
			{
			}

			// Node
			virtual size_t GetIndexWithinParent() const
			{
				return npos;
			}
			virtual String GetName() const
			{
				return {};
			}
			virtual bool SetName(const String& name)
			{
				return false;
			}

			virtual size_t GetChildrenCount() const
			{
				return 0;
			}
			virtual bool HasChildren() const
			{
				return GetChildrenCount() != 0;
			}
			virtual bool ClearChildren()
			{
				return false;
			}
			virtual bool ClearNode()
			{
				return false;
			}

			virtual bool IsCDATA() const
			{
				return false;
			}
			virtual bool SetCDATA(bool value)
			{
				return false;
			}

			// Values
			std::optional<String> QueryValue() const
			{
				return DoGetValue();
			}
			String GetValue(String defaultValue = {}) const
			{
				return DoGetValue().value_or(std::move(defaultValue));
			}

			std::optional<bool> QueryValueBool() const
			{
				return DoGetValueBool();
			}
			bool GetValueBool(bool defaultValue = false) const
			{
				return DoGetValueBool().value_or(defaultValue);
			}

			std::optional<double> QueryValueFloat(double defaultValue = 0.0) const
			{
				return DoGetValueFloat().value_or(defaultValue);
			}
			double GetValueFloat(double defaultValue = 0.0) const
			{
				return DoGetValueFloat().value_or(defaultValue);
			}

			std::optional<void*> QueryValuePointer() const
			{
				return DoGetValuePointer();
			}
			void* GetValuePointer(const void* defaultValue = nullptr) const
			{
				return DoGetValuePointer().value_or(const_cast<void*>(defaultValue));
			}

			template<class T = int64_t> requires(Private::TestIntType<T>())
			std::optional<T> QueryValueInt() const
			{
				return QueryValueIntWithBase<T>(10);
			}

			template<class T = int64_t> requires(Private::TestIntType<T>())
			T GetValueInt(T defaultValue = 0) const
			{
				return QueryValueIntWithBase<T>(10).value_or(defaultValue);
			}

			template<class T = int64_t> requires(Private::TestIntType<T>())
			std::optional<T> QueryValueIntWithBase(int base) const
			{
				if (auto value = DoGetValueIntWithBase(base))
				{
					return static_cast<T>(*value);
				}
				return {};
			}

			template<class T = int64_t> requires(Private::TestIntType<T>())
			T GetValueIntWithBase(int base, T defaultValue = 0) const
			{
				return QueryValueIntWithBase<T>(base).value_or(defaultValue);
			}

			bool SetValue(const char* value, WriteEmpty writeEmpty = WriteEmpty::Always, AsCDATA asCDATA = AsCDATA::Auto)
			{
				return DoSetValue(String::FromUTF8(value), writeEmpty, asCDATA);
			}
			bool SetValue(const wchar_t* value, WriteEmpty writeEmpty = WriteEmpty::Always, AsCDATA asCDATA = AsCDATA::Auto)
			{
				return DoSetValue(value, writeEmpty, asCDATA);
			}
			bool SetValue(const String& value, WriteEmpty writeEmpty = WriteEmpty::Always, AsCDATA asCDATA = AsCDATA::Auto)
			{
				return DoSetValue(value, writeEmpty, asCDATA);
			}
			bool SetValue(bool value)
			{
				return DoSetValue(FormatBool(value), WriteEmpty::Always, AsCDATA::Never);
			}
			bool SetValue(float value, int precision = -1)
			{
				return DoSetValue(FormatFloat(static_cast<double>(value), precision), WriteEmpty::Always, AsCDATA::Never);
			}
			bool SetValue(double value, int precision = -1)
			{
				return DoSetValue(FormatFloat(value, precision), WriteEmpty::Always, AsCDATA::Never);
			}
			bool SetValue(const void* value)
			{
				return DoSetValue(FormatPointer(value), WriteEmpty::Always, AsCDATA::Never);
			}
			bool SetValue(std::nullptr_t)
			{
				return DoSetValue(FormatPointer(nullptr), WriteEmpty::Always, AsCDATA::Never);
			}

			template<class T> requires(Private::TestIntType<T>())
			bool SetValue(T value, int base = 10)
			{
				return DoSetValue(FormatInt(static_cast<int64_t>(value), base), WriteEmpty::Always, AsCDATA::Never);
			}

			// Attributes
			virtual size_t GetAttributeCount() const
			{
				return 0;
			}
			virtual bool HasAttributes() const
			{
				return GetAttributeCount() != 0;
			}
			virtual CallbackResult<void> EnumAttributeNames(CallbackFunction<String> func) const
			{
				return {};
			}

			virtual bool HasAttribute(const String& name) const
			{
				return false;
			}
			virtual bool RemoveAttribute(const String& name)
			{
				return false;
			}
			virtual bool ClearAttributes()
			{
				return false;
			}

			std::optional<String> QueryAttribute(const String& name) const
			{
				return DoGetAttribute(name);
			}
			String GetAttribute(const String& name, String defaultValue = {}) const
			{
				return DoGetAttribute(name).value_or(std::move(defaultValue));
			}

			std::optional<bool> QueryAttributeBool(const String& name) const
			{
				return DoGetAttributeBool(name);
			}
			bool GetAttributeBool(const String& name, bool defaultValue = false) const
			{
				return DoGetAttributeBool(name).value_or(defaultValue);
			}

			std::optional<double> QueryAttributeFloat(const String& name) const
			{
				return DoGetAttributeFloat(name);
			}
			double GetAttributeFloat(const String& name, double defaultValue = 0.0) const
			{
				return DoGetAttributeFloat(name).value_or(defaultValue);
			}

			std::optional<void*> QueryAttributePointer(const String& name) const
			{
				return DoGetAttributePointer(name);
			}
			void* GetAttributePointer(const String& name, const void* defaultValue = nullptr) const
			{
				return DoGetAttributePointer(name).value_or(const_cast<void*>(defaultValue));
			}

			template<class T = int64_t> requires(Private::TestIntType<T>())
			std::optional<T> QueryAttributeInt(const String& name) const
			{
				return QueryAttributeIntWithBase<T>(name, 10);
			}

			template<class T = int64_t> requires(Private::TestIntType<T>())
			T GetAttributeInt(const String& name, T defaultValue = 0) const
			{
				return QueryAttributeIntWithBase<T>(name, 10).value_or(defaultValue);
			}

			template<class T = int64_t> requires(Private::TestIntType<T>())
			std::optional<T> QueryAttributeIntWithBase(const String& name, int base) const
			{
				if (auto value = DoGetAttributeIntWithBase(name, base))
				{
					return static_cast<T>(*value);
				}
				return {};
			}

			template<class T = int64_t> requires(Private::TestIntType<T>())
			T GetAttributeIntWithBase(const String& name, int base, T defaultValue = 0) const
			{
				return QueryAttributeIntWithBase<T>(name, base).value_or(defaultValue);
			}

			bool SetAttribute(const String& name, const String& value, WriteEmpty writeEmpty = WriteEmpty::Always)
			{
				return DoSetAttribute(name, value, writeEmpty);
			}
			bool SetAttribute(const String& name, const char* value, WriteEmpty writeEmpty = WriteEmpty::Always)
			{
				return DoSetAttribute(name, String::FromUTF8(value), writeEmpty);
			}
			bool SetAttribute(const String& name, const wchar_t* value, WriteEmpty writeEmpty = WriteEmpty::Always)
			{
				return DoSetAttribute(name, String(value), writeEmpty);
			}
			bool SetAttribute(const String& name, bool value)
			{
				return DoSetAttribute(name, FormatBool(value), WriteEmpty::Always);
			}
			bool SetAttribute(const String& name, float value, int precision = -1)
			{
				return DoSetAttribute(name, FormatFloat(static_cast<double>(value), precision), WriteEmpty::Always);
			}
			bool SetAttribute(const String& name, double value, int precision = -1)
			{
				return DoSetAttribute(name, FormatFloat(value, precision), WriteEmpty::Always);
			}
			bool SetAttribute(const String& name, const void* value)
			{
				return DoSetAttribute(name, FormatPointer(value), WriteEmpty::Always);
			}
			bool SetAttribute(const String& name, std::nullptr_t)
			{
				return DoSetAttribute(name, FormatPointer(nullptr), WriteEmpty::Always);
			}

			template<class T> requires(Private::TestIntType<T>())
			bool SetAttribute(const String& name, T value, int base = 10)
			{
				return DoSetAttribute(name, FormatInt(static_cast<int64_t>(value), base), WriteEmpty::Always);
			}

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

namespace kxf::XDocument
{
	template<class TNode_>
	class XNode: public IXNode
	{
		public:
			using TNode = TNode_;

		public:
			// General
			virtual TNode QueryElement(const String& XPath) const
			{
				return {};
			}
			virtual TNode ConstructElement(const String& XPath)
			{
				return {};
			}

			// Node
			virtual CallbackResult<void> EnumChildren(CallbackFunction<TNode> func) const
			{
				return {};
			}

			// Navigation
			virtual TNode GetElementByAttribute(const String& name, const String& value) const
			{
				return {};
			}
			virtual TNode GetElementByTag(const String& tagName) const
			{
				return {};
			}

			virtual TNode GetParent() const
			{
				return {};
			}
			virtual TNode GetPreviousSibling() const
			{
				return {};
			}
			virtual TNode GetNextSibling() const
			{
				return {};
			}
			virtual TNode GetFirstChild() const
			{
				return {};
			}
			virtual TNode GetLastChild() const
			{
				return {};
			}

			// Insertion

			// Deletion
	};
}
