#include "kxf-pch.h"
#include "FileBrowseDialog.h"
#include "Private/FileBrowseDialogEvents.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/ScopeGuard.h"

namespace
{
	using namespace kxf;
	using namespace kxf::UI;

	constexpr SIGDN g_ItemType = SIGDN_FILESYSPATH;
	constexpr FILEOPENDIALOGOPTIONS g_RequiredOptions = FOS_NOCHANGEDIR|FOS_NOVALIDATE|FOS_NOTESTFILECREATE;

	constexpr FlagSet<_FILEOPENDIALOGOPTIONS> MapDialogStyle(FlagSet<FileBrowseDialogStyle> style) noexcept
	{
		FlagSet<_FILEOPENDIALOGOPTIONS> nativeStyle;
		nativeStyle.Add(FOS_CREATEPROMPT, style & FileBrowseDialogStyle::WarnCreate);
		nativeStyle.Add(FOS_OVERWRITEPROMPT, style & FileBrowseDialogStyle::WarnOverwrite);
		nativeStyle.Add(FOS_FORCEFILESYSTEM, style & FileBrowseDialogStyle::ForceFileSystem);
		nativeStyle.Add(FOS_FORCEPREVIEWPANEON, style & FileBrowseDialogStyle::ForcePreviewPane);
		nativeStyle.Add(FOS_ALLNONSTORAGEITEMS, !(style & FileBrowseDialogStyle::ForceFileSystem));
		nativeStyle.Add(FOS_FORCESHOWHIDDEN, style & FileBrowseDialogStyle::ForceShowHidden);
		nativeStyle.Add(FOS_DONTADDTORECENT, !(style & FileBrowseDialogStyle::AddToRecent));
		nativeStyle.Add(FOS_ALLOWMULTISELECT, style & FileBrowseDialogStyle::Multiselect);
		nativeStyle.Add(static_cast<_FILEOPENDIALOGOPTIONS>(FOS_PATHMUSTEXIST|FOS_FILEMUSTEXIST), style & FileBrowseDialogStyle::ForceExistingItem);
		nativeStyle.Add(FOS_NOREADONLYRETURN, style & FileBrowseDialogStyle::SkipReadOnlyItems);
		nativeStyle.Add(FOS_HIDEPINNEDPLACES, style & FileBrowseDialogStyle::HidePinnedPlaces);
		nativeStyle.Add(FOS_NODEREFERENCELINKS, style & FileBrowseDialogStyle::NoDereferenceLinks);
		nativeStyle.Add(FOS_SUPPORTSTREAMABLEITEMS, style & FileBrowseDialogStyle::AllowStreamableItems);

		return nativeStyle;
	}
	String GetDisplayName(IShellItem& shellItem, SIGDN type)
	{
		COMMemoryPtr<wchar_t> result;
		if (HResult(shellItem.GetDisplayName(type, &result)))
		{
			return String(result);
		}
		return {};
	}
	HResult InitIFileDialog2(IFileDialog& fileDialog, COMPtr<IFileDialog2>& fileDialog2)
	{
		return fileDialog.QueryInterface(IID_PPV_ARGS(&fileDialog2));
	}
}

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(FileBrowseDialog, StdDialog);

	void FileBrowseDialog::ApplyFileFilters()
	{
		if (m_Instance)
		{
			m_FilterListSpec.resize(m_FilterList.size());
			for (size_t i = 0; i < m_FilterList.size(); i++)
			{
				Private::FilterSpecItem& spec = m_FilterListSpec[i];
				spec.Name = m_FilterList[i].Name.wc_str();
				spec.Filter = m_FilterList[i].Filter.wc_str();
			}
			m_Instance->SetFileTypes(m_FilterListSpec.size(), reinterpret_cast<const COMDLG_FILTERSPEC*>(m_FilterListSpec.data()));
		}
	}

	FileBrowseDialog::FileBrowseDialog() = default;
	bool FileBrowseDialog::Create(wxWindow* parent,
								  wxWindowID id,
								  FileBrowseDialogMode mode,
								  const String& caption,
								  FlagSet<StdButton> buttons,
								  FlagSet<FileBrowseDialogStyle> style
	)
	{
		if (Dialog::Create(parent, id, caption, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), DialogStyle::None|*style))
		{
			m_Parent = parent;
			m_Style = style;
			m_Mode = mode;

			// Create instance
			HResult hr = HResult::Fail();
			if (m_Mode == FileBrowseDialogMode::Save)
			{
				hr = COM::CreateInstance(CLSID_FileSaveDialog, COMClassContext::InprocServer, &m_Instance);
			}
			else
			{
				hr = COM::CreateInstance(CLSID_FileOpenDialog, COMClassContext::InprocServer, &m_Instance);
			}

			if (m_Instance && hr)
			{
				// Options
				FlagSet<FILEOPENDIALOGOPTIONS> options = g_RequiredOptions|*MapDialogStyle(style);
				options.Add(_FILEOPENDIALOGOPTIONS::FOS_PICKFOLDERS, m_Mode == FileBrowseDialogMode::OpenDirectory);

				if (hr = m_Instance->SetOptions(*options))
				{
					if (!caption.IsEmpty())
					{
						SetCaption(caption);
					}

					InitIFileDialog2(*m_Instance, m_InstanceExtra);
					m_Events = new Private::FileBrowseDialogEvents(*this);
				}
				return true;
			}
		}
		return false;
	}
	FileBrowseDialog::~FileBrowseDialog()
	{
	}

	int FileBrowseDialog::ShowModal()
	{
		wxWindowID retCode = wxID_CANCEL;
		if (m_Instance)
		{
			// Add empty filter if there is no user filters
			if (m_FilterList.empty())
			{
				AddUnivsersalFilter();
			}

			// Apply pending changes and show the dialog
			ApplyFileFilters();
			if (m_ShowStatus = m_Instance->Show(m_Parent ? m_Parent->GetHandle() : nullptr))
			{
				retCode = wxID_OK;
			}
		}

		Dialog::SetReturnCode(retCode);
		return retCode;
	}
	bool FileBrowseDialog::Show(bool show)
	{
		return ShowNativeWindow(this, show);
	}
	bool FileBrowseDialog::Close(bool force)
	{
		if (m_Instance)
		{
			m_Instance->Close(*m_ShowStatus);
		}
		return Dialog::Close(force);
	}
	bool FileBrowseDialog::IsShown() const
	{
		return GetHandle() && ::IsWindowVisible(GetHandle());
	}

	void FileBrowseDialog::SetCaption(const wxString& caption)
	{
		m_Caption = caption;
		if (m_Instance)
		{
			m_Instance->SetTitle(m_Caption.IsEmpty() ? nullptr : m_Caption.wc_str());
		}
	}
	void FileBrowseDialog::SetLabel(const wxString& label)
	{
		m_Label = label;
		if (m_Instance)
		{
			m_Instance->SetFileNameLabel(m_Label.wc_str());
		}
	}

	bool FileBrowseDialog::SetDirectory(const FSPath& path)
	{
		if (m_Instance)
		{
			String pathName = path.GetFullPath();

			COMPtr<IShellItem> shellItem;
			::SHCreateItemFromParsingName(pathName.wc_str(), nullptr, IID_PPV_ARGS(&shellItem));
			if (shellItem)
			{
				return HResult(m_Instance->SetFolder(shellItem)).IsSuccess();
			}
		}
		return false;
	}
	bool FileBrowseDialog::SetNavigationRoot(const FSPath& path)
	{
		if (m_InstanceExtra)
		{
			String pathName = path.GetFullPath();

			COMPtr<IShellItem> shellItem;
			SHCreateItemFromParsingName(pathName.wc_str(), nullptr, IID_PPV_ARGS(&shellItem));
			if (shellItem)
			{
				return HResult(m_InstanceExtra->SetNavigationRoot(shellItem)).IsSuccess();
			}
		}
		return false;
	}
	bool FileBrowseDialog::SetButtonLabel(StdID buttonID, const String& text)
	{
		if (m_Instance)
		{
			HResult hr = HResult::Fail();
			switch (buttonID)
			{
				case StdID::OK:
				{
					hr = m_Instance->SetOkButtonLabel(text.wc_str());
					break;
				}
				case StdID::Cancel:
				{
					if (m_InstanceExtra)
					{
						hr = m_InstanceExtra->SetCancelButtonLabel(text.wc_str());
					}
					break;
				}
			};
			return hr.IsSuccess();
		}
		return false;
	}

	String FileBrowseDialog::GetItemName() const
	{
		if (m_Instance)
		{
			COMMemoryPtr<wchar_t> result;
			if (HResult(m_Instance->GetFileName(&result)))
			{
				return String(result);
			}
		}
		return {};
	}
	void FileBrowseDialog::SetItemName(const String& name)
	{
		if (m_Instance)
		{
			m_Instance->SetFileName(name.wc_str());
		}
	}
	void FileBrowseDialog::SetDefaultExtension(const String& extension)
	{
		if (m_Instance)
		{
			if (extension.length() > 0 && extension[0] == '.')
			{
				m_Instance->SetDefaultExtension(extension.SubMid(1).wc_str());
			}
			else
			{
				m_Instance->SetDefaultExtension(extension.wc_str());
			}
		}
	}

	size_t FileBrowseDialog::GetSelectedFilter() const
	{
		if (m_Instance)
		{
			UINT index = 0;
			if (HResult(m_Instance->GetFileTypeIndex(&index)) && index != 0 && index - 1 < m_FilterList.size())
			{
				// This is one-based index: https://msdn.microsoft.com/ru-ru/library/windows/desktop/bb775958(v=vs.85).aspx
				return index - 1;
			}
		}
		return std::numeric_limits<size_t>::max();
	}
	void FileBrowseDialog::SetSelectedFilter(size_t filterIndex)
	{
		if (m_Instance && filterIndex < m_FilterList.size())
		{
			// One-based index
			m_Instance->SetFileTypeIndex(filterIndex + 1);
		}
	}

	bool FileBrowseDialog::AddPinnedPlace(const FSPath& path, const String& label, bool top)
	{
		if (m_Instance)
		{
			String pathName = path.GetFullPath();

			COMPtr<IShellItem> pathItem;
			::SHCreateItemFromParsingName(pathName.wc_str(), nullptr, IID_PPV_ARGS(&pathItem));
			if (pathItem)
			{
				if (!label.IsEmpty())
				{
					auto labelCOM = COM::AllocateRawString(label);

					PROPVARIANT property = {};
					property.vt = VT_LPWSTR;
					property.pwszVal = labelCOM;
					::SHSetTemporaryPropertyForItem(pathItem, ::PKEY_ItemNameDisplay, property);
				}

				return HResult(m_Instance->AddPlace(pathItem, top ? FDAP_TOP : FDAP_BOTTOM)).IsSuccess();
			}
		}
		return false;
	}
	size_t FileBrowseDialog::AddFilter(const String& filter, const String& label)
	{
		if (!filter.IsEmpty())
		{
			if (label.IsEmpty())
			{
				// Create label from filter
				String temp = String(filter);
				temp.Replace(";", ", ", true);
				temp = Format("{} ({})", label, temp);

				auto& item = m_FilterList.emplace_back();
				item.Name = std::move(temp);
				item.Filter = filter;
			}
			else
			{
				auto& item = m_FilterList.emplace_back();
				item.Name = label;
				item.Filter = filter;
			}
			return m_FilterList.size() - 1;
		}
		return std::numeric_limits<size_t>::max();
	}
	size_t FileBrowseDialog::AddUnivsersalFilter()
	{
		return AddFilter("*", "*");
	}

	FSPath FileBrowseDialog::GetResult() const
	{
		if (m_Instance)
		{
			HResult hr = HResult::Fail();
			if (m_Style & FileBrowseDialogStyle::Multiselect && m_Mode != FileBrowseDialogMode::Save)
			{
				IFileOpenDialog* dialog = static_cast<IFileOpenDialog*>(m_Instance.Get());
				COMPtr<IShellItemArray> shellList;
				if (hr = dialog->GetResults(&shellList))
				{
					DWORD count = 0;
					if (hr = shellList->GetCount(&count); hr && count != 0)
					{
						COMPtr<IShellItem> shellItem;
						if (hr = shellList->GetItemAt(0, &shellItem))
						{
							return GetDisplayName(*shellItem, g_ItemType);
						}
					}
				}
			}
			else
			{
				COMPtr<IShellItem> shellItem;
				if (hr = m_Instance->GetResult(&shellItem))
				{
					return GetDisplayName(*shellItem, g_ItemType);
				}
			}
		}
		return {};
	}
	size_t FileBrowseDialog::EnumResults(std::function<bool(FSPath)> func) const
	{
		if (m_Instance)
		{
			HResult hr = HResult::Fail();
			if (GetMode() == FileBrowseDialogMode::Save)
			{
				// In save mode we have only one result
				std::invoke(func, GetResult());
				return 1;
			}
			else
			{
				IFileOpenDialog* dialog = static_cast<IFileOpenDialog*>(m_Instance.Get());
				COMPtr<IShellItemArray> shellList;
				if (hr = dialog->GetResults(&shellList))
				{
					DWORD itemCount = 0;
					if (hr = shellList->GetCount(&itemCount))
					{
						size_t count = 0;
						for (DWORD i = 0; i < itemCount; i++)
						{
							COMPtr<IShellItem> shellItem;
							if (hr = shellList->GetItemAt(i, &shellItem))
							{
								count++;
								if (!std::invoke(func, GetDisplayName(*shellItem, g_ItemType)))
								{
									break;
								}
							}
						}
						return count;
					}
				}
			}
		}
		return 0;
	}
}
