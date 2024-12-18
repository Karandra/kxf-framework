#include "kxf-pch.h"
#include "Host.h"
#include "SciterAPI.h"
#include "Stylesheets/MasterStylesheetStorage.h"
#include "kxf/Sciter/Private/Conversion.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/Utility/Common.h"
#include <WindowsX.h>
#include <Uxtheme.h>
#include "kxf/System/UndefWindows.h"

#pragma warning(disable: 4302) // 'reinterpret_cast': truncation from 'void *' to 'UINT'
#pragma warning(disable: 4311) // 'reinterpret_cast': pointer truncation from 'void *' to 'UINT'

namespace kxf::Sciter
{
	template<class TEvent>
	TEvent MakeEvent(Host& host)
	{
		TEvent event(host);
		event.Allow();
		event.SetEventSource(host.GetEvtHandler().QueryInterface<IEvtHandler>());

		return event;
	}
}

namespace kxf::Sciter
{
	void Host::SetDefaultOptions()
	{
		EnableSystemTheme(m_Style.Contains(HostStyle::SystemTheme));
		EnableSmoothScrolling(m_Style.Contains(HostStyle::SmoothScrolling));
		SetFontSmoothingMode(FontSmoothing::SystemDefault);

		GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_HTTPS_ERROR, 1);
		GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_SET_DEBUG_MODE, m_Style.Contains(HostStyle::AllowDebug));
		GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_CONNECTION_TIMEOUT, 20);
		GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_SET_GFX_LAYER, GFX_LAYER_AUTO);
		GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_TRANSPARENT_WINDOW, static_cast<uintptr_t>(true));
		GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_SET_SCRIPT_RUNTIME_FEATURES, ALLOW_FILE_IO|ALLOW_SOCKET_IO|ALLOW_EVAL|ALLOW_SYSINFO);
	}
	std::pair<FlagSet<int>, FlagSet<int>> Host::UpdateWindowStyle()
	{
		FlagSet<int> style = m_SciterWindow.GetWindowStyle();
		FlagSet<int> exStyle = m_SciterWindow.GetExtraStyle();

		// We don't need this as Sciter paints the entire window itself when needed
		if (style.Contains(wxFULL_REPAINT_ON_RESIZE))
		{
			m_SciterWindow.SetWindowStyle(*exStyle.Remove(wxFULL_REPAINT_ON_RESIZE));
		}

		// Add 'wxWS_EX_PROCESS_IDLE'. We always need to process idle, especially when using non-default renderer
		// because we're drawing the window content when idle. We don't require 'wxWS_EX_PROCESS_UI_UPDATES' though.
		if (!exStyle.Contains(wxWS_EX_PROCESS_IDLE))
		{
			m_SciterWindow.SetExtraStyle(*exStyle.Add(wxWS_EX_PROCESS_IDLE));
		}

		return {style, exStyle};
	}

	void Host::OnEngineCreated()
	{
		// Window options
		::SetWindowLongPtrW(m_SciterWindow.GetHandle(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		m_SciterWindow.SetBackgroundStyle(wxBG_STYLE_PAINT);

		// Create renderer
		m_Renderer = IWindowRenderer::CreateInstance(m_Option_WindowRenderer, *this);
		if (m_Renderer)
		{
			m_Renderer->Create();
		}

		// Sciter options
		SetDefaultOptions();
		m_EventDispatcher.AttachHost();

		// Send event
		SciterEvent event = MakeEvent<SciterEvent>(*this);
		ProcessEvent(event, EvtEngineCreated);
	}
	void Host::OnEngineDestroyed()
	{
		::SetWindowLongPtrW(m_SciterWindow.GetHandle(), GWLP_USERDATA, 0);

		SciterEvent event = MakeEvent<SciterEvent>(*this);
		ProcessEvent(event, EvtEngineDestroyed);

		m_EventDispatcher.DetachHost();
	}
	void Host::OnDocumentChanged()
	{
		m_SciterWindow.InvalidateBestSize();
	}
	bool Host::ProcessEvent(IEvent& event, const EventID& eventID)
	{
		//return m_SciterWindow.ProcessWindowEvent(event, eventID) && !event.IsSkipped();
		return m_EvtHandler.ProcessEvent(event, eventID, ProcessEventFlag::HandleExceptions) && !event.IsSkipped();
	}

	void Host::AttachElementHandler(Element& element)
	{
		m_EventDispatcher.AttachElement(element);
	}
	void Host::DetachElementHandler(Element& element)
	{
		m_EventDispatcher.DetachElement(element);
	}
	void Host::AttachElementHandler(Element& element, IEvtHandler& evtHandler)
	{
		auto [it, newItem] = m_ElementEventDispatchers.insert_or_assign(&evtHandler, std::make_unique<EventDispatcher>(*this, evtHandler));
		it->second->AttachElement(element);
	}
	void Host::DetachElementHandler(Element& element, IEvtHandler& evtHandler)
	{
		if (auto it = m_ElementEventDispatchers.find(&evtHandler); it != m_ElementEventDispatchers.end())
		{
			it->second->DetachElement(element);
			m_ElementEventDispatchers.erase(it);
		}
	}

	bool Host::SciterPreHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
	{
		if (m_AllowSciterHandleMessage)
		{
			bool allowSciter = true;
			bool messageHandled = false;
			if (m_Option_WindowRenderer != WindowRenderer::Default && (msg == WM_CREATE || msg == WM_PAINT))
			{
				allowSciter = false;
			}
			if (msg == WM_PAINT && m_SciterWindow.IsFrozen())
			{
				allowSciter = false;
			}

			// Forward messages to Sciter
			if (allowSciter)
			{
				std::optional<FPSCounter::Watcher> fpsWatcher;
				if (msg == WM_PAINT)
				{
					fpsWatcher.emplace(m_FrameCounter.CreateWatcher());
				}

				BOOL handled = FALSE;
				*result = GetSciterAPI()->SciterProcND(m_SciterWindow.GetHandle(), msg, wParam, lParam, &handled);
				messageHandled = handled;
			}

			// Handle engine creation and renderer callbacks
			switch (msg)
			{
				case WM_CREATE:
				{
					m_EngineCreated = true;
					OnEngineCreated();
					break;
				}
				case WM_SIZE:
				{
					if (m_Renderer)
					{
						m_Renderer->OnSize();
					}
					break;
				}
				case WM_PAINT:
				{
					if (m_Renderer)
					{
						m_Renderer->Render();
					}
					break;
				}
				case WM_DESTROY:
				{
					OnEngineDestroyed();
					m_EngineCreated = false;
					m_AllowSciterHandleMessage = false;
					break;
				}
			};
			return messageHandled;
		}
		return false;
	}
	bool Host::SciterPostHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
	{
		return false;
	}
	void Host::OnInternalIdle()
	{
		if (m_ReloadScheduled)
		{
			if (auto root = GetRootElement())
			{
				String path = m_DocumentPath;
				LoadHTML(root.GetOuterHTML(), m_DocumentBasePath);
				m_DocumentPath = std::move(path);
			}
			m_ReloadScheduled = false;
		}

		if (m_Renderer)
		{
			m_Renderer->OnIdle();
		}
	}

	Host::Host(wxWindow& window, EvtHandler& evtHandler, FlagSet<HostStyle> style)
		:m_SciterWindow(window), m_EvtHandler(evtHandler), m_EventDispatcher(*this, evtHandler), m_Style(style)
	{
		if (m_SciterWindow.GetHandle())
		{
			::SetWindowLongPtrW(m_SciterWindow.GetHandle(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		}

		// Child windows are fine with already created window. They don't need 'WS_EX_NOREDIRECTIONBITMAP' style.
		// Use of desktop composition requires 'WS_EX_NOREDIRECTIONBITMAP' style.
		if (!m_SciterWindow.IsTopLevel() && !m_Style.Contains(HostStyle::DesktopComposition))
		{
			UpdateWindowStyle();
			m_AllowSciterHandleMessage = true;
		}
	}
	Host::~Host()
	{
	}

	bool Host::Create()
	{
		if (!m_EngineCreated && m_SciterWindow.IsTopLevel())
		{
			// Get original window info
			auto [style, exStyle] = UpdateWindowStyle();
			auto [nativeStyle, nativeExStyle] = [&]()
			{
				DWORD nativeExStyle = 0;
				DWORD nativeStyle = m_SciterWindow.MSWGetStyle(*style, &nativeExStyle);

				return std::make_pair(FlagSet<DWORD>(nativeStyle), FlagSet<DWORD>(nativeExStyle));
			}();

			const XChar* nativeClassName = m_SciterWindow.GetMSWClassName(*style);
			const Point pos = Point(m_SciterWindow.GetPosition()).SetDefaults({CW_USEDEFAULT, CW_USEDEFAULT});
			const Size size = Size(m_SciterWindow.GetSize()).SetDefaults({CW_USEDEFAULT, CW_USEDEFAULT});
			const String title = m_SciterWindow.GetLabel();

			// Destroy and detach original window
			const HWND oldHandle = m_SciterWindow.GetHandle();
			m_SciterWindow.DestroyChildren();
			m_SciterWindow.DissociateHandle();
			::DestroyWindow(oldHandle);

			// Create new window with 'WS_EX_NOREDIRECTIONBITMAP' extended style instead (if the style is supported) and attach it to the wxWindow
			nativeExStyle.Add(WS_EX_NOREDIRECTIONBITMAP, m_Style.Contains(HostStyle::DesktopComposition) && System::IsWindowsVersionOrGreater(NamedSystemRelease::Windows8));

			m_AllowSciterHandleMessage = true;
			m_SciterWindow.MSWCreate(nativeClassName, title.wc_str(), pos, size, *nativeStyle, *nativeExStyle);
			return m_EngineCreated;
		}
		return true;
	}
	void Host::Update()
	{
		GetSciterAPI()->SciterUpdateWindow(m_SciterWindow.GetHandle());
		if (m_Renderer)
		{
			m_Renderer->Render();
		}
	}
	void Host::Reload()
	{
		m_ReloadScheduled = true;
	}

	Size Host::GetBestSize() const
	{
		const int paddingX = wxSystemSettings::GetMetric(wxSYS_SMALLICON_X, &m_SciterWindow);
		const int paddingY = wxSystemSettings::GetMetric(wxSYS_SMALLICON_Y, &m_SciterWindow);

		const int width = GetSciterAPI()->SciterGetMinWidth(m_SciterWindow.GetHandle()) + paddingX;
		const int height = GetSciterAPI()->SciterGetMinHeight(m_SciterWindow.GetHandle(), width) + paddingY;

		return Size(m_SciterWindow.FromDIP(Size(width, height)));
	}
	Size Host::GetDPI() const
	{
		UINT x = 0;
		UINT y = 0;
		GetSciterAPI()->SciterGetPPI(m_SciterWindow.GetHandle(), &x, &y);

		return Size(x, y);
	}
	double Host::GetFPS() const
	{
		return m_Renderer ? m_Renderer->GetFPS() : m_FrameCounter.GetCount();
	}

	bool Host::IsSystemThemeEnabled() const
	{
		return m_Style.Contains(HostStyle::SystemTheme);
	}
	bool Host::EnableSystemTheme(bool enable)
	{
		::SetWindowTheme(m_SciterWindow.GetHandle(), enable ? L"Explorer" : nullptr, nullptr);

		m_Style.Mod(HostStyle::SystemTheme, enable);
		return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_SET_UX_THEMING, enable);
	}

	bool Host::IsSmoothScrollingEnabled() const
	{
		return m_Style.Contains(HostStyle::SmoothScrolling);
	}
	bool Host::EnableSmoothScrolling(bool enable)
	{
		m_Style.Mod(HostStyle::SmoothScrolling, enable);
		return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_SMOOTH_SCROLL, enable);
	}

	FontSmoothing Host::GetFontSommthingMode() const
	{
		return m_Option_FontSmoothing;
	}
	bool Host::SetFontSmoothingMode(FontSmoothing mode)
	{
		switch (mode)
		{
			case FontSmoothing::None:
			{
				return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 1);
			}
			case FontSmoothing::SystemDefault:
			{
				return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 0);
			}
			case FontSmoothing::Standard:
			{
				return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 2);
			}
			case FontSmoothing::ClearType:
			{
				return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 3);
			}
		};
		return false;
	}

	bool Host::SetWindowFrame(WindowFrame windowFrame)
	{
		if (!m_SciterWindow.IsTopLevel())
		{
			return false;
		}

		auto SetAttribute = [&](const String& value)
		{
			if (GetRootElement().SetAttribute("window-frame", value))
			{
				Reload();
				return true;
			}
			return false;
		};

		switch (windowFrame)
		{
			case WindowFrame::Standard:
			{
				return SetAttribute("none");
			}
			case WindowFrame::Solid:
			{
				return SetAttribute("solid");
			}
			case WindowFrame::SolidWithShadow:
			{
				return SetAttribute("solid-with-shadow");
			}
			case WindowFrame::Extended:
			{
				return SetAttribute("extended");
			}
			case WindowFrame::Transparent:
			{
				return SetAttribute("transparent");
			}
		};
		return false;
	}
	bool Host::SetBlurBehind(WindowBlurBehind blurMode)
	{
		if (!m_SciterWindow.IsTopLevel())
		{
			return false;
		}

		auto SetAttribute = [&](const String& value)
		{
			GetRootElement().SetAttribute("window-blurbehind", value);
			if (!value.IsEmpty())
			{
				return !ExecuteScript(Format("view.windowBlurbehind = #{};", value)).IsUndefined();
			}
			return !ExecuteScript("view.windowBlurbehind = #none;").IsUndefined();
		};

		switch (blurMode)
		{
			case WindowBlurBehind::None:
			{
				return SetAttribute({});
			}
			case WindowBlurBehind::Auto:
			{
				return SetAttribute("auto");
			}
			case WindowBlurBehind::Light:
			{
				return SetAttribute("light");
			}
			case WindowBlurBehind::UltraLight:
			{
				return SetAttribute("ultra-light");
			}
			case WindowBlurBehind::Dark:
			{
				return SetAttribute("dark");
			}
			case WindowBlurBehind::UltraDark:
			{
				return SetAttribute("ultra-dark");
			}
		};
		return false;
	}

	WindowRenderer Host::GetWindowRenderer() const
	{
		return m_Option_WindowRenderer;
	}
	bool Host::SetWindowRenderer(WindowRenderer renderer)
	{
		m_Option_WindowRenderer = renderer;
		return !m_EngineCreated;
	}

	bool Host::IsTransparentBackgroundSupported(wxString* reason) const
	{
		if (m_SciterWindow.IsTopLevel())
		{
			Utility::SetIfNotNull(reason, "Always supported for top-level windows");
			return true;
		}
		else if (m_Renderer)
		{
			Utility::SetIfNotNull(reason, "Graphics renderer can enable transparency even for child windows");
			return true;
		}
		else
		{
			Utility::SetIfNotNull(reason, "Not a top-level window and doesn't use graphics renderer");
			return false;
		}
	}
	bool Host::SetFont(const GDIFont& font)
	{
		return GetRootElement().SetStyleFont(font);
	}
	bool Host::SetBackgroundColor(const Color& color)
	{
		return GetRootElement().SetStyleAttribute("background-color", color);
	}
	bool Host::SetForegroundColor(const Color& color)
	{
		return GetRootElement().SetStyleAttribute("color", color);
	}

	LayoutDirection Host::GetLayoutDirection() const
	{
		using namespace UI;

		String value = GetRootElement().GetStyleAttribute("direction");
		if (value == "ltr")
		{
			return LayoutDirection::LeftToRight;
		}
		else if (value == "rtl")
		{
			return LayoutDirection::RightToLeft;
		}
		return LayoutDirection::Default;
	}
	void Host::SetLayoutDirection(LayoutDirection value)
	{
		using namespace UI;

		switch (value)
		{
			case LayoutDirection::LeftToRight:
			{
				GetRootElement().SetStyleAttribute("direction", "ltr");
				break;
			}
			case LayoutDirection::RightToLeft:
			{
				GetRootElement().SetStyleAttribute("direction", "rtl");
				break;
			}
			default:
			{
				GetRootElement().RemoveStyleAttribute("direction");
			}
		};
	}

	bool Host::LoadHTML(const String& html, const FSPath& basePath)
	{
		m_DocumentPath = {};
		m_DocumentBasePath = basePath;

		auto utf8 = ToSciterUTF8(html);
		auto basePathString = FSPathToSciterAddress(basePath);
		return GetSciterAPI()->SciterLoadHtml(m_SciterWindow.GetHandle(), utf8.data(), utf8.size(), basePathString.wc_str());
	}
	bool Host::LoadHTML(const String& html, const URI& baseURI)
	{
		m_DocumentPath = {};
		m_DocumentBasePath = URIToSciterAddress(baseURI);

		auto utf8 = ToSciterUTF8(html);
		auto basePathString = m_DocumentBasePath.GetFullPath();
		return GetSciterAPI()->SciterLoadHtml(m_SciterWindow.GetHandle(), utf8.data(), utf8.size(), basePathString.wc_str());
	}
	bool Host::ApplyStylesheet(const StylesheetStorage& stylesheet)
	{
		StylesheetStorage combinedStorage;
		combinedStorage.CopyItems(MasterStylesheetStorage::GetInstance());
		combinedStorage.CopyItems(stylesheet);

		return combinedStorage.Apply(*this, m_DocumentBasePath);
	}

	bool Host::LoadDocument(const FSPath& localPath)
	{
		m_DocumentPath = localPath;
		m_DocumentBasePath = localPath.GetParent();

		auto documentPathString = m_DocumentPath.GetFullPath();
		return GetSciterAPI()->SciterLoadFile(m_SciterWindow.GetHandle(), documentPathString.wc_str());
	}
	bool Host::LoadDocument(const URI& uri)
	{
		m_DocumentPath = uri.BuildURI();
		m_DocumentBasePath = m_DocumentPath;

		auto documentPathString = m_DocumentPath.GetFullPath();
		return GetSciterAPI()->SciterLoadFile(m_SciterWindow.GetHandle(), documentPathString.wc_str());
	}
	void Host::ClearDocument()
	{
		m_DocumentBasePath = {};
		LoadHTML({});
	}

	Element Host::GetRootElement() const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterGetRootElement(m_SciterWindow.GetHandle(), &node) == SCDOM_OK)
		{
			return reinterpret_cast<ElementHandle*>(node);
		}
		return {};
	}
	Element Host::GetFocusedElement() const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterGetFocusElement(m_SciterWindow.GetHandle(), &node) == SCDOM_OK)
		{
			return reinterpret_cast<ElementHandle*>(node);
		}
		return {};
	}
	Element Host::GetElementByUID(ElementUID* id) const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterGetElementByUID(m_SciterWindow.GetHandle(), reinterpret_cast<UINT>(id), &node) == SCDOM_OK)
		{
			return reinterpret_cast<ElementHandle*>(node);
		}
		return {};
	}
	Element Host::GetElementFromPoint(const Point& pos) const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterFindElement(m_SciterWindow.GetHandle(), {pos.GetX(), pos.GetY()}, &node) == SCDOM_OK)
		{
			return reinterpret_cast<ElementHandle*>(node);
		}
		return {};
	}

	Element Host::GetHighlightedElement() const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterGetHighlightedElement(m_SciterWindow.GetHandle(), &node) == SCDOM_OK)
		{
			return reinterpret_cast<ElementHandle*>(node);
		}
		return {};
	}
	void Host::SetHighlightedElement(const Element& node)
	{
		GetSciterAPI()->SciterSetHighlightedElement(m_SciterWindow.GetHandle(), (HELEMENT)node.GetHandle());
	}

	ScriptValue Host::ExecuteScript(const String& script)
	{
		ScriptValue result;
		GetSciterAPI()->SciterEval(m_SciterWindow.GetHandle(), script.wc_str(), script.length(), reinterpret_cast<SCITER_VALUE*>(&result.GetNativeValue()));
		return result;
	}
}
