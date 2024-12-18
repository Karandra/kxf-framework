#pragma once
#include "Common.h"
#include "IWidget.h"

namespace kxf
{
	class URI;
	class Version;
}

namespace kxf
{
	class KXF_API IWebViewWidget: public RTTI::ExtendInterface<IWebViewWidget, IWidget>
	{
		kxf_RTTI_DeclareIID(IWebViewWidget, {0xfe1ceae9, 0xd8e, 0x4ac8, {0x88, 0x7e, 0x5c, 0x8a, 0x8b, 0xb2, 0x62, 0x2}});

		public:
			struct HistoryItem
			{
				URI Address;
				String Title;
			};

		public:
			virtual ~IWebViewWidget() = default;

		public:
			virtual String GetBackendName() const = 0;
			virtual Version GetBackendVersion() const = 0;

			virtual void Stop() = 0;
			virtual bool IsBusy() const = 0;
			virtual void PrintDocument() = 0;

			virtual void LoadDocument(const URI& uri) = 0;
			virtual void LoadDocumentHTML(const String& html, const URI& baseURI) = 0;
			virtual void ReloadDocument() = 0;

			virtual String GetDocumentText() const = 0;
			virtual String GetDocumentHTML() const = 0;

			virtual String GetDocumentTitle() const = 0;
			virtual URI GetDocumentURI() const = 0;

			virtual bool IsEditable() const = 0;
			virtual void SetEditable(bool isEditable = true) = 0;

			// Clipboard
			virtual bool CanCut() const = 0;
			virtual bool CanCopy() const = 0;
			virtual bool CanPaste() const = 0;

			virtual void Cut() = 0;
			virtual void Copy() = 0;
			virtual void Paste() = 0;

			// Undo/Redo
			virtual bool CanUndo() const = 0;
			virtual bool CanRedo() const = 0;

			virtual void Undo() = 0;
			virtual void Redo() = 0;

			// Scripting
			virtual bool RunScript(const String& script) = 0;
			virtual bool RunScript(const String& script, String& result) = 0;

			// Dev Tools
			virtual bool IsDevToolsEnabled() const = 0;
			virtual void SetDevToolsEnabled(bool enabled = true) = 0;

			virtual String GetUserAgent() const = 0;
			virtual void SetUserAgent(const String& userAgent);

			// History
			virtual bool CanGoBack() const = 0;
			virtual bool CanGoForward() const = 0;
			virtual void GoBack() = 0;
			virtual void GoForward() = 0;

			virtual bool IsHistoryEnabled() const = 0;
			virtual void SetHistoryEnabled(bool enabled = true) = 0;

			virtual void ClearHistory() = 0;
			virtual Enumerator<HistoryItem> EnumBackwardHistory() const = 0;
			virtual Enumerator<HistoryItem> EnumForwardHistory() const = 0;

			// Selection
			virtual bool HasSelection() const = 0;
			virtual void SelectNone() = 0;
			virtual void SelectAll() = 0;

			virtual String GetSelectedText() const = 0;
			virtual String GetSelectedHTML() const = 0;
	};
}
