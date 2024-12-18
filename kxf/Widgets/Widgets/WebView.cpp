#include "kxf-pch.h"
#include "WebView.h"
#include "kxf/Core/Version.h"
#include "kxf/Network/URI.h"
#include "kxf/Utility/Enumerator.h"
#include <wx/webview.h>

namespace
{
	kxf::Enumerator<kxf::IWebViewWidget::HistoryItem> EnumHistory(wxVector<wxSharedPtr<wxWebViewHistoryItem>> items)
	{
		using namespace kxf;

		return Utility::EnumerateIterableContainer<IWebViewWidget::HistoryItem>(std::move(items), [](const wxSharedPtr<wxWebViewHistoryItem>& item) -> IWebViewWidget::HistoryItem
		{
			return {item->GetUrl(), item->GetTitle()};
		});
	}
}

namespace kxf::Widgets
{
	// ComboBox
	WebView::WebView()
	{
		auto MakeWidget = [&](wxString backend) -> std::unique_ptr<wxWebView>
		{
			if (wxWebView::IsBackendAvailable(backend))
			{
				if (auto ptr = std::unique_ptr<wxWebView>(wxWebView::New(backend)))
				{
					m_BackendName = std::move(backend);
					return ptr;
				}
			}
			return nullptr;
		};

		auto widget = MakeWidget(wxWebViewBackendEdge);
		if (!widget)
		{
			widget = MakeWidget(wxWebViewBackendWebKit);
		}
		if (!widget)
		{
			widget = MakeWidget(wxWebViewBackendIE);
		}
		if (!widget)
		{
			widget = MakeWidget(wxWebViewBackendDefault);
		}

		AssignWxWidget(std::move(widget));
	}
	WebView::~WebView() = default;

	// IWidget
	bool WebView::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		auto widget = Get();
		if (parent && widget && widget->Create(parent->GetWxWindow(), wxID_NONE, wxWebViewDefaultURLStr, pos, size, wxBORDER_THEME))
		{
			m_IsHistoryEnabled = true;
			widget->SetLabel(label);

			return true;
		}
		return false;
	}

	// IWebViewWidget
	String WebView::GetBackendName() const
	{
		return m_BackendName;
	}
	Version WebView::GetBackendVersion() const
	{
		if (!m_BackendName.IsEmpty())
		{
			return wxWebView::GetBackendVersionInfo(m_BackendName);
		}
		return {};
	}

	void WebView::Stop()
	{
		Get()->Stop();
	}
	bool WebView::IsBusy() const
	{
		return Get()->IsBusy();
	}
	void WebView::PrintDocument()
	{
		Get()->Print();
	}

	void WebView::LoadDocument(const URI& uri)
	{
		Get()->LoadURL(uri.BuildURI());
	}
	void WebView::LoadDocumentHTML(const String& html, const URI& baseURI)
	{
		Get()->SetPage(html, baseURI.BuildURI());
	}
	void WebView::ReloadDocument()
	{
		Get()->Reload();
	}

	String WebView::GetDocumentText() const
	{
		return Get()->GetPageText();
	}
	String WebView::GetDocumentHTML() const
	{
		return Get()->GetPageSource();
	}

	String WebView::GetDocumentTitle() const
	{
		return Get()->GetCurrentTitle();
	}
	URI WebView::GetDocumentURI() const
	{
		return Get()->GetCurrentURL();
	}

	bool WebView::IsEditable() const
	{
		return Get()->IsEditable();
	}
	void WebView::SetEditable(bool isEditable)
	{
		Get()->SetEditable(isEditable);
	}

	// Clipboard
	bool WebView::CanCut() const
	{
		return Get()->CanCut();
	}
	bool WebView::CanCopy() const
	{
		return Get()->CanCopy();
	}
	bool WebView::CanPaste() const
	{
		return Get()->CanPaste();
	}

	void WebView::Cut()
	{
		Get()->Cut();
	}
	void WebView::Copy()
	{
		Get()->Copy();
	}
	void WebView::Paste()
	{
		Get()->Paste();
	}

	// Undo/Redo
	bool WebView::CanUndo() const
	{
		return Get()->CanUndo();
	}
	bool WebView::CanRedo() const
	{
		return Get()->CanRedo();
	}

	void WebView::Undo()
	{
		Get()->Undo();
	}
	void WebView::Redo()
	{
		Get()->Redo();
	}

	// Scripting
	bool WebView::RunScript(const String& script)
	{
		return Get()->RunScript(script);
	}
	bool WebView::RunScript(const String& script, String& result)
	{
		wxString temp;
		if (Get()->RunScript(script, &temp))
		{
			result = std::move(temp);
			return true;
		}
		return false;
	}

	// Dev Tools
	bool WebView::IsDevToolsEnabled() const
	{
		return Get()->IsAccessToDevToolsEnabled();
	}
	void WebView::SetDevToolsEnabled(bool enabled)
	{
		Get()->EnableAccessToDevTools(enabled);
	}

	String WebView::GetUserAgent() const
	{
		return Get()->GetUserAgent();
	}
	void WebView::SetUserAgent(const String& userAgent)
	{
		Get()->SetUserAgent(userAgent);
	}

	// History
	bool WebView::CanGoBack() const
	{
		return Get()->CanGoBack();
	}
	bool WebView::CanGoForward() const
	{
		return Get()->CanGoForward();
	}
	void WebView::GoBack()
	{
		Get()->GoBack();
	}
	void WebView::GoForward()
	{
		Get()->GoForward();
	}

	bool WebView::IsHistoryEnabled() const
	{
		return m_IsHistoryEnabled;
	}
	void WebView::SetHistoryEnabled(bool enabled)
	{
		m_IsHistoryEnabled = enabled;
		Get()->EnableHistory(enabled);
	}

	void WebView::ClearHistory()
	{
		Get()->ClearHistory();
	}
	Enumerator<IWebViewWidget::HistoryItem> WebView::EnumBackwardHistory() const
	{
		return EnumHistory(const_cast<WebView*>(this)->Get()->GetBackwardHistory());
	}
	Enumerator<IWebViewWidget::HistoryItem> WebView::EnumForwardHistory() const
	{
		return EnumHistory(const_cast<WebView*>(this)->Get()->GetForwardHistory());
	}

	// Selection
	bool WebView::HasSelection() const
	{
		return Get()->HasSelection();
	}
	void WebView::SelectNone()
	{
		Get()->ClearSelection();
	}
	void WebView::SelectAll()
	{
		Get()->SelectAll();
	}

	String WebView::GetSelectedText() const
	{
		return Get()->GetSelectedText();
	}
	String WebView::GetSelectedHTML() const
	{
		return Get()->GetSelectedSource();
	}
}
