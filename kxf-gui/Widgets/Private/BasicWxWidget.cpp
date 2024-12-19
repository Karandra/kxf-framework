#include "kxf-pch.h"
#include "BasicWxWidget.h"
#include "kxf/System/NativeAPI.h"
#include "kxf/Utility/Enumerator.h"
#include "kxf/Utility/String.h"
#include "kxf/wxWidgets/MapCore.h"
#include "kxf-gui/Drawing/GDIRenderer/GDIFont.h"
#include <wx/window.h>
#include <wx/colour.h>
#include "kxf/Win32/Include-GUI.h"

namespace
{
	std::optional<int> MapWindowLongPtrIndex(kxf::NativeWidgetProperty index)
	{
		using namespace kxf;

		switch (index)
		{
			case NativeWidgetProperty::ID:
			{
				return GWLP_ID;
			}
			case NativeWidgetProperty::Style:
			{
				return GWL_STYLE;
			}
			case NativeWidgetProperty::ExStyle:
			{
				return GWL_EXSTYLE;
			}
			case NativeWidgetProperty::Instance:
			{
				return GWLP_HINSTANCE;
			}
			case NativeWidgetProperty::UserData:
			{
				return GWLP_USERDATA;
			}
			case NativeWidgetProperty::WindowProcedure:
			{
				return GWLP_WNDPROC;
			}

			case NativeWidgetProperty::DialogResult:
			{
				return DWLP_MSGRESULT;
			}
			case NativeWidgetProperty::DialogUserData:
			{
				return DWLP_USER;
			}
			case NativeWidgetProperty::DialogProcedure:
			{
				return DWLP_DLGPROC;
			}
		};
		return {};
	}
}

namespace kxf::Private
{
	void BasicWxWidgetBase::OnWindowCreate(wxWindowCreateEvent& event)
	{
		// Look for comment inside 'BasicWxWidgetBase::OnWindowDestroy'
		event.Skip(false);
		event.StopPropagation();

		m_RefLock = m_Widget.shared_from_this();
	}
	void BasicWxWidgetBase::OnWindowDestroy(wxWindowDestroyEvent& event)
	{
		// We must neither skip nor propagate this event to parent widgets. If we do this,
		// the parent widget is going to assume it's being destroyed and the entire widget
		// chain is going to get deleted instead of just a single widget.
		event.Skip(false);
		event.StopPropagation();

		m_ShouldDelete = false;
		Uninitialize();
	}

	void BasicWxWidgetBase::Initialize(std::unique_ptr<wxWindow> window)
	{
		if (!m_Window)
		{
			m_Window = window.release();
			m_ShouldDelete = true;

			AssociateWXObject(*m_Window, m_Widget);
			m_Window->Bind(wxEVT_CREATE, &BasicWxWidgetBase::OnWindowCreate, this);
			m_Window->Bind(wxEVT_DESTROY, &BasicWxWidgetBase::OnWindowDestroy, this);
		}
	}
	void BasicWxWidgetBase::Uninitialize()
	{
		if (wxWindow* window = std::exchange(m_Window, nullptr))
		{
			window->Unbind(wxEVT_CREATE, &BasicWxWidgetBase::OnWindowCreate, this);
			window->Unbind(wxEVT_DESTROY, &BasicWxWidgetBase::OnWindowDestroy, this);
			DissociateWXObject(*window);

			if (m_ShouldDelete)
			{
				window->Destroy();
			}
		}
		m_RefLock = nullptr;
		m_ShouldDelete = false;
	}

	// Native interface
	void* BasicWxWidgetBase::GetHandle() const noexcept
	{
		return m_Window ? static_cast<void*>(m_Window->GetHandle()) : nullptr;
	}

	// Lifetime management
	bool BasicWxWidgetBase::IsWidgetAlive() const noexcept
	{
		return m_Window && !m_Window->IsBeingDeleted();
	}
	bool BasicWxWidgetBase::CloseWidget(bool force)
	{
		if (m_Window)
		{
			return m_Window->Close(force);
		}
		return false;
	}
	bool BasicWxWidgetBase::DestroyWidget()
	{
		if (m_Window)
		{
			m_ShouldDelete = false;
			return m_Window->Destroy();
		}
		return false;
	}

	// HiDPI support
	float BasicWxWidgetBase::GetContentScaleFactor() const
	{
		return static_cast<float>(m_Window->GetDPIScaleFactor());
	}

	void BasicWxWidgetBase::FromDIP(int& x, int& y) const
	{
		auto temp = m_Window->FromDIP(wxPoint(x, y));
		x = temp.x;
		y = temp.y;
	}
	void BasicWxWidgetBase::ToDIP(int& x, int& y) const
	{
		auto temp = m_Window->ToDIP(wxPoint(x, y));
		x = temp.x;
		y = temp.y;
	}

	// Positioning functions
	Point BasicWxWidgetBase::GetPosition() const
	{
		return Point(m_Window->GetPosition());
	}
	void BasicWxWidgetBase::SetPosition(const Point& pos)
	{
		m_Window->SetPosition(pos);
	}

	void BasicWxWidgetBase::Center(FlagSet<Orientation> orientation)
	{
		m_Window->Center(*wxWidgets::MapOrientation(orientation));
	}
	void BasicWxWidgetBase::CenterOnParent(FlagSet<Orientation> orientation)
	{
		m_Window->CenterOnParent(*wxWidgets::MapOrientation(orientation));
	}

	// Size functions
	Rect BasicWxWidgetBase::GetRect(WidgetSizeFlag sizeType) const
	{
		return Rect([&]() -> wxRect
		{
			switch (sizeType)
			{
				case WidgetSizeFlag::Widget:
				{
					return m_Window->GetRect();
				}
				case WidgetSizeFlag::WidgetMin:
				{
					return {m_Window->GetPosition(), m_Window->GetMinSize()};
				}
				case WidgetSizeFlag::WidgetMax:
				{
					return {m_Window->GetPosition(), m_Window->GetMaxSize()};
				}
				case WidgetSizeFlag::WidgetBest:
				{
					return {m_Window->GetPosition(), m_Window->GetBestSize()};
				}

				case WidgetSizeFlag::Client:
				{
					return m_Window->GetClientRect();
				}
				case WidgetSizeFlag::ClientMin:
				{
					return {m_Window->GetClientRect().GetPosition(), m_Window->GetMinClientSize()};
				}
				case WidgetSizeFlag::ClientMax:
				{
					return {m_Window->GetClientRect().GetPosition(), m_Window->GetMaxClientSize()};
				}

				case WidgetSizeFlag::Virtual:
				{
					return {m_Window->GetClientRect().GetPosition(), m_Window->GetVirtualSize()};
				}
				case WidgetSizeFlag::VirtualBest:
				{
					return {m_Window->GetClientRect().GetPosition(), m_Window->GetBestVirtualSize()};
				}

				case WidgetSizeFlag::Border:
				{
					return {m_Window->GetClientRect().GetPosition(), m_Window->GetWindowBorderSize()};
				}
			}
			return {wxDefaultCoord, wxDefaultCoord, wxDefaultCoord, wxDefaultCoord};
		}());
	}
	Size BasicWxWidgetBase::GetSize(WidgetSizeFlag sizeType) const
	{
		return Size([&]() -> wxSize
		{
			switch (sizeType)
			{
				case WidgetSizeFlag::Widget:
				{
					return m_Window->GetSize();
				}
				case WidgetSizeFlag::WidgetMin:
				{
					return m_Window->GetMinSize();
				}
				case WidgetSizeFlag::WidgetMax:
				{
					return m_Window->GetMaxSize();
				}
				case WidgetSizeFlag::WidgetBest:
				{
					return m_Window->GetBestSize();
				}

				case WidgetSizeFlag::Client:
				{
					return m_Window->GetClientSize();
				}
				case WidgetSizeFlag::ClientMin:
				{
					return m_Window->GetMinClientSize();
				}
				case WidgetSizeFlag::ClientMax:
				{
					return m_Window->GetMaxClientSize();
				}

				case WidgetSizeFlag::Virtual:
				{
					return m_Window->GetVirtualSize();
				}
				case WidgetSizeFlag::VirtualBest:
				{
					return m_Window->GetBestVirtualSize();
				}

				case WidgetSizeFlag::Border:
				{
					return m_Window->GetWindowBorderSize();
				}
			}
			return wxDefaultSize;
		}());
	}
	void BasicWxWidgetBase::SetSize(const Size& size, FlagSet<WidgetSizeFlag> flags)
	{
		if (flags.Contains(WidgetSizeFlag::Widget))
		{
			m_Window->SetSize(size);
		}
		if (flags.Contains(WidgetSizeFlag::WidgetMin))
		{
			m_Window->SetMinSize(size);
		}
		if (flags.Contains(WidgetSizeFlag::WidgetMax))
		{
			m_Window->SetMaxSize(size);
		}
		if (flags.Contains(WidgetSizeFlag::WidgetBest))
		{
			if (size.IsFullySpecified())
			{
				m_Window->CacheBestSize(size);
			}
			else
			{
				m_Window->InvalidateBestSize();
			}
		}

		if (flags.Contains(WidgetSizeFlag::Client))
		{
			m_Window->SetClientSize(size);
		}
		if (flags.Contains(WidgetSizeFlag::ClientMin))
		{
			m_Window->SetMinClientSize(size);
		}
		if (flags.Contains(WidgetSizeFlag::ClientMax))
		{
			m_Window->SetMaxClientSize(size);
		}

		if (flags.Contains(WidgetSizeFlag::Virtual))
		{
			m_Window->SetVirtualSize(size);
		}
	}

	// Coordinate conversion functions
	void BasicWxWidgetBase::ScreenToClient(int& x, int& y) const
	{
		m_Window->ScreenToClient(&x, &y);
	}
	void BasicWxWidgetBase::ClientToScreen(int& x, int& y) const
	{
		m_Window->ClientToScreen(&x, &y);
	}

	void BasicWxWidgetBase::DialogUnitsToPixels(int& x, int& y) const
	{
		auto temp = m_Window->ConvertDialogToPixels(wxPoint(x, y));
		x = temp.x;
		y = temp.y;
	}
	void BasicWxWidgetBase::PixelsToDialogUnits(int& x, int& y) const
	{
		auto temp = m_Window->ConvertPixelsToDialog(wxPoint(x, y));
		x = temp.x;
		y = temp.y;
	}

	// Focus
	bool BasicWxWidgetBase::IsFocusable() const
	{
		return m_Window->IsFocusable();
	}
	bool BasicWxWidgetBase::HasFocus() const
	{
		return m_Window->HasFocus();
	}
	void BasicWxWidgetBase::SetFocus()
	{
		m_Window->SetFocus();
	}

	bool BasicWxWidgetBase::IsFocusVisible() const
	{
		return m_VisibleFocusEnabled;
	}
	void BasicWxWidgetBase::SetFocusVisible(bool visible)
	{
		m_VisibleFocusEnabled = visible;
		m_Window->EnableVisibleFocus(visible);
	}

	// Layout
	LayoutDirection BasicWxWidgetBase::GetLayoutDirection() const
	{
		switch (m_Window->GetLayoutDirection())
		{
			case wxLayoutDirection::wxLayout_LeftToRight:
			{
				return LayoutDirection::LeftToRight;
			}
			case wxLayoutDirection::wxLayout_RightToLeft:
			{
				return LayoutDirection::RightToLeft;
			}
		};
		return LayoutDirection::Default;
	}
	void BasicWxWidgetBase::SetLayoutDirection(LayoutDirection direction)
	{
		switch (direction)
		{
			case LayoutDirection::LeftToRight:
			{
				m_Window->SetLayoutDirection(wxLayoutDirection::wxLayout_LeftToRight);
				break;
			}
			case LayoutDirection::RightToLeft:
			{
				m_Window->SetLayoutDirection(wxLayoutDirection::wxLayout_RightToLeft);
				break;
			}
			default:
			{
				m_Window->SetLayoutDirection(wxLayoutDirection::wxLayout_Default);
				break;
			}
		};
	}

	void BasicWxWidgetBase::Fit()
	{
		m_Window->Fit();
	}
	void BasicWxWidgetBase::FitInterior()
	{
		m_Window->FitInside();
	}
	bool BasicWxWidgetBase::Layout()
	{
		return m_Window->Layout();
	}

	// Child management functions
	void BasicWxWidgetBase::AddChildWidget(IWidget& widget)
	{
		m_Window->AddChild(widget.GetWxWindow());
	}
	void BasicWxWidgetBase::RemoveChildWidget(const IWidget& widget)
	{
		m_Window->RemoveChild(const_cast<wxWindow*>(widget.GetWxWindow()));
	}
	void BasicWxWidgetBase::DestroyChildWidgets()
	{
		m_Window->DestroyChildren();
	}

	std::shared_ptr<IWidget> BasicWxWidgetBase::FindChildWidgetByID(WidgetID id) const
	{
		if (auto window = m_Window->FindWindow(id.ToInt()))
		{
			return FindByWXObject(*window);
		}
		return nullptr;
	}
	std::shared_ptr<IWidget> BasicWxWidgetBase::FindChildWidgetByName(const String& widgetName) const
	{
		if (auto window = m_Window->FindWindow(widgetName))
		{
			return FindByWXObject(*window);
		}
		return nullptr;
	}
	Enumerator<std::shared_ptr<IWidget>> BasicWxWidgetBase::EnumChildWidgets() const
	{
		return Utility::EnumerateIterableContainer<std::shared_ptr<IWidget>>(m_Window->GetChildren(), [](wxWindow* window)
		{
			return FindByWXObject(*window);
		});
	}

	// Sibling and parent management functions
	std::shared_ptr<IWidget> BasicWxWidgetBase::GetParentWidget() const
	{
		if (auto window = m_Window->GetParent())
		{
			return FindByWXObject(*window);
		}
		return nullptr;
	}
	void BasicWxWidgetBase::SetParentWidget(IWidget& widget)
	{
		m_Window->Reparent(widget.GetWxWindow());
	}

	std::shared_ptr<IWidget> BasicWxWidgetBase::GetPrevSiblingWidget() const
	{
		if (auto window = m_Window->GetPrevSibling())
		{
			return FindByWXObject(*window);
		}
		return nullptr;
	}
	std::shared_ptr<IWidget> BasicWxWidgetBase::GetNextSiblingWidget() const
	{
		if (auto window = m_Window->GetNextSibling())
		{
			return FindByWXObject(*window);
		}
		return nullptr;
	}

	// Drawing-related functions
	bool BasicWxWidgetBase::IsFrozen() const
	{
		return m_Window->IsFrozen();
	}
	void BasicWxWidgetBase::Freeze()
	{
		m_Window->Freeze();
	}
	void BasicWxWidgetBase::Thaw()
	{
		m_Window->Thaw();
	}

	Rect BasicWxWidgetBase::GetRefreshRect() const
	{
		return Rect(m_Window->GetUpdateClientRect());
	}
	void BasicWxWidgetBase::Refresh(const Rect& rect)
	{
		if (rect.IsFullySpecified())
		{
			m_Window->RefreshRect(rect);
		}
		else
		{
			m_Window->Refresh();
		}
	}

	Font BasicWxWidgetBase::GetFont() const
	{
		return m_Window->GetFont();
	}
	void BasicWxWidgetBase::SetFont(const Font& font)
	{
		m_Window->SetFont(font.ToWXFont());
	}

	Color BasicWxWidgetBase::GetColor(WidgetColorFlag colorType) const
	{
		switch (colorType)
		{
			case WidgetColorFlag::Background:
			{
				return m_Window->GetBackgroundColour();
			}
			case WidgetColorFlag::Text:
			case WidgetColorFlag::Foreground:
			{
				return m_Window->GetForegroundColour();
			}
		};
		return {};
	}
	void BasicWxWidgetBase::SetColor(const Color& color, FlagSet<WidgetColorFlag> flags)
	{
		if (flags.Contains(WidgetColorFlag::Background))
		{
			m_Window->SetBackgroundColour(color);
		}
		if (flags.Contains(WidgetColorFlag::Foreground) || flags.Contains(WidgetColorFlag::Text))
		{
			m_Window->SetForegroundColour(color);
		}
	}

	float BasicWxWidgetBase::GetTransparency() const
	{
		return m_Transparency;
	}
	bool BasicWxWidgetBase::SetTransparency(float value)
	{
		if (m_Window->CanSetTransparent())
		{
			m_Transparency = std::clamp(value, 0.0f, 1.0f);
			return m_Window->SetTransparent(static_cast<wxByte>(255 - (m_Transparency * 255)));
		}
		return false;
	}

	// Widget state and visibility functions
	bool BasicWxWidgetBase::IsEnabled() const
	{
		return m_Window->IsEnabled();
	}
	bool BasicWxWidgetBase::IsIntrinsicallyEnabled() const
	{
		return m_Window->IsThisEnabled();
	}
	void BasicWxWidgetBase::SetEnabled(bool enabled)
	{
		m_Window->Enable(enabled);
	}

	bool BasicWxWidgetBase::IsVisible() const
	{
		return m_Window->IsShown();
	}
	bool BasicWxWidgetBase::IsDisplayed() const
	{
		return m_Window->IsShownOnScreen();
	}
	void BasicWxWidgetBase::SetVisible(bool visible)
	{
		m_Window->Show(visible);
	}

	// Widget style functions
	FlagSet<WidgetStyle> BasicWxWidgetBase::GetWidgetStyle() const
	{
		const FlagSet<int> wx = m_Window->GetWindowStyle();

		FlagSet<WidgetStyle> style;
		style.Add(WidgetStyle::Invisible, wx.Contains(wxTRANSPARENT_WINDOW));
		style.Add(WidgetStyle::AllKeyEvents, wx.Contains(wxWANTS_CHARS));
		style.Add(WidgetStyle::ScrollVertical, wx.Contains(wxVSCROLL));
		style.Add(WidgetStyle::ScrollHorizontal, wx.Contains(wxHSCROLL));
		style.Add(WidgetStyle::ScrollShowAlways, wx.Contains(wxALWAYS_SHOW_SB));

		return style;
	}
	void BasicWxWidgetBase::SetWidgetStyle(FlagSet<WidgetStyle> style)
	{
		FlagSet<int> wx = m_Window->GetWindowStyle();
		wx.Mod(wxTRANSPARENT_WINDOW, style.Contains(WidgetStyle::Invisible));
		wx.Mod(wxWANTS_CHARS, style.Contains(WidgetStyle::AllKeyEvents));
		wx.Mod(wxVSCROLL, style.Contains(WidgetStyle::ScrollVertical));
		wx.Mod(wxHSCROLL, style.Contains(WidgetStyle::ScrollHorizontal));
		wx.Mod(wxALWAYS_SHOW_SB, style.Contains(WidgetStyle::ScrollShowAlways));

		m_Window->SetWindowStyle(*wx);
	}

	FlagSet<WidgetExStyle> BasicWxWidgetBase::GetWidgetExStyle() const
	{
		const FlagSet<int> wx = m_Window->GetWindowStyle();

		FlagSet<WidgetExStyle> style;
		return style;
	}
	void BasicWxWidgetBase::SetWidgetExStyle(FlagSet<WidgetExStyle> style)
	{
		FlagSet<int> wx = m_Window->GetExtraStyle();
		m_Window->SetExtraStyle(*wx);
	}

	WidgetBorder BasicWxWidgetBase::GetWidgetBorder() const
	{
		switch (m_Window->GetBorder())
		{
			case wxBorder::wxBORDER_NONE:
			{
				return WidgetBorder::None;
			}
			case wxBorder::wxBORDER_SIMPLE:
			{
				return WidgetBorder::Simple;
			}
			case wxBorder::wxBORDER_STATIC:
			{
				return WidgetBorder::Simple;
			}
			case wxBorder::wxBORDER_THEME:
			{
				return WidgetBorder::Theme;
			}
		}
		return WidgetBorder::Default;
	}
	void BasicWxWidgetBase::SetWidgetBorder(WidgetBorder border)
	{
		FlagSet<int> style = m_Window->GetWindowStyle();

		style.Remove(wxBORDER_SUNKEN);
		style.Remove(wxBORDER_RAISED);
		style.Mod(wxBORDER_NONE, border == WidgetBorder::None);
		style.Mod(wxBORDER_DEFAULT, border == WidgetBorder::Default);
		style.Mod(wxBORDER_STATIC, border == WidgetBorder::Static);
		style.Mod(wxBORDER_THEME, border == WidgetBorder::Theme);

		m_Window->SetWindowStyle(*style);
	}

	// Widget properties
	WidgetID BasicWxWidgetBase::GetWidgetID() const
	{
		return m_Window->GetId();
	}
	void BasicWxWidgetBase::SetWidgetID(WidgetID id)
	{
		m_Window->SetId(id.ToInt());
	}

	String BasicWxWidgetBase::GetWidgetName() const
	{
		return m_Window->GetName();
	}
	void BasicWxWidgetBase::SetWidgetName(const String& widgetName)
	{
		m_Window->SetName(widgetName);
	}
	String BasicWxWidgetBase::GetWidgetText(FlagSet<WidgetTextFlag> flags) const
	{
		return m_Window->GetLabel();
	}
	void BasicWxWidgetBase::SetWidgetText(const String& widgetText, FlagSet<WidgetTextFlag> flags)
	{
		m_Window->SetLabel(widgetText);
	}

	// INativeWidget
	void* BasicWxWidgetBase::GetNativeHandle() const
	{
		return BasicWxWidgetBase::GetHandle();
	}
	String BasicWxWidgetBase::GetIntrinsicText() const
	{
		size_t length = ::InternalGetWindowText(m_Window->GetHandle(), nullptr, 0);
		if (length != 0)
		{
			String text;
			::InternalGetWindowText(m_Window->GetHandle(), Utility::StringBuffer(text, length - 1), length);

			return text;
		}
		return {};
	}

	intptr_t BasicWxWidgetBase::GetWindowProperty(NativeWidgetProperty index) const
	{
		if (auto nativeIndex = MapWindowLongPtrIndex(index))
		{
			return static_cast<intptr_t>(::GetWindowLongPtrW(m_Window->GetHandle(), *nativeIndex));
		}
		return 0;
	}
	intptr_t BasicWxWidgetBase::SetWindowProperty(NativeWidgetProperty index, intptr_t value)
	{
		if (auto nativeIndex = MapWindowLongPtrIndex(index))
		{
			return static_cast<intptr_t>(::SetWindowLongPtrW(m_Window->GetHandle(), *nativeIndex, value));
		}
		return 0;
	}

	bool BasicWxWidgetBase::PostMessage(uint32_t messageID, intptr_t wParam, intptr_t lParam)
	{
		return ::PostMessageW(m_Window->GetHandle(), messageID, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam));
	}
	bool BasicWxWidgetBase::NotifyMessage(uint32_t messageID, intptr_t wParam, intptr_t lParam)
	{
		return ::SendNotifyMessageW(m_Window->GetHandle(), messageID, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam));
	}
	bool BasicWxWidgetBase::SendMessageSignal(uint32_t messageID, intptr_t wParam, intptr_t lParam)
	{
		return ::SendMessageCallbackW(m_Window->GetHandle(), messageID, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam), [](HWND handle, UINT messageID, ULONG_PTR context, LRESULT result)
		{
			IWidget& widget = reinterpret_cast<BasicWxWidgetBase*>(context)->m_Widget;
			widget.ProcessSignal(&INativeWidget::OnMessageProcessed, handle, messageID, result);
		}, reinterpret_cast<ULONG_PTR>(this));
	}
	intptr_t BasicWxWidgetBase::SendMessage(uint32_t messageID, intptr_t wParam, intptr_t lParam, TimeSpan timeout)
	{
		if (timeout.IsPositive())
		{
			DWORD_PTR result = 0;
			constexpr FlagSet<int> flags = SMTO_NORMAL|SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT;
			::SendMessageTimeoutW(m_Window->GetHandle(), messageID, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam), *flags, timeout.GetMilliseconds(), &result);

			return result;
		}
		else
		{
			return ::SendMessageW(m_Window->GetHandle(), messageID, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam));
		}
	}

	HResult BasicWxWidgetBase::SetWindowTheme(const String& applicationName, const std::vector<String>& subIDs)
	{
		if (!subIDs.empty())
		{
			String subIDString;
			for (const auto& id: subIDs)
			{
				if (!subIDString.IsEmpty())
				{
					subIDString += ';';
				}
				subIDString += id;
			}

			return ::SetWindowTheme(m_Window->GetHandle(), applicationName.wc_str(), subIDString.wc_str());
		}
		else
		{
			return ::SetWindowTheme(m_Window->GetHandle(), applicationName.wc_str(), nullptr);
		}
	}
	bool BasicWxWidgetBase::EnableNonClientDPIScaling()
	{
		if (NativeAPI::User32::EnableNonClientDpiScaling)
		{
			return NativeAPI::User32::EnableNonClientDpiScaling(m_Window->GetHandle());
		}
		return false;
	}

	bool BasicWxWidgetBase::IsForegroundWindow() const
	{
		return m_Window->GetHandle() == ::GetForegroundWindow();
	}
	bool BasicWxWidgetBase::SetForegroundWindow()
	{
		return ::SetForegroundWindow(m_Window->GetHandle());
	}
}
