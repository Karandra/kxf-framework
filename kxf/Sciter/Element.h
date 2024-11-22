#pragma once
#include "Common.h"
#include "ScriptValue.h"
#include "Utility/HandleWrapper.h"
#include "kxf/Core/String.h"

namespace kxf
{
	class IEvtHandler;
}
namespace kxf::Sciter
{
	class Node;
	class Host;
	class Widget;

	struct ElementHandle;
	struct ElementUID;

	enum class ElementVisibility
	{
		Hidden = 0,
		Visible = 1,
		Default = -1
	};
}

namespace kxf::Sciter
{
	class KX_API Element final: public HandleWrapper<Element, ElementHandle>
	{
		friend class HandleWrapper<Element, ElementHandle>;

		public:
			static Element Create(const String& tagName, const String& value = {});

		private:
			bool DoAcquire(ElementHandle* handle) noexcept;
			void DoRelease() noexcept;

		public:
			Element() noexcept = default;
			Element(ElementHandle* handle) noexcept
				:HandleWrapper(handle)
			{
			}
			Element(const Element& other) noexcept
				:HandleWrapper(other)
			{
			}
			Element(Element&& other) noexcept
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
			void AttachEventHandler(IEvtHandler& evtHandler);
			void DetachEventHandler(IEvtHandler& evtHandler);

			// Refreshing
			bool Update(bool force = false);
			bool UpdateRect(const Rect& rect);

			// Size and position
			Rect GetRect() const;
			Point GetPosition() const;
			Size GetSize() const;

			Size GetMinSize() const;
			Size GetMaxSize() const;

			// Visibility
			bool IsVisible() const;
			bool SetVisible(bool visible = true)
			{
				return SetVisible(visible ? ElementVisibility::Default : ElementVisibility::Hidden);
			}
			bool SetVisible(ElementVisibility visibility);

			// Misc
			bool SetCapture();
			bool ReleaseCapture();

			bool IsFocusable() const;
			bool HasFocus() const;
			bool SetFocus();

			bool IsHighlighted() const;
			void SetHighlighted();

			FlagSet<ElementState> GetState() const;
			bool SetState(FlagSet<ElementState> state, bool update = false);
			bool AddState(FlagSet<ElementState> state, bool update = false);
			bool RemoveState(FlagSet<ElementState> state, bool update = false);

			bool IsActive() const;
			bool IsCurrent() const;
			bool IsAnimating() const;

			// Scrolling
			void ScrollIntoView(bool toTop = false);
			Point GetScrollPos() const;
			Size GetScrollRange() const;
			bool SetScrollPos(const Point& pos) const;

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
			bool AppendChild(const Element& childNode);
			bool PrependChild(const Element& childNode);
			bool InsertChildAt(const Element& childNode, size_t index);
			bool InsertChildBefore(const Element& childNode);
			bool InsertChildAfter(const Element& childNode);

			// Native window
			wxWindow* GetWindow() const;
			void* GetNativeWindow() const;

			bool AttachWindow(wxWindow& window);
			bool AttachNativeWindow(void* handle);

			wxWindow* DetachWindow();
			void* DetachNativeWindow();

			// Text
			bool HasText() const;
			String GetText() const;
			bool SetText(const String& text) const
			{
				return SetText(text.view());
			}
			bool SetText(StringView text) const;

			// Value
			bool HasValue() const;
			ScriptValue GetValue() const;
			bool SetValue(const ScriptValue& value) const;

			// Attributes
			bool HasAttribute(const String& name) const;
			bool HasAttribute(const char* name) const;

			String GetAttribute(const String& name) const;
			String GetAttribute(const char* name) const;

			size_t GetAttributeCount() const;
			String GetAttributeNameAt(size_t index) const;
			String GetAttributeValueAt(size_t index) const;

			bool SetAttribute(const String& name, const String& value);
			bool SetAttribute(const char* name, const String& value);
			bool RemoveAttribute(const String& name);
			bool RemoveAttribute(const char* name);
			bool ClearAttributes();

			// Style (CSS) attributes
			bool HasStyleAttribute(const String& name) const;
			bool HasStyleAttribute(const char* name) const;

			String GetStyleAttribute(const String& name) const;
			String GetStyleAttribute(const char* name) const;

			std::optional<int> GetStyleAttributeInt(const String& name) const;
			std::optional<int> GetStyleAttributeInt(const char* name) const;

			std::optional<double> GetStyleAttributeFloat(const String& name) const;
			std::optional<double> GetStyleAttributeFloat(const char* name) const;

			Color GetStyleAttributeColor(const char* name, ColorSpace* colorSpace = nullptr) const;
			Color GetStyleAttributeColor(const String& name, ColorSpace* colorSpace = nullptr) const;

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

			Font GetStyleFont() const;
			bool SetStyleFont(const Font& font);

			// Selectors
			size_t Select(const String& query, std::function<bool(Element)> onElement) const;
			Element SelectAny(const String& query) const;
			Enumerator<Element> SelectAll(const String& query) const;

			Element GetElementByAttribute(const String& name, const String& value) const
			{
				return SelectAny(Format("[{}={}]", name, value));
			}
			Element GetElementByID(const String& id) const
			{
				return SelectAny(Format("#{}", id));
			}
			Element GetElementByClass(const String& name) const
			{
				return SelectAny(Format(".{}", name));
			}

			// Widgets
			size_t SelectWidgets(const String& query, std::function<bool(Widget&)> onWidget) const;
			Widget* SelectAnyWidget(const String& query) const;
			Enumerator<Widget&> SelectAllWidgets(const String& query) const;

			Widget* GetWidgetByAttribute(const String& name, const String& value) const
			{
				return SelectAnyWidget(Format("[{}={}]", name, value));
			}
			Widget* GetWidgetByID(const String& id) const
			{
				return SelectAnyWidget(Format("#{}", id));
			}
			Widget* GetWidgetByClass(const String& name) const
			{
				return SelectAnyWidget(Format(".{}", name));
			}

			template<class TWidget, class TFunc>
			size_t SelectWidgetsByType(const String& query, TFunc&& onWidget) const
			{
				if constexpr(std::is_same_v<Widget, TWidget>)
				{
					return SelectWidgets(query, onWidget);
				}
				else
				{
					size_t count = 0;
					SelectWidgets(query, [&](auto& widget)
					{
						if (auto desiredWidget = widget.QueryInterface<TWidget>())
						{
							count++;
							if (!std::invoke(onWidget, *desiredWidget))
							{
								return false;
							}
						}
						return true;
					});
					return count;
				}
			}

			// Scripts
			ScriptValue ExecuteScript(const String& script);

		public:
			Element& operator=(const Element& other) noexcept
			{
				CopyFrom(other);
				return *this;
			}
			Element& operator=(Element&& other) noexcept
			{
				MoveFrom(other);
				return *this;
			}
			Element& operator=(ElementHandle* handle) noexcept
			{
				CopyFrom(handle);
				return *this;
			}
	};
}
