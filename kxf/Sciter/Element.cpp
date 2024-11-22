#include "KxfPCH.h"
#include "Element.h"
#include "Node.h"
#include "Host.h"
#include "Widget.h"
#include "EventDispatcher.h"
#include "ScriptValue.h"
#include "SciterAPI.h"
#include "kxf/Sciter/Private/Conversion.h"
#include "kxf/Core/RegEx.h"
#include "kxf/Utility/Drawing.h"
#include "kxf/Utility/Enumerator.h"

#pragma warning(disable: 4312) // 'reinterpret_cast': conversion from 'UINT' to 'void *' of greater size

namespace
{
	void ExtractInt(kxf::String& value)
	{
		for (size_t i = 0; i < value.length(); i++)
		{
			if (!std::isdigit(value[i]))
			{
				value.Truncate(i);
				break;
			}
		}
	}
	void ExtractFloat(kxf::String& value)
	{
		for (size_t i = 0; i < value.length(); i++)
		{
			auto c = value[i];
			if (!std::isdigit(c) && c != '.')
			{
				value.Truncate(i);
				break;
			}
		}
	}

	constexpr kxf::FlagSet<kxf::Sciter::ElementState> MapElementState(uint32_t nativeState) noexcept
	{
		using namespace kxf;
		using namespace kxf::Sciter;

		FlagSet<ElementState> state;
		state.Add(ElementState::Link, nativeState & ELEMENT_STATE_BITS::STATE_LINK);
		state.Add(ElementState::Hover, nativeState & ELEMENT_STATE_BITS::STATE_HOVER);
		state.Add(ElementState::Active, nativeState & ELEMENT_STATE_BITS::STATE_ACTIVE);
		state.Add(ElementState::Focus, nativeState & ELEMENT_STATE_BITS::STATE_FOCUS);
		state.Add(ElementState::Visited, nativeState & ELEMENT_STATE_BITS::STATE_VISITED);
		state.Add(ElementState::Current, nativeState & ELEMENT_STATE_BITS::STATE_CURRENT);
		state.Add(ElementState::Checked, nativeState & ELEMENT_STATE_BITS::STATE_CHECKED);
		state.Add(ElementState::Disabled, nativeState & ELEMENT_STATE_BITS::STATE_DISABLED);
		state.Add(ElementState::ReadOnly, nativeState & ELEMENT_STATE_BITS::STATE_READONLY);
		state.Add(ElementState::Expanded, nativeState & ELEMENT_STATE_BITS::STATE_EXPANDED);
		state.Add(ElementState::Collapsed, nativeState & ELEMENT_STATE_BITS::STATE_COLLAPSED);
		state.Add(ElementState::Incomplete, nativeState & ELEMENT_STATE_BITS::STATE_INCOMPLETE);
		state.Add(ElementState::Animating, nativeState & ELEMENT_STATE_BITS::STATE_ANIMATING);
		state.Add(ElementState::Focusable, nativeState & ELEMENT_STATE_BITS::STATE_FOCUSABLE);
		state.Add(ElementState::Anchor, nativeState & ELEMENT_STATE_BITS::STATE_ANCHOR);
		state.Add(ElementState::Syntetic, nativeState & ELEMENT_STATE_BITS::STATE_SYNTHETIC);
		state.Add(ElementState::OwnsPopup, nativeState & ELEMENT_STATE_BITS::STATE_OWNS_POPUP);
		state.Add(ElementState::TabFocus, nativeState & ELEMENT_STATE_BITS::STATE_TABFOCUS);
		state.Add(ElementState::Empty, nativeState & ELEMENT_STATE_BITS::STATE_EMPTY);
		state.Add(ElementState::Busy, nativeState & ELEMENT_STATE_BITS::STATE_BUSY);
		state.Add(ElementState::DragOver, nativeState & ELEMENT_STATE_BITS::STATE_DRAG_OVER);
		state.Add(ElementState::DragSource, nativeState & ELEMENT_STATE_BITS::STATE_DRAG_SOURCE);
		state.Add(ElementState::DropTarget, nativeState & ELEMENT_STATE_BITS::STATE_DROP_TARGET);
		state.Add(ElementState::DropMarker, nativeState & ELEMENT_STATE_BITS::STATE_DROP_MARKER);
		state.Add(ElementState::Moving, nativeState & ELEMENT_STATE_BITS::STATE_MOVING);
		state.Add(ElementState::Copying, nativeState & ELEMENT_STATE_BITS::STATE_COPYING);
		state.Add(ElementState::Pressed, nativeState & ELEMENT_STATE_BITS::STATE_PRESSED);
		state.Add(ElementState::Popup, nativeState & ELEMENT_STATE_BITS::STATE_POPUP);
		state.Add(ElementState::LayoutLTR, nativeState & ELEMENT_STATE_BITS::STATE_IS_LTR);
		state.Add(ElementState::LayoutRTL, nativeState & ELEMENT_STATE_BITS::STATE_IS_RTL);

		return state;
	}
	constexpr uint32_t MapElementState(kxf::FlagSet<kxf::Sciter::ElementState> state) noexcept
	{
		using namespace kxf;
		using namespace kxf::Sciter;

		FlagSet<uint32_t> nativeState;
		nativeState.Add(ELEMENT_STATE_BITS::STATE_LINK, state & ElementState::Link);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_HOVER, state & ElementState::Hover);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_ACTIVE, state & ElementState::Active);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_FOCUS, state & ElementState::Focus);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_VISITED, state & ElementState::Visited);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_CURRENT, state & ElementState::Current);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_CHECKED, state & ElementState::Checked);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_DISABLED, state & ElementState::Disabled);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_READONLY, state & ElementState::ReadOnly);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_EXPANDED, state & ElementState::Expanded);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_COLLAPSED, state & ElementState::Collapsed);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_INCOMPLETE, state & ElementState::Incomplete);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_ANIMATING, state & ElementState::Animating);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_FOCUSABLE, state & ElementState::Focusable);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_ANCHOR, state & ElementState::Anchor);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_SYNTHETIC, state & ElementState::Syntetic);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_OWNS_POPUP, state & ElementState::OwnsPopup);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_TABFOCUS, state & ElementState::TabFocus);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_EMPTY, state & ElementState::Empty);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_BUSY, state & ElementState::Busy);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_DRAG_OVER, state & ElementState::DragOver);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_DRAG_SOURCE, state & ElementState::DragSource);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_DROP_TARGET, state & ElementState::DropTarget);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_DROP_MARKER, state & ElementState::DropMarker);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_MOVING, state & ElementState::Moving);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_COPYING, state & ElementState::Copying);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_PRESSED, state & ElementState::Pressed);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_POPUP, state & ElementState::Popup);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_IS_LTR, state & ElementState::LayoutLTR);
		nativeState.Add(ELEMENT_STATE_BITS::STATE_IS_RTL, state & ElementState::LayoutRTL);

		return *nativeState;
	}

	template<class TFunc>
	kxf::Sciter::Element DoGetElemenet(kxf::Sciter::ElementHandle* handle, TFunc&& func)
	{
		using namespace kxf::Sciter;

		HELEMENT node = nullptr;
		if (func(ToSciterElement(handle), &node) == SCDOM_OK)
		{
			return FromSciterElement(node);
		}
		return {};
	}

	kxf::FlagSet<ELEMENT_STATE_BITS> DoGetElementState(kxf::Sciter::ElementHandle* handle) noexcept
	{
		using namespace kxf::Sciter;

		UINT state = 0;
		GetSciterAPI()->SciterGetElementState(ToSciterElement(handle), &state);
		return static_cast<ELEMENT_STATE_BITS>(state);
	}

	template<class TName>
	std::optional<int> DoGetStyleAttributeInt(const kxf::Sciter::Element& element, TName&& name)
	{
		kxf::String value = element.GetStyleAttribute(name);
		ExtractInt(value);

		return value.ToInteger<int>();
	}

	template<class TName>
	std::optional<double> DoGetStyleAttributeFloat(const kxf::Sciter::Element& element, TName&& name)
	{
		kxf::String value = element.GetStyleAttribute(name);
		ExtractFloat(value);
		return value.ToFloatingPoint<double>();
	}

	void SC_CALLBACK ExtractStringLength(const wchar_t* value, UINT length, void* context)
	{
		*reinterpret_cast<size_t*>(context) = length;
	}
	void SC_CALLBACK ExtractStringLength(const char* value, UINT length, void* context)
	{
		*reinterpret_cast<size_t*>(context) = length;
	}
	void SC_CALLBACK ExtractStringLength(const BYTE* value, UINT length, void* context)
	{
		*reinterpret_cast<size_t*>(context) = length;
	}

	void SC_CALLBACK ExtractKxfString(const wchar_t* value, UINT length, void* context)
	{
		using namespace kxf;

		*reinterpret_cast<String*>(context) = String(value, length);
	}
	void SC_CALLBACK ExtractKxfString(const char* value, UINT length, void* context)
	{
		using namespace kxf;

		*reinterpret_cast<String*>(context) = String(value, length);
	}
	void SC_CALLBACK ExtractKxfString(const BYTE* value, UINT length, void* context)
	{
		ExtractKxfString(reinterpret_cast<const char*>(value), length, context);
	}
}

namespace kxf::Sciter
{
	Element Element::Create(const String& tagName, const String& value)
	{
		Element node;

		auto tagNameUTF8 = tagName.ToUTF8();
		HELEMENT nativeNode = nullptr;
		if (GetSciterAPI()->SciterCreateElement(tagNameUTF8.data(), value.wc_str(), &nativeNode) == SCDOM_OK)
		{
			node.AttachHandle(FromSciterElement(nativeNode));
		}
		return node;
	}

	bool Element::DoAcquire(ElementHandle* handle) noexcept
	{
		return GetSciterAPI()->Sciter_UseElement(ToSciterElement(handle)) == SCDOM_OK;
	}
	void Element::DoRelease() noexcept
	{
		GetSciterAPI()->Sciter_UnuseElement(ToSciterElement(m_Handle));
	}

	ElementUID* Element::GetUID() const
	{
		UINT id = 0;
		GetSciterAPI()->SciterGetElementUID(ToSciterElement(m_Handle), &id);
		return reinterpret_cast<ElementUID*>(id);
	}
	Host* Element::GetHost() const
	{
		HWND windowHandle = nullptr;
		GetSciterAPI()->SciterGetElementHwnd(ToSciterElement(m_Handle), &windowHandle, TRUE);
		if (windowHandle)
		{
			return reinterpret_cast<Host*>(::GetWindowLongPtrW(windowHandle, GWLP_USERDATA));
		}
		return nullptr;
	}

	bool Element::Detach()
	{
		return GetSciterAPI()->SciterDetachElement(ToSciterElement(m_Handle)) == SCDOM_OK;
	}
	bool Element::Remove()
	{
		if (GetSciterAPI()->SciterDeleteElement(ToSciterElement(m_Handle)) == SCDOM_OK)
		{
			Release();
			return true;
		}
		return false;
	}

	Node Element::ToNode() const
	{
		HNODE node = nullptr;
		GetSciterAPI()->SciterNodeCastFromElement(ToSciterElement(m_Handle), &node);
		return Node(reinterpret_cast<NodeHandle*>(node));
	}
	Element Element::Clone() const
	{
		HELEMENT nativeNode = nullptr;
		if (GetSciterAPI()->SciterCloneElement(ToSciterElement(m_Handle), &nativeNode) == SCDOM_OK)
		{
			Element node;
			node.AttachHandle(FromSciterElement(nativeNode));
			return node;
		}
		return {};
	}
	void Element::Swap(Element& other)
	{
		GetSciterAPI()->SciterSwapElements(ToSciterElement(m_Handle), ToSciterElement(other.m_Handle));
	}

	// Event handling
	void Element::AttachEventHandler()
	{
		if (Host* host = GetHost())
		{
			host->AttachElementHandler(*this);
		}
	}
	void Element::DetachEventHandler()
	{
		if (Host* host = GetHost())
		{
			host->DetachElementHandler(*this);
		}
	}
	void Element::AttachEventHandler(IEvtHandler& evtHandler)
	{
		if (Host* host = GetHost())
		{
			host->AttachElementHandler(*this, evtHandler);
		}
	}
	void Element::DetachEventHandler(IEvtHandler& evtHandler)
	{
		if (Host* host = GetHost())
		{
			host->DetachElementHandler(*this, evtHandler);
		}
	}

	// Refreshing
	bool Element::Update(bool force)
	{
		return GetSciterAPI()->SciterUpdateElement(ToSciterElement(m_Handle), force) == SCDOM_OK;
	}
	bool Element::UpdateRect(const Rect& rect)
	{
		return GetSciterAPI()->SciterRefreshElementArea(ToSciterElement(m_Handle), Utility::ToWindowsRect(rect)) == SCDOM_OK;
	}

	// Size and position
	Rect Element::GetRect() const
	{
		RECT nativeRect = {};
		if (GetSciterAPI()->SciterGetElementLocation(ToSciterElement(m_Handle), &nativeRect, ELEMENT_AREAS::VIEW_RELATIVE) == SCDOM_OK)
		{
			return Utility::FromWindowsRect(nativeRect);
		}
		return {};
	}
	Point Element::GetPosition() const
	{
		return GetRect().GetPosition();
	}
	Size Element::GetSize() const
	{
		return GetRect().GetSize();
	}

	Size Element::GetMinSize() const
	{
		INT minWidth = 0;
		INT maxWidth = 0;
		INT height = 0;
		GetSciterAPI()->SciterGetElementIntrinsicWidths(ToSciterElement(m_Handle), &minWidth, &maxWidth);
		GetSciterAPI()->SciterGetElementIntrinsicHeight(ToSciterElement(m_Handle), minWidth, &height);

		return Size(minWidth, height);
	}
	Size Element::GetMaxSize() const
	{
		INT minWidth = 0;
		INT maxWidth = 0;
		INT height = 0;
		GetSciterAPI()->SciterGetElementIntrinsicWidths(ToSciterElement(m_Handle), &minWidth, &maxWidth);
		GetSciterAPI()->SciterGetElementIntrinsicHeight(ToSciterElement(m_Handle), maxWidth, &height);

		return Size(maxWidth, height);
	}

	// Visibility
	bool Element::IsVisible() const
	{
		BOOL result = FALSE;
		GetSciterAPI()->SciterIsElementVisible(ToSciterElement(m_Handle), &result);

		return result;
	}
	bool Element::SetVisible(ElementVisibility visibility)
	{
		switch (visibility)
		{
			case ElementVisibility::Visible:
			{
				return SetStyleAttribute("visibility", "visible");
			}
			case ElementVisibility::Hidden:
			{
				return SetStyleAttribute("visibility", "hidden");
			}
			case ElementVisibility::Default:
			{
				return RemoveStyleAttribute("visibility");
			}
		};
		return false;
	}

	// Misc
	bool Element::SetCapture()
	{
		return GetSciterAPI()->SciterSetCapture(ToSciterElement(m_Handle)) == SCDOM_OK;
	}
	bool Element::ReleaseCapture()
	{
		return GetSciterAPI()->SciterReleaseCapture(ToSciterElement(m_Handle)) == SCDOM_OK;
	}

	bool Element::IsFocusable() const
	{
		return DoGetElementState(m_Handle).Contains(ELEMENT_STATE_BITS::STATE_FOCUSABLE);
	}
	bool Element::HasFocus() const
	{
		return DoGetElementState(m_Handle).Contains(ELEMENT_STATE_BITS::STATE_FOCUS);
	}
	bool Element::SetFocus()
	{
		return GetSciterAPI()->SciterSetElementState(ToSciterElement(m_Handle), ELEMENT_STATE_BITS::STATE_FOCUS, 0, FALSE) == SCDOM_OK;
	}

	bool Element::IsHighlighted() const
	{
		if (Host* host = GetHost())
		{
			return host->GetHighlightedElement() == *this;
		}
		return false;
	}
	void Element::SetHighlighted()
	{
		if (Host* host = GetHost())
		{
			host->SetHighlightedElement(*this);
		}
	}

	FlagSet<ElementState> Element::GetState() const
	{
		UINT nativeState = 0;
		if (GetSciterAPI()->SciterGetElementState(ToSciterElement(m_Handle), &nativeState) == SCDOM_OK)
		{
			return MapElementState(nativeState);
		}
		return {};
	}
	bool Element::SetState(FlagSet<ElementState> state, bool update)
	{
		UINT existingNativeState = 0;
		if (GetSciterAPI()->SciterGetElementState(ToSciterElement(m_Handle), &existingNativeState) == SCDOM_OK)
		{
			const uint32_t newNativeState = MapElementState(state);
			return GetSciterAPI()->SciterSetElementState(ToSciterElement(m_Handle), newNativeState, existingNativeState ^ newNativeState, update) == SCDOM_OK;
		}
		return false;
	}
	bool Element::AddState(FlagSet<ElementState> state, bool update)
	{
		const uint32_t nativeState = MapElementState(state);
		return GetSciterAPI()->SciterSetElementState(ToSciterElement(m_Handle), nativeState, 0, update) == SCDOM_OK;
	}
	bool Element::RemoveState(FlagSet<ElementState> state, bool update)
	{
		const uint32_t nativeState = MapElementState(state);
		return GetSciterAPI()->SciterSetElementState(ToSciterElement(m_Handle), 0, nativeState, update) == SCDOM_OK;
	}

	bool Element::IsActive() const
	{
		return DoGetElementState(m_Handle).Contains(ELEMENT_STATE_BITS::STATE_ACTIVE);
	}
	bool Element::IsCurrent() const
	{
		return DoGetElementState(m_Handle).Contains(ELEMENT_STATE_BITS::STATE_CURRENT);
	}
	bool Element::IsAnimating() const
	{
		return DoGetElementState(m_Handle).Contains(ELEMENT_STATE_BITS::STATE_ANIMATING);
	}

	// Scrolling
	void Element::ScrollIntoView(bool toTop)
	{
		UINT nativeFlags = 0;
		if (toTop)
		{
			nativeFlags |= SCITER_SCROLL_FLAGS::SCROLL_TO_TOP;
		}
		if (Host* host = GetHost(); host && host->IsSmoothScrollingEnabled())
		{
			nativeFlags |= SCITER_SCROLL_FLAGS::SCROLL_SMOOTH;
		}

		GetSciterAPI()->SciterScrollToView(ToSciterElement(m_Handle), nativeFlags);
	}
	Point Element::GetScrollPos() const
	{
		POINT pos = {};
		if (GetSciterAPI()->SciterGetScrollInfo(ToSciterElement(m_Handle), &pos, nullptr, nullptr) == SCDOM_OK)
		{
			return Point(pos.x, pos.y);
		}
		return Point::UnspecifiedPosition();
	}
	Size Element::GetScrollRange() const
	{
		RECT range = {};
		if (GetSciterAPI()->SciterGetScrollInfo(ToSciterElement(m_Handle), nullptr, &range, nullptr) == SCDOM_OK)
		{
			return Utility::FromWindowsRect(range).GetSize();
		}
		return Size::UnspecifiedSize();
	}
	bool Element::SetScrollPos(const Point& pos) const
	{
		const Host* host = GetHost();
		return GetSciterAPI()->SciterSetScrollPos(ToSciterElement(m_Handle), {pos.GetX(), pos.GetY()}, host ? host->IsSmoothScrollingEnabled() : false) == SCDOM_OK;
	}

	// HTML content
	String Element::GetInnerHTML() const
	{
		String result;
		if (GetSciterAPI()->SciterGetElementHtmlCB(ToSciterElement(m_Handle), FALSE, ExtractKxfString, &result) == SCDOM_OK)
		{
			return result;
		}
		return {};
	}
	String Element::GetOuterHTML() const
	{
		String result;
		if (GetSciterAPI()->SciterGetElementHtmlCB(ToSciterElement(m_Handle), TRUE, ExtractKxfString, &result) == SCDOM_OK)
		{
			return result;
		}
		return {};
	}
	bool Element::SetInnerHTML(const String& html, ElementInnerHTML mode)
	{
		auto MapMode = [&]() -> std::optional<UINT>
		{
			switch (mode)
			{
				case ElementInnerHTML::Replace:
				{
					return SIH_REPLACE_CONTENT;
				}
				case ElementInnerHTML::InsertBefore:
				{
					return SIH_INSERT_AT_START;
				}
				case ElementInnerHTML::InsertAfter:
				{
					return SIH_APPEND_AFTER_LAST;
				}
			};
			return std::nullopt;
		};

		if (auto nativeMode = MapMode())
		{
			auto utf8 = ToSciterUTF8(html);
			return GetSciterAPI()->SciterSetElementHtml(ToSciterElement(m_Handle), utf8.data(), utf8.size(), *nativeMode) == SCDOM_OK;
		}
		return false;
	}
	bool Element::SetOuterHTML(const String& html, ElementOuterHTML mode)
	{
		auto MapMode = [&]() -> std::optional<UINT>
		{
			switch (mode)
			{
				case ElementOuterHTML::Replace:
				{
					return SOH_REPLACE;
				}
				case ElementOuterHTML::InsertBefore:
				{
					return SOH_INSERT_BEFORE;
				}
				case ElementOuterHTML::InsertAfter:
				{
					return SOH_INSERT_AFTER;
				}
			};
			return std::nullopt;
		};

		if (auto nativeMode = MapMode())
		{
			auto utf8 = ToSciterUTF8(html);
			return GetSciterAPI()->SciterSetElementHtml(ToSciterElement(m_Handle), utf8.data(), utf8.size(), *nativeMode) == SCDOM_OK;
		}
		return false;
	}

	String Element::GetTagName() const
	{
		String result;
		GetSciterAPI()->SciterGetElementTypeCB(ToSciterElement(m_Handle), ExtractKxfString, &result);
		return result;
	}
	bool Element::SetTagName(const String& tagName)
	{
		if (GetTagName() == tagName)
		{
			return true;
		}

		// I can't believe there isn't a function is Sciter to change element's tag name.
		// This ugly hack is absolutely terrible.
		if (Element parent = GetParent())
		{
			// Collect all element's children
			const size_t childrenCount = GetChildrenCount();

			std::vector<Element> children;
			children.reserve(childrenCount);
			for (size_t i = 0; i < childrenCount; i++)
			{
				children.emplace_back(GetChildAt(i));
			}

			// Collect all attributes
			const size_t attributeCount = GetAttributeCount();

			std::vector<std::pair<String, String>> attributes;
			attributes.reserve(attributeCount);
			for (size_t i = 0; i < attributeCount; i++)
			{
				attributes.emplace_back(GetAttributeNameAt(i), GetAttributeValueAt(i));
			}

			// Create empty element with required tag name
			Element newElement = Create(tagName);

			// Insert new the element into the DOM
			parent.InsertChildAt(newElement, GetIndexWithinParent());

			// Move children into new element
			for (Element& element: children)
			{
				newElement.AppendChild(element);
			}

			// Add text node if needed
			if (String value = GetText(); !value.IsEmpty())
			{
				newElement.ToNode().AppendChild(Node::CreateTextNode(value));
			}

			// Add attributes
			for (const auto& [name, value]: attributes)
			{
				newElement.SetAttribute(name, value);
			}

			// Remove the original one
			Remove();

			// Now new element is this element
			*this = std::move(newElement);

			return true;
		}
		return false;
	}

	// Children and parents
	Element Element::GetRoot() const
	{
		if (Host* host = GetHost())
		{
			return host->GetRootElement();
		}
		return {};
	}
	Element Element::GetParent() const
	{
		return DoGetElemenet(m_Handle, GetSciterAPI()->SciterGetParentElement);
	}
	Element Element::GetPrevSibling() const
	{
		const size_t index = GetIndexWithinParent();
		const Element parent = GetParent();

		if (parent && index != 0)
		{
			return parent.GetChildAt(index - 1);
		}
		return {};
	}
	Element Element::GetNextSibling() const
	{
		const size_t index = GetIndexWithinParent();
		const Element parent = GetParent();

		if (parent && index + 1 < parent.GetChildrenCount())
		{
			return parent.GetChildAt(index + 1);
		}
		return {};
	}
	Element Element::GetFirstChild() const
	{
		return GetChildAt(0);
	}
	Element Element::GetLastChild() const
	{
		size_t count = GetChildrenCount();
		if (count != 0)
		{
			return GetChildAt(count - 1);
		}
		return {};
	}

	size_t Element::GetIndexWithinParent() const
	{
		UINT index = std::numeric_limits<size_t>::max();
		GetSciterAPI()->SciterGetElementIndex(ToSciterElement(m_Handle), &index);
		return index;
	}
	size_t Element::GetChildrenCount() const
	{
		UINT count = 0;
		GetSciterAPI()->SciterGetChildrenCount(ToSciterElement(m_Handle), &count);

		return count;
	}
	Element Element::GetChildAt(size_t index) const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterGetNthChild(ToSciterElement(m_Handle), index, &node) == SCDOM_OK)
		{
			return FromSciterElement(node);
		}
		return {};
	}

	// Insertion
	bool Element::AppendChild(const Element& childNode)
	{
		return InsertChildAt(childNode, GetIndexWithinParent() + 1);
	}
	bool Element::PrependChild(const Element& childNode)
	{
		const size_t index = GetIndexWithinParent();
		return InsertChildAt(childNode, index != 0 ? index - 1 : 0);
	}
	bool Element::InsertChildAt(const Element& childNode, size_t index)
	{
		return GetSciterAPI()->SciterInsertElement(ToSciterElement(childNode.m_Handle), ToSciterElement(m_Handle), index) == SCDOM_OK;
	}
	bool Element::InsertChildBefore(const Element& childNode)
	{
		const size_t index = childNode.GetIndexWithinParent();
		return InsertChildAt(childNode, index != 0 ? index - 1 : 0);
	}
	bool Element::InsertChildAfter(const Element& childNode)
	{
		const size_t index = childNode.GetIndexWithinParent();
		return InsertChildAt(childNode, index + 1);
	}

	// Native window
	wxWindow* Element::GetWindow() const
	{
		if (void* handle = GetNativeWindow())
		{
			for (wxWindow* window: wxTopLevelWindows)
			{
				if (window->GetHandle() == handle)
				{
					return window;
				}
				else if (wxWindow* childWindow = window->FindItemByHWND(static_cast<HWND>(handle)))
				{
					return childWindow;
				}
			}
		}
		return nullptr;
	}
	void* Element::GetNativeWindow() const
	{
		HWND windowHandle = nullptr;
		GetSciterAPI()->SciterGetElementHwnd(ToSciterElement(m_Handle), &windowHandle, FALSE);
		return windowHandle;
	}

	bool Element::AttachWindow(wxWindow& window)
	{
		return AttachNativeWindow(window.GetHandle());
	}
	bool Element::AttachNativeWindow(void* handle)
	{
		if (handle)
		{
			return GetSciterAPI()->SciterAttachHwndToElement(ToSciterElement(m_Handle), static_cast<HWND>(handle)) == SCDOM_OK;
		}
		return false;
	}

	wxWindow* Element::DetachWindow()
	{
		wxWindow* window = GetWindow();
		AttachNativeWindow(nullptr);

		return window;
	}
	void* Element::DetachNativeWindow()
	{
		void* window = GetNativeWindow();
		AttachNativeWindow(nullptr);

		return window;
	}

	// Text
	bool Element::HasText() const
	{
		size_t length = 0;
		GetSciterAPI()->SciterGetElementTextCB(ToSciterElement(m_Handle), ExtractStringLength, &length);
		return length != 0;
	}
	String Element::GetText() const
	{
		String result;
		GetSciterAPI()->SciterGetElementTextCB(ToSciterElement(m_Handle), ExtractKxfString, &result);
		return result;
	}
	bool Element::SetText(StringView text) const
	{
		return GetSciterAPI()->SciterSetElementText(ToSciterElement(m_Handle), text.data(), text.length()) == SCDOM_OK;
	}

	// Value
	bool Element::HasValue() const
	{
		return !GetValue().IsNull();
	}
	ScriptValue Element::GetValue() const
	{
		ScriptValue scriptValue;
		if (GetSciterAPI()->SciterGetValue(ToSciterElement(m_Handle), ToSciterScriptValue(scriptValue.GetNativeValue())) == SCDOM_OK)
		{
			return scriptValue;
		}
		return {};
	}
	bool Element::SetValue(const ScriptValue& value) const
	{
		return GetSciterAPI()->SciterSetValue(ToSciterElement(m_Handle), ToSciterScriptValue(value.GetNativeValue())) == SCDOM_OK;
	}

	// Attributes
	bool Element::HasAttribute(const String& name) const
	{
		auto nameUTF8 = name.ToUTF8();
		return HasAttribute(nameUTF8.data());
	}
	bool Element::HasAttribute(const char* name) const
	{
		size_t length = 0;
		return GetSciterAPI()->SciterGetAttributeByNameCB(ToSciterElement(m_Handle), name, ExtractStringLength, &length);
		return length != 0;
	}

	String Element::GetAttribute(const String& name) const
	{
		auto nameUTF8 = name.ToUTF8();
		return GetAttribute(nameUTF8.data());
	}
	String Element::GetAttribute(const char* name) const
	{
		String result;

		GetSciterAPI()->SciterGetAttributeByNameCB(ToSciterElement(m_Handle), name, ExtractKxfString, &result);
		return result;
	}

	size_t Element::GetAttributeCount() const
	{
		UINT count = 0;
		GetSciterAPI()->SciterGetAttributeCount(ToSciterElement(m_Handle), &count);
		return count;
	}
	String Element::GetAttributeNameAt(size_t index) const
	{
		String result;
		GetSciterAPI()->SciterGetNthAttributeNameCB(ToSciterElement(m_Handle), index, ExtractKxfString, &result);
		return result;
	}
	String Element::GetAttributeValueAt(size_t index) const
	{
		String result;
		GetSciterAPI()->SciterGetNthAttributeValueCB(ToSciterElement(m_Handle), index, ExtractKxfString, &result);
		return result;
	}

	bool Element::SetAttribute(const String& name, const String& value)
	{
		auto nameUTF8 = name.ToUTF8();
		return SetAttribute(nameUTF8.data(), value);
	}
	bool Element::SetAttribute(const char* name, const String& value)
	{
		return GetSciterAPI()->SciterSetAttributeByName(ToSciterElement(m_Handle), name, value.wc_str()) == SCDOM_OK;
	}
	bool Element::RemoveAttribute(const String& name)
	{
		auto nameUTF8 = name.ToUTF8();
		return RemoveAttribute(nameUTF8.data());
	}
	bool Element::RemoveAttribute(const char* name)
	{
		return GetSciterAPI()->SciterSetAttributeByName(ToSciterElement(m_Handle), name, nullptr) == SCDOM_OK;
	}
	bool Element::ClearAttributes()
	{
		return GetSciterAPI()->SciterClearAttributes(ToSciterElement(m_Handle)) == SCDOM_OK;
	}

	// Style (CSS) attributes
	bool Element::HasStyleAttribute(const String& name) const
	{
		auto nameUTF8 = name.ToUTF8();
		return HasStyleAttribute(nameUTF8.data());
	}
	bool Element::HasStyleAttribute(const char* name) const
	{
		size_t length = 0;
		return GetSciterAPI()->SciterGetStyleAttributeCB(ToSciterElement(m_Handle), name, ExtractStringLength, &length);
		return length != 0;
	}

	String Element::GetStyleAttribute(const String& name) const
	{
		auto nameUTF8 = name.ToUTF8();
		return GetStyleAttribute(nameUTF8.data());
	}
	String Element::GetStyleAttribute(const char* name) const
	{
		String result;
		GetSciterAPI()->SciterGetStyleAttributeCB(ToSciterElement(m_Handle), name, ExtractKxfString, &result);
		return result;
	}

	std::optional<int> Element::GetStyleAttributeInt(const char* name) const
	{
		return DoGetStyleAttributeInt(*this, name);
	}
	std::optional<int> Element::GetStyleAttributeInt(const String& name) const
	{
		return DoGetStyleAttributeInt(*this, name);
	}

	std::optional<double> Element::GetStyleAttributeFloat(const String& name) const
	{
		return DoGetStyleAttributeFloat(*this, name);
	}
	std::optional<double> Element::GetStyleAttributeFloat(const char* name) const
	{
		return DoGetStyleAttributeFloat(*this, name);
	}

	Color Element::GetStyleAttributeColor(const char* name, ColorSpace* colorSpace) const
	{
		return Color::FromString(GetStyleAttribute(name), colorSpace);
	}
	Color Element::GetStyleAttributeColor(const String& name, ColorSpace* colorSpace) const
	{
		return Color::FromString(GetStyleAttribute(name), colorSpace);
	}

	bool Element::SetStyleAttribute(const String& name, const String& value)
	{
		auto nameUTF8 = name.ToUTF8();
		return SetStyleAttribute(nameUTF8.data(), value);
	}
	bool Element::SetStyleAttribute(const char* name, const String& value)
	{
		return GetSciterAPI()->SciterSetStyleAttribute(ToSciterElement(m_Handle), name, value.wc_str()) == SCDOM_OK;
	}

	bool Element::SetStyleAttribute(const String& name, const Color& value)
	{
		return SetStyleAttribute(name, value.ToString(C2SFormat::CSS, C2SAlpha::Always));
	}
	bool Element::SetStyleAttribute(const char* name, const Color& value)
	{
		return SetStyleAttribute(name, value.ToString(C2SFormat::CSS, C2SAlpha::Always));
	}

	bool Element::SetStyleAttribute(const String& name, int value, SizeUnit unit)
	{
		return SetStyleAttribute(name, Format("{}{}", value, SizeUnitToString(unit)));
	}
	bool Element::SetStyleAttribute(const char* name, int value, SizeUnit unit)
	{
		return SetStyleAttribute(name, Format("{}{}", value, SizeUnitToString(unit)));
	}

	bool Element::SetStyleAttribute(const String& name, double value, SizeUnit unit)
	{
		return SetStyleAttribute(name, Format("{}{}", value, SizeUnitToString(unit)));
	}
	bool Element::SetStyleAttribute(const char* name, double value, SizeUnit unit)
	{
		return SetStyleAttribute(name, Format("{}{}", value, SizeUnitToString(unit)));
	}

	bool Element::RemoveStyleAttribute(const String& name)
	{
		auto nameUTF8 = name.ToUTF8();
		return RemoveStyleAttribute(nameUTF8.data());
	}
	bool Element::RemoveStyleAttribute(const char* name)
	{
		return GetSciterAPI()->SciterSetStyleAttribute(ToSciterElement(m_Handle), name, nullptr) == SCDOM_OK;
	}

	Font Element::GetStyleFont() const
	{
		Font font;

		// Family
		if (String fontFamily = GetStyleAttribute("font-family"); !fontFamily.IsEmpty())
		{
			if (RegEx regEx(wxS(R"("?([\w\s]+)\"?)")); regEx.Matches(fontFamily))
			{
				for (size_t i = 1; i <= regEx.GetMatchCount(); i++)
				{
					String value = regEx.GetMatch(fontFamily, i).TrimBoth();
					if (i == regEx.GetMatchCount())
					{
						if (value == "fantasy")
						{
							font.SetFamily(FontFamily::Fantasy);
						}
						else if (value == "serif")
						{
							font.SetFamily(FontFamily::Serif);
						}
						else if (value == "sans-serif")
						{
							font.SetFamily(FontFamily::SansSerif);
						}
						else if (value == "cursive")
						{
							font.SetFamily(FontFamily::Cursive);
						}
						else if (value == "monospace")
						{
							font.SetFamily(FontFamily::FixedWidth);
						}
					}

					// Set the face name and if such font is installed use it
					font.SetFaceName(value);
					if (font.IsInstalled())
					{
						break;
					}
				}
			}
		}

		// Style
		if (String fontStyle = GetStyleAttribute("font-style"); !fontStyle.IsEmpty())
		{
			if (fontStyle == "normal")
			{
				font.SetStyle(FontStyle::Normal);
			}
			else if (fontStyle == "italic")
			{
				font.SetStyle(FontStyle::Italic);
			}
			else if (fontStyle == "oblique")
			{
				font.SetStyle(FontStyle::Oblique);
			}
		}

		// Text decoration
		if (String textDecoration = GetStyleAttribute("text-decoration"); !textDecoration.IsEmpty())
		{
			if (textDecoration.Contains("underline"))
			{
				font.AddStyle(FontStyle::Underline);
			}
			if (textDecoration.Contains("line-through"))
			{
				font.AddStyle(FontStyle::Strikethrough);
			}
		}

		// Size
		if (auto fontSize = GetStyleAttributeFloat("font-size"))
		{
			font.SetPointSize(static_cast<float>(*fontSize));
		}

		// Weight
		if (auto fontWeight = GetStyleAttributeFloat("font-weight"))
		{
			font.SetNumericWeight(*fontWeight);
		}

		return font;
	}
	bool Element::SetStyleFont(const Font& font)
	{
		if (!IsNull())
		{
			auto MapFamily = [&]() -> String
			{
				switch (font.GetFamily())
				{
					case FontFamily::Fantasy:
					{
						return "fantasy";
					}
					case FontFamily::Serif:
					{
						return "serif";
					}
					case FontFamily::SansSerif:
					{
						return "sans-serif";
					}
					case FontFamily::Cursive:
					{
						return "cursive";
					}
					case FontFamily::FixedWidth:
					{
						return "monospace";
					}
				};
				return {};
			};
			auto MapStyle = [&]() -> String
			{
				const auto style = font.GetStyle();
				if (style.Contains(FontStyle::Italic))
				{
					return "italic";
				}
				else if (style.Contains(FontStyle::Oblique))
				{
					return "oblique";
				}
				return {};
			};
			auto MapTextDecoration = [&]() -> String
			{
				String textDecoration;

				const auto style = font.GetStyle();
				if (style.Contains(FontStyle::Underline))
				{
					textDecoration += "underline";
				}
				if (style.Contains(FontStyle::Strikethrough))
				{
					if (!textDecoration.IsEmpty())
					{
						textDecoration += ' ';
					}
					textDecoration += "line-through";
				}
				return {};
			};

			// Family
			if (String family = MapFamily(); !family.IsEmpty())
			{
				SetStyleAttribute("font-family", Format(R"("{}", {})", font.GetFaceName(), family));
			}
			else
			{
				SetStyleAttribute("font-family", Format(R"("{}")", font.GetFaceName()));
			}

			// Style
			if (String style = MapStyle(); !style.IsEmpty())
			{
				SetStyleAttribute("font-style", style);
			}
			else
			{
				RemoveStyleAttribute("font-style");
			}

			// Text decoration
			if (String textDecoration = MapTextDecoration(); !textDecoration.IsEmpty())
			{
				SetStyleAttribute("text-decoration", textDecoration);
			}
			else
			{
				RemoveStyleAttribute("text-decoration");
			}

			// Size
			SetStyleAttribute("font-size", font.GetPointSize(), SizeUnit::pt);

			// Weight
			SetStyleAttribute("font-weight", font.GetNumericWeight());

			return true;
		}
		return false;
	}

	// Selectors
	size_t Element::Select(const String& query, std::function<bool(Element)> onElement) const
	{
		struct CallContext
		{
			decltype(onElement)& Callback;
			size_t Count = 0;
		};

		CallContext context = {onElement};
		GetSciterAPI()->SciterSelectElementsW(ToSciterElement(m_Handle), query.wc_str(), [](HELEMENT nativeElement, void* context) -> BOOL
		{
			CallContext& callContext = *reinterpret_cast<CallContext*>(context);
			callContext.Count++;

			return !std::invoke(callContext.Callback, FromSciterElement(nativeElement));
		}, &context);
		return context.Count;
	}
	Element Element::SelectAny(const String& query) const
	{
		Element result;
		Select(query, [&](Element element)
		{
			result = std::move(element);
			return false;
		});
		return result;
	}
	Enumerator<Element> Element::SelectAll(const String& query) const
	{
		std::vector<Element> results;
		Select(query, [&](Element element)
		{
			results.emplace_back(std::move(element));
			return true;
		});
		return Utility::EnumerateIndexableContainer<Element>(std::move(results));
	}

	// Widgets
	size_t Element::SelectWidgets(const String& query, std::function<bool(Widget&)> onWidget) const
	{
		auto DoSelectWidgets = [&](const String& query)
		{
			size_t count = 0;
			Select(query, [&](Element element)
			{
				if (Widget* widget = Widget::FromElement(element))
				{
					count++;
					if (!std::invoke(onWidget, *widget))
					{
						return false;
					}
				}
				return true;
			});
			return count;
		};

		if (query.IsEmptyOrWhitespace() || query == '*')
		{
			return DoSelectWidgets("[Widget]");
		}
		else
		{
			return DoSelectWidgets(String("[Widget] ") + query);
		}
	}
	Widget* Element::SelectAnyWidget(const String& query) const
	{
		Widget* result = nullptr;
		SelectWidgets(query, [&](Widget& widget)
		{
			result = &widget;
			return false;
		});
		return result;
	}
	Enumerator<Widget&> Element::SelectAllWidgets(const String& query) const
	{
		std::vector<Widget*> results;
		SelectWidgets(query, [&](Widget& widget)
		{
			results.emplace_back(&widget);
			return true;
		});
		return Utility::EnumerateIndexableContainer<Widget&, Utility::ReferenceOf>(std::move(results));
	}

	// Scripts
	ScriptValue Element::ExecuteScript(const String& script)
	{
		ScriptValue result;
		GetSciterAPI()->SciterEvalElementScript(ToSciterElement(m_Handle), script.wc_str(), script.length(), ToSciterScriptValue(result.GetNativeValue()));
		return result;
	}
}
