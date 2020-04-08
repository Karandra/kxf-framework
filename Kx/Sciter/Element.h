#pragma once
#include "Common.h"
#include "ScriptValue.h"
#include "Utility/HandleWrapper.h"
#include "Kx/General/Color.h"
#include "Kx/General/String.h"

namespace KxFramework::Sciter
{
	class Node;
	class Host;

	struct ElementHandle;
	struct ElementUID;
}

namespace KxFramework::Sciter
{
	class KX_API Element final: public HandleWrapper<Element, ElementHandle>
	{
		friend class HandleWrapper<Element, ElementHandle>;

		public:
			using TOnElement = std::function<bool(Element)>;

		public:
			static Element Create(const String& tagName, const String& value = {});

		private:
			bool DoAcquire(ElementHandle* handle);
			void DoRelease();

		public:
			Element() = default;
			Element(ElementHandle* handle)
				:HandleWrapper(handle)
			{
			}
			Element(const Element& other)
				:HandleWrapper(other)
			{
			}
			Element(Element&& other)
				:HandleWrapper(std::move(other))
			{
			}
			
		public:
			ElementUID* GetUID() const;
			Host* GetHost() const;

			bool Detach();
			bool Remove();

			Node ToNode() const;
			Element Clone() const;
			void Swap(Element& other);

			// Event handling
			void AttachEventHandler();
			void DetachEventHandler();
			void AttachEventHandler(wxEvtHandler& evtHandler);
			void DetachEventHandler(wxEvtHandler& evtHandler);

			// Refreshing
			bool Update(bool force = false);
			bool UpdateRect(const wxRect& rect);

			// Size and position
			wxRect GetRect() const;
			wxPoint GetPosition() const;
			wxSize GetSize() const;

			wxSize GetMinSize() const;
			wxSize GetMaxSize() const;

			// Visibility
			bool IsVisible() const;
			void SetVisible(bool visible = true);

			// Misc
			bool SetCapture();
			bool ReleaseCapture();

			bool IsFocusable() const;
			bool HasFocus() const;
			void SetFocus();

			bool IsHighlighted() const;
			void SetHighlighted();

			// Scrolling
			void ScrollIntoView(bool toTop = false);
			wxPoint GetScrollPos() const;
			wxSize GetScrollRange() const;
			bool SetScrollPos(const wxPoint& pos) const;

			// HTML content
			String GetInnerHTML() const;
			String GetOuterHTML() const;
			bool SetInnerHTML(const String& html, ElementInnerHTML mode);
			bool SetOuterHTML(const String& html, ElementOuterHTML mode);

			String GetTagName() const;
			bool SetTagName(const String& tagName);

			// Children and parents
			Element GetRoot() const;
			Element GetParent() const;
			Element GetPrevSibling() const;
			Element GetNextSibling() const;
			Element GetFirstChild() const;
			Element GetLastChild() const;

			size_t GetIndexWithinParent() const;
			size_t GetChildrenCount() const;
			Element GetChildAt(size_t index) const;
			Element operator[](size_t index) const
			{
				return GetChildAt(index);
			}
			
			// Insertion
			bool Append(const Element& node);
			bool Prepend(const Element& node);
			bool InsertAt(const Element& node, size_t index);
			bool InsertBefore(const Element& node);
			bool InsertAfter(const Element& node);

			// Native window
			wxWindow* GetWindow() const;
			HWND GetNativeWindow() const;

			bool AttachWindow(wxWindow& window);
			bool AttachNativeWindow(HWND handle);

			wxWindow* DetachWindow();
			HWND DetachNativeWindow();

			// Text
			String GetText() const;
			bool SetText(const String& text) const;

			// Value
			String GetValue() const;
			bool SetValue(StringView value) const;

			// Attributes
			size_t GetAttributeCount() const;
			String GetAttributeNameAt(size_t index) const;
			String GetAttributeValueAt(size_t index) const;
			String GetAttribute(const String& name) const;
			String GetAttribute(const char* name) const;

			bool SetAttribute(const String& name, const String& value);
			bool SetAttribute(const char* name, const String& value);
			bool RemoveAttribute(const String& name);
			bool RemoveAttribute(const char* name);
			bool ClearAttributes();

			// Style (CSS) attributes
			String GetStyleAttribute(const String& name) const;
			String GetStyleAttribute(const char* name) const;

			std::optional<int> GetStyleAttributeInt(const String& name) const;
			std::optional<int> GetStyleAttributeInt(const char* name) const;

			std::optional<double> GetStyleAttributeFloat(const String& name) const;
			std::optional<double> GetStyleAttributeFloat(const char* name) const;

			bool SetStyleAttribute(const String& name, const String& value);
			bool SetStyleAttribute(const char* name, const String& value);

			bool SetStyleAttribute(const String& name, const char* value)
			{
				return SetStyleAttribute(name, String(value));
			}
			bool SetStyleAttribute(const char* name, const char* value)
			{
				return SetStyleAttribute(name, String(value));
			}

			bool SetStyleAttribute(const String& name, const wchar_t* value)
			{
				return SetStyleAttribute(name, String(value));
			}
			bool SetStyleAttribute(const char* name, const wchar_t* value)
			{
				return SetStyleAttribute(name, String(value));
			}

			bool SetStyleAttribute(const String& name, const Color& value);
			bool SetStyleAttribute(const char* name, const Color& value);

			bool SetStyleAttribute(const String& name, int value, SizeUnit unit = SizeUnit::None);
			bool SetStyleAttribute(const char* name, int value, SizeUnit unit = SizeUnit::None);

			bool SetStyleAttribute(const String& name, double value, SizeUnit unit = SizeUnit::None);
			bool SetStyleAttribute(const char* name, double value, SizeUnit unit = SizeUnit::None);

			bool RemoveStyleAttribute(const String& name);
			bool RemoveStyleAttribute(const char* name);

			bool SetStyleFont(const wxFont& font);

			// Selectors
			size_t Select(const String& query, TOnElement onElement) const;
			Element SelectAny(const String& query) const;
			std::vector<Element> SelectAll(const String& query) const;

			Element GetElementByAttribute(const String& name, const String& value) const
			{
				return SelectAny(KxString::Format(wxS("[%1=%2]"), name, value));
			}
			Element GetElementByID(const String& id) const
			{
				return SelectAny(KxString::Format(wxS("#%1"), id));
			}
			Element GetElementByClass(const String& name) const
			{
				return SelectAny(KxString::Format(wxS(".%1"), name));
			}

			// Scripts
			ScriptValue ExecuteScript(const String& script);

		public:
			Element& operator=(const Element& other)
			{
				CopyFrom(other);
				return *this;
			}
			Element& operator=(Element&& other)
			{
				MoveFrom(other);
				return *this;
			}
			Element& operator=(ElementHandle* handle)
			{
				CopyFrom(handle);
				return *this;
			}
	};
}
