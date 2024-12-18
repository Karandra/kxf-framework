#pragma once
#include "Common.h"
#include "../IWebViewWidget.h"
class wxWebView;

namespace kxf::Widgets
{
	class KXF_API WebView: public RTTI::Implementation<WebView, Private::BasicWxWidget<WebView, wxWebView, IWebViewWidget>>
	{
		private:
			String m_BackendName;
			bool m_IsHistoryEnabled = false;

		public:
			WebView();
			~WebView();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IWebViewWidget
			String GetBackendName() const override;
			Version GetBackendVersion() const override;

			void Stop() override;
			bool IsBusy() const override;
			void PrintDocument() override;

			void LoadDocument(const URI& uri) override;
			void LoadDocumentHTML(const String& html, const URI& baseURI) override;
			void ReloadDocument() override;

			String GetDocumentText() const override;
			String GetDocumentHTML() const override;

			String GetDocumentTitle() const override;
			URI GetDocumentURI() const override;

			bool IsEditable() const override;
			void SetEditable(bool isEditable = true) override;

			// Clipboard
			bool CanCut() const override;
			bool CanCopy() const override;
			bool CanPaste() const override;

			void Cut() override;
			void Copy() override;
			void Paste() override;

			// Undo/Redo
			bool CanUndo() const override;
			bool CanRedo() const override;

			void Undo() override;
			void Redo() override;

			// Scripting
			bool RunScript(const String& script) override;
			bool RunScript(const String& script, String& result) override;

			// Dev Tools
			bool IsDevToolsEnabled() const override;
			void SetDevToolsEnabled(bool enabled = true) override;

			String GetUserAgent() const override;
			void SetUserAgent(const String& userAgent);

			// History
			bool CanGoBack() const override;
			bool CanGoForward() const override;
			void GoBack() override;
			void GoForward() override;

			bool IsHistoryEnabled() const override;
			void SetHistoryEnabled(bool enabled = true) override;

			void ClearHistory() override;
			Enumerator<HistoryItem> EnumBackwardHistory() const override;
			Enumerator<HistoryItem> EnumForwardHistory() const override;

			// Selection
			bool HasSelection() const override;
			void SelectNone() override;
			void SelectAll() override;

			String GetSelectedText() const override;
			String GetSelectedHTML() const override;
	};
}
