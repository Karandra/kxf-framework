#include "kxf-pch.h"
#include "ArchiveUpdateCallback.h"
#include "InStreamWrapper.h"
#include "Utility.h"
#include "kxf/FileSystem/Private/NativeFSUtility.h"
#include "kxf/System/VariantProperty.h"
#include "kxf/Utility/ScopeGuard.h"

namespace kxf::SevenZip::Private::Callback
{
	FileItem UpdateArchive::GetExistingItem(size_t fileIndex) const
	{
		return GetArchiveItem(*m_Archive, fileIndex);
	}
	STDMETHODIMP UpdateArchive::QueryInterface(const ::IID& iid, void** ppvObject)
	{
		if (iid == __uuidof(IUnknown))
		{
			*ppvObject = reinterpret_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_IArchiveUpdateCallback)
		{
			*ppvObject = static_cast<IArchiveUpdateCallback*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_ICryptoGetTextPassword)
		{
			*ppvObject = static_cast<ICryptoGetTextPassword*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_ICryptoGetTextPassword2)
		{
			*ppvObject = static_cast<ICryptoGetTextPassword2*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_ICompressProgressInfo)
		{
			*ppvObject = static_cast<ICompressProgressInfo*>(this);
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
}

namespace kxf::SevenZip::Private::Callback
{
	STDMETHODIMP UpdateArchiveWrapper::GetUpdateItemInfo(UInt32 index, Int32* newData, Int32* newProperties, UInt32* indexInArchive)
	{
		if (ShouldCancel())
		{
			return *HResult::Abort();
		}

		if (indexInArchive && newData && newProperties)
		{
			bool updateData = false;
			bool updateProperties = false;
			size_t newIndex = m_Callback.OnGetUpdateMode(index, updateData, updateProperties);

			constexpr auto maxNativeIndex = std::numeric_limits<UInt32>::max();
			if (newIndex != Compression::InvalidIndex && newIndex < static_cast<size_t>(maxNativeIndex))
			{
				*indexInArchive = newIndex;
				*newProperties = updateProperties;
				*newData = updateData;
			}
			else
			{
				// Assume new file
				*newData = 1;
				*newProperties = 1;
				*indexInArchive = maxNativeIndex;
			}
			return *HResult::Success();
		}
		return *HResult::InvalidPointer();
	}
	STDMETHODIMP UpdateArchiveWrapper::GetProperty(UInt32 index, PROPID propID, PROPVARIANT* value)
	{
		if (ShouldCancel())
		{
			return *HResult::Abort();
		}

		if (value)
		{
			auto DoGetProperty = [](PROPID propID, const FileItem& fileItem) -> VariantProperty
			{
				switch (propID)
				{
					case kpidIsAnti:
					{
						// WTF is this property?
						return false;
					}
					case kpidPath:
					{
						return fileItem.GetFullPath().GetFullPath();
					}
					case kpidIsDir:
					{
						return fileItem.IsDirectory();
					}
					case kpidSize:
					{
						// Apparently 7-Zip requires file size to be of 'uint64_t' type
						return fileItem.GetSize().ToBytes<uint64_t>();
					}
					case kpidAttrib:
					{
						// Attributes must be of 'uint32_t' type
						return static_cast<uint32_t>(*FileSystem::Private::MapFileAttributes(fileItem.GetAttributes()));
					}
					case kpidCTime:
					{
						return fileItem.GetCreationTime();
					}
					case kpidMTime:
					{
						return fileItem.GetModificationTime();
					}
					case kpidATime:
					{
						return fileItem.GetLastAccessTime();
					}
				};
				return {};
			};

			auto it = m_ItemStore.find(index);
			if (it == m_ItemStore.end())
			{
				it = m_ItemStore.insert_or_assign(index, m_Callback.OnGetProperties(index)).first;
				it->second.SetUniqueID(LocallyUniqueID(index));
			}
			if (it != m_ItemStore.end())
			{
				VariantProperty property = DoGetProperty(propID, it->second);
				if (property)
				{
					*property.CopyToNative(*value);
				}
				return *HResult::Success();
			}
			return *HResult::InvalidArgument();
		}
		return *HResult::InvalidPointer();
	}
	STDMETHODIMP UpdateArchiveWrapper::GetStream(UInt32 index, ISequentialInStream** inStream)
	{
		if (ShouldCancel())
		{
			return *HResult::Abort();
		}
		if (!inStream)
		{
			return *HResult::InvalidPointer();
		}

		*inStream = nullptr;
		m_Stream = nullptr;
		if (auto it = m_ItemStore.find(index); it != m_ItemStore.end())
		{
			m_CurrentItem = std::move(it->second);
			m_ItemStore.erase(it);

			if (m_EvtHandler && !SendItemEvent(IArchiveUpdate::EvtItem, m_CurrentItem))
			{
				return *HResult::Abort();
			}

			m_Stream = m_Callback.OnGetStream(m_CurrentItem);
			if (m_Stream)
			{
				if (ShouldCancel())
				{
					return *HResult::Abort();
				}

				auto wrapperStream = COM::CreateLocalInstance<InStreamWrapper_IInputStream>(*m_Stream, m_EvtHandler.Get());
				wrapperStream->SpecifyTotalSize(std::max(m_Stream->GetSize(), m_CurrentItem.GetSize()).ToBytes());
				*inStream = wrapperStream.Detach();

				return *HResult::Success();
			}
			return *HResult::False();
		}
		return *HResult::Fail();
	}
	STDMETHODIMP UpdateArchiveWrapper::SetOperationResult(Int32 operationResult)
	{
		InputStreamDelegate stream = std::move(m_Stream);

		if (ShouldCancel())
		{
			return *HResult::Abort();
		}
		if (operationResult == NArchive::NExtract::NOperationResult::kOK)
		{
			if (stream && !m_Callback.OnItemDone(m_CurrentItem, *stream))
			{
				return *HResult::Fail();
			}
			if (m_EvtHandler && !SendItemEvent(IArchiveUpdate::EvtItemDone, std::move(m_CurrentItem)))
			{
				return *HResult::Abort();
			}
		}
		return *HResult::Success();
	}
}

namespace kxf::SevenZip::Private::Callback
{
	size_t UpdateArchiveFromFS::OnGetUpdateMode(size_t index, bool& updateData, bool& updateProperties)
	{
		// We don't support updating existing files yet
		updateData = true;
		updateProperties = true;
		return Compression::InvalidIndex;
	}
	FileItem UpdateArchiveFromFS::OnGetProperties(size_t index)
	{
		if (index < m_Files.size())
		{
			FileItem item = m_Files[index];
			item.SetFullPath(item.GetFullPath().GetAfter(m_Directory));

			return item;
		}
		return {};
	}
	InputStreamDelegate UpdateArchiveFromFS::OnGetStream(const FileItem& item)
	{
		size_t index = item.GetUniqueID().ToLocallyUniqueID().ToInt();
		if (index < m_Files.size())
		{
			return m_FileSystem.OpenToRead(m_Files[index].GetFullPath());
		}
		return nullptr;
	}
	bool UpdateArchiveFromFS::OnItemDone(const FileItem& item, IInputStream& stream)
	{
		size_t index = item.GetUniqueID().ToLocallyUniqueID().ToInt();
		if (index < m_Files.size())
		{
			m_Files[index] = {};
		}
		return true;
	}
}
