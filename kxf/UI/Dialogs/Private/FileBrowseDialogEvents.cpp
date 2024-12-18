#include "kxf-pch.h"
#include "FileBrowseDialogEvents.h"
#include "../FileBrowseDialog.h"
#include "kxf/Utility/ScopeGuard.h"

namespace
{
	kxf::HResult InitWindowHandle(IFileDialog& fileDialog, HWND& handle) noexcept
	{
		using namespace kxf;

		HResult hr = HResult::Fail();

		COMPtr<IOleWindow> window;
		if (hr = fileDialog.QueryInterface(IID_PPV_ARGS(&window)))
		{
			hr = window->GetWindow(&handle);
		}
		return hr;
	}
	kxf::String GetDisplayName(IShellItem& shellItem, SIGDN type)
	{
		using namespace kxf;

		COMMemoryPtr<wchar_t> result;
		if (HResult(shellItem.GetDisplayName(type, &result)))
		{
			return String(result);
		}
		return {};
	}
}

namespace kxf::UI::Private
{
	wxNotifyEvent FileBrowseDialogEvents::CreateEvent(const EventID& eventID)
	{
		wxNotifyEvent event;
		event.SetEventType(eventID.AsInt());
		event.SetEventObject(m_Dialog);
		event.SetId(wxID_NONE);
		event.Allow();

		return event;
	}
	bool FileBrowseDialogEvents::ProcessEvent(wxEvent& event) const
	{
		if (m_Dialog)
		{
			return m_Dialog->HandleWindowEvent(event) && !event.GetSkipped();
		}
		return false;
	}

	FileBrowseDialogEvents::FileBrowseDialogEvents(FileBrowseDialog& fileBrowseDialog)
	{
		m_Dialog = &fileBrowseDialog;
		if (m_Dialog->m_Instance)
		{
			m_Dialog->m_Instance->Advise(this, &m_EventsCookie);
		}
	}
	FileBrowseDialogEvents::~FileBrowseDialogEvents()
	{
		if (m_Dialog && m_Dialog->m_Instance)
		{
			m_Dialog->m_Instance->Unadvise(m_EventsCookie);
		}
		m_Dialog = nullptr;
		m_EventsCookie = 0;
	}

	HRESULT FileBrowseDialogEvents::QueryInterface(const ::IID& riid, void** ppvObject)
	{
		*ppvObject = nullptr;
		if (riid == __uuidof(IUnknown))
		{
			*ppvObject = static_cast<IUnknown*>(this);
			return S_OK;
		}
		if (riid == __uuidof(IFileDialogEvents))
		{
			*ppvObject = static_cast<IFileDialogEvents*>(this);
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	ULONG FileBrowseDialogEvents::AddRef()
	{
		const ULONG oldValue = ++m_RefCount;
		if (oldValue > 1)
		{
			// On Create
			m_Dialog->m_Handle = nullptr;
		}
		return oldValue;
	}
	ULONG FileBrowseDialogEvents::Release()
	{
		const ULONG oldValue = --m_RefCount;
		if (oldValue <= 1)
		{
			// On Destroy
			m_Dialog->m_Handle = nullptr;

			wxNotifyEvent event = CreateEvent(wxEVT_CLOSE_WINDOW);
			ProcessEvent(event);
		}
		if (oldValue == 0)
		{
			delete this;
		}
		return oldValue;
	}

	HRESULT FileBrowseDialogEvents::OnShow(IFileDialog* instance)
	{
		HWND handle = nullptr;
		if (InitWindowHandle(*m_Dialog->m_Instance, handle))
		{
			m_Dialog->m_Handle = handle;
		}

		wxNotifyEvent event = CreateEvent(wxEVT_SHOW);
		ProcessEvent(event);
		return S_OK;
	}
	HRESULT FileBrowseDialogEvents::OnFileOk(IFileDialog* instance)
	{
		wxNotifyEvent event = CreateEvent(IStdDialog::EvtButton);
		event.SetId(wxID_OK);
		ProcessEvent(event);
		return event.IsAllowed() ? S_OK : S_FALSE;
	}
	HRESULT FileBrowseDialogEvents::OnFolderChange(IFileDialog* instance)
	{
		if (!m_Dialog->GetHandle())
		{
			OnShow(instance);
		}

		wxNotifyEvent event = CreateEvent(IStdDialog::EvtNavigate);
		ProcessEvent(event);
		return S_OK;
	}
	HRESULT FileBrowseDialogEvents::OnFolderChanging(IFileDialog* instance, IShellItem* shellItem)
	{
		wxNotifyEvent event = CreateEvent(IStdDialog::EvtNavigating);
		event.SetString(GetDisplayName(*shellItem, SIGDN_NORMALDISPLAY));
		ProcessEvent(event);

		return event.IsAllowed() ? S_OK : S_FALSE;
	}
	HRESULT FileBrowseDialogEvents::OnSelectionChange(IFileDialog* instance)
	{
		wxNotifyEvent event = CreateEvent(IStdDialog::EvtSelect);
		ProcessEvent(event);
		return S_OK;
	}
	HRESULT FileBrowseDialogEvents::OnOverwrite(IFileDialog* instance, IShellItem* shellItem, FDE_OVERWRITE_RESPONSE* pResponse)
	{
		wxNotifyEvent event = CreateEvent(IStdDialog::EvtOverwrite);
		event.SetString(GetDisplayName(*shellItem, SIGDN_NORMALDISPLAY));
		event.SetInt(*pResponse);
		ProcessEvent(event);

		return S_OK;
	}
	HRESULT FileBrowseDialogEvents::OnTypeChange(IFileDialog* instance)
	{
		wxNotifyEvent event = CreateEvent(IStdDialog::EvtTypeChanged);
		event.SetInt(m_Dialog->GetSelectedFilter());
		ProcessEvent(event);
		return S_OK;
	}
	HRESULT FileBrowseDialogEvents::OnShareViolation(IFileDialog* instance, IShellItem* shellItem, FDE_SHAREVIOLATION_RESPONSE* pResponse)
	{
		return E_NOTIMPL;
	}
}
