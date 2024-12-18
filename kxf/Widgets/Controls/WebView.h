#pragma once
#include "kxf/Widgets/Common.h"
#include <wx/webview.h>

namespace kxf::UI
{
	class KXF_API WebView: public wxWebView
	{
		public:
			static constexpr char DefaultPage[] = "about:blank";
			static constexpr int DefaultStyle = 0;

		public:
			static wxWebView* New(wxWindow* parent,
								  wxWindowID id,
								  const String& defaultURL = DefaultPage,
								  const String& backend = wxWebViewBackendDefault,
								  long style = DefaultStyle
			);

		public:
			wxDECLARE_ABSTRACT_CLASS(WebView);
	};
}
