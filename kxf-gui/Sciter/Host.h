#pragma once
#include "Common.h"
#include "Node.h"
#include "Element.h"
#include "ScriptValue.h"
#include "Events.h"
#include "EventDispatcher.h"
#include "IWindowRenderer.h"
#include "Renderers/FPSCounter.h"
#include "Stylesheets/StylesheetStorage.h"
#include "kxf/EventSystem/EvtHandler.h"
#include <wx/window.h>

namespace kxf::Sciter
{
	enum class HostStyle: uint32_t
	{
		None = 0,

		DesktopComposition = 1 << 0,
		SmoothScrolling = 1 << 1,
		SystemTheme = 1 << 2,
		AllowDebug = 1 << 3,
	};
}
namespace kxf
{
	kxf_FlagSet_Declare(Sciter::HostStyle);
}

namespace kxf::Sciter
{
	class KXF_API Host: public wxObject
	{
		friend class Node;
		friend class Element;
		friend class BasicEventDispatcher;

		private:
			wxWindow& m_SciterWindow;
			EvtHandler& m_EvtHandler;

			EventDispatcher m_EventDispatcher;
			std::unique_ptr<IWindowRenderer> m_Renderer;
			std::unordered_map<IEvtHandler*, std::unique_ptr<BasicEventDispatcher>> m_ElementEventDispatchers;
			FPSCounter m_FrameCounter;

			bool m_EngineCreated = false;
			bool m_AllowSciterHandleMessage = false;

			FlagSet<HostStyle> m_Style;
			FontSmoothing m_Option_FontSmoothing = FontSmoothing::SystemDefault;
			WindowRenderer m_Option_WindowRenderer = WindowRenderer::Default;

			FSPath m_DocumentPath;
			FSPath m_DocumentBasePath;
			bool m_ReloadScheduled = false;

		private:
			void SetDefaultOptions();
			std::pair<FlagSet<int>, FlagSet<int>> UpdateWindowStyle();

			void OnEngineCreated();
			void OnEngineDestroyed();
			void OnDocumentChanged();
			bool ProcessEvent(IEvent& event, const EventID& eventID);

			void AttachElementHandler(Element& element);
			void DetachElementHandler(Element& element);
			void AttachElementHandler(Element& element, IEvtHandler& evtHandler);
			void DetachElementHandler(Element& element, IEvtHandler& evtHandler);

		protected:
			bool SciterPreHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam);
			bool SciterPostHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam);
			void OnInternalIdle();

		public:
			Host(wxWindow& window, EvtHandler& evtHandler, FlagSet<HostStyle> style = {});
			Host(const Host&) = delete;
			virtual ~Host();

		public:
			bool Create();
			void Update();
			void Reload();
			bool IsEngineCreated()
			{
				return m_EngineCreated;
			}

			const wxWindow& GetWindow() const
			{
				return m_SciterWindow;
			}
			wxWindow& GetWindow()
			{
				return m_SciterWindow;
			}
			EvtHandler& GetEvtHandler()
			{
				return m_EvtHandler;
			}

			Size GetBestSize() const;
			Size GetDPI() const;
			double GetFPS() const;

			FlagSet<HostStyle> GetStyle() const
			{
				return m_Style;
			}
			void SetStyle(FlagSet<HostStyle> style)
			{
				m_Style = style;
			}

			bool IsSystemThemeEnabled() const;
			bool EnableSystemTheme(bool enable = true);

			bool IsSmoothScrollingEnabled() const;
			bool EnableSmoothScrolling(bool enable = true);

			FontSmoothing GetFontSommthingMode() const;
			bool SetFontSmoothingMode(FontSmoothing mode);

			bool SetWindowFrame(WindowFrame windowFrame);
			bool SetBlurBehind(WindowBlurBehind blurMode);

			WindowRenderer GetWindowRenderer() const;
			bool SetWindowRenderer(WindowRenderer renderer);

			bool IsTransparentBackgroundSupported(wxString* reason = nullptr) const;
			bool SetFont(const GDIFont& font);
			bool SetBackgroundColor(const Color& color);
			bool SetForegroundColor(const Color& color);

			LayoutDirection GetLayoutDirection() const;
			void SetLayoutDirection(LayoutDirection value);

		public:
			bool LoadHTML(const String& html, const FSPath& basePath = {});
			bool LoadHTML(const String& html, const URI& baseURI);
			bool ApplyStylesheet(const StylesheetStorage& stylesheet);

			bool LoadDocument(const FSPath& localPath);
			bool LoadDocument(const URI& uri);
			void ClearDocument();

			Element GetRootElement() const;
			Element GetFocusedElement() const;
			Element GetElementByUID(ElementUID* id) const;
			Element GetElementFromPoint(const Point& pos) const;

			Element GetHighlightedElement() const;
			void SetHighlightedElement(const Element& node);

			ScriptValue ExecuteScript(const String& script);

		public:
			Host& operator=(const Host&) = delete;
	};
}
