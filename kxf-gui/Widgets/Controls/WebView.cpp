#include "kxf-pch.h"
#include "WebView.h"

namespace kxf::UI
{
	wxIMPLEMENT_ABSTRACT_CLASS(WebView, wxWebView);

	wxWebView* WebView::New(wxWindow* parent,
							  wxWindowID id,
							  const String& defaultURL,
							  const String& backend,
							  long style
	)
	{
		return wxWebView::New(parent, id, defaultURL, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), backend, style);
	}
}
