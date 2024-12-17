#include "KxfPCH.h"
#include "ArchiveExtractCallback.h"
#include "OutStreamWrapper.h"
#include "Utility.h"
#include "kxf/IO/INativeStream.h"
#include "kxf/System/VariantProperty.h"
#include "kxf/FileSystem/NativeFileSystem.h"
#include "kxf/Utility/ScopeGuard.h"

namespace kxf::SevenZip::Private::Callback
{
	FileItem ExtractArchive::GetExistingItem(size_t fileIndex) const
	{
		return GetArchiveItem(*m_Archive, fileIndex);
	}
	STDMETHODIMP ExtractArchive::QueryInterface(const ::IID& iid, void** ppvObject)
	{
		if (iid == __uuidof(IUnknown))
		{
			*ppvObject = reinterpret_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_IArchiveExtractCallback)
		{
			*ppvObject = static_cast<IArchiveExtractCallback*>(this);
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
		return E_NOINTERFACE;
	}
}

namespace kxf::SevenZip::Private::Callback
{
	STDMETHODIMP ExtractArchiveWrapper::GetStream(UInt32 fileIndex, ISequentialOutStream** outStream, Int32 askExtractMode)
	{
		if (ShouldCancel())
		{
			return *HResult::Abort();
		}
		if (!outStream)
		{
			return *HResult::InvalidPointer();
		}
		if (askExtractMode != NArchive::NExtract::NAskMode::kExtract)
		{
			return *HResult::False();
		}

		*outStream = nullptr;
		m_Stream = nullptr;
		m_Item = GetExistingItem(fileIndex);
		if (m_Item)
		{
			if (m_EvtHandler && !SendItemEvent(IArchiveExtract::EvtItem, m_Item))
			{
				return *HResult::Abort();
			}

			if (m_Stream = m_Callback.OnGetStream(m_Item))
			{
				if (ShouldCancel())
				{
					return *HResult::Abort();
				}

				auto wrapperStream = COM::CreateLocalInstance<OutStreamWrapper_IOutputStream>(*m_Stream, m_EvtHandler.Get());
				wrapperStream->SetSize(m_Item.GetSize().ToBytes());
				*outStream = wrapperStream.Detach();

				return *HResult::Success();
			}
			return *HResult::False();
		}
		return *HResult::Fail();
	}
	STDMETHODIMP ExtractArchiveWrapper::PrepareOperation(Int32 askExtractMode)
	{
		return ShouldCancel() ? *HResult::Abort() : *HResult::Success();
	}
	STDMETHODIMP ExtractArchiveWrapper::SetOperationResult(Int32 operationResult)
	{
		OutputStreamDelegate stream = std::move(m_Stream);

		if (ShouldCancel())
		{
			return *HResult::Abort();
		}
		if (operationResult == NArchive::NExtract::NOperationResult::kOK)
		{
			if (stream && !m_Callback.OnItemDone(m_Item, *stream))
			{
				return *HResult::Fail();
			}
			if (m_EvtHandler && !SendItemEvent(IArchiveExtract::EvtItemDone, std::move(m_Item)))
			{
				return *HResult::Abort();
			}
		}
		return *HResult::Success();
	}
}

namespace kxf::SevenZip::Private::Callback
{
	OutputStreamDelegate ExtractArchiveToFS::OnGetStream(const FileItem& item)
	{
		const bool hasTargetPath = GetTargetPath(item, m_TargetPath);
		if (!hasTargetPath)
		{
			m_TargetPath = m_Directory / item.GetFullPath();
		}
		if (!m_TargetPath)
		{
			m_ShouldCancel = true;
			return {};
		}

		if (m_TargetPath)
		{
			if (item.IsDirectory())
			{
				// Creating the directory here supports having empty directories but do we really need this? Probably not.
				//m_FileSystem.CreateDirectory(m_TargetPath);
				return {};
			}

			auto stream = m_FileSystem.OpenToWrite(m_TargetPath);
			if (!stream && m_FileSystem.CreateDirectory(m_TargetPath.GetParent()))
			{
				stream = m_FileSystem.OpenToWrite(m_TargetPath);
			}
			if (stream)
			{
				return stream;
			}
			m_ShouldCancel = true;
		}
		return {};
	}
	bool ExtractArchiveToFS::OnItemDone(const FileItem& item, IOutputStream& stream)
	{
		if (auto nativeStream = stream.QueryInterface<INativeStream>())
		{
			nativeStream->ChangeTimestamp(item.GetCreationTime(), item.GetModificationTime(), item.GetLastAccessTime());
			nativeStream->SetAttributes(item.GetAttributes());

			return true;
		}

		stream.Close();
		if (m_TargetPath)
		{
			m_FileSystem.ChangeAttributes(m_TargetPath, item.GetAttributes());
			m_FileSystem.ChangeTimestamp(m_TargetPath, item.GetCreationTime(), item.GetModificationTime(), item.GetLastAccessTime());
		}
		return true;
	}
}

namespace kxf::SevenZip::Private::Callback
{
	OutputStreamDelegate ExtractArchiveToStream::OnGetStream(const FileItem& item)
	{
		if (!item.IsDirectory())
		{
			return *m_Stream;
		}
		return {};
	}
	bool ExtractArchiveToStream::OnItemDone(const FileItem& item, IOutputStream& stream)
	{
		return true;
	}
}
