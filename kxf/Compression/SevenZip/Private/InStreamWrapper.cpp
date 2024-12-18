#include "kxf-pch.h"
#include "InStreamWrapper.h"
#include "Utility.h"

namespace kxf::SevenZip::Private
{
	HRESULT STDMETHODCALLTYPE InStream::QueryInterface(const ::IID& iid, void** ppvObject)
	{
		if (iid == __uuidof(IUnknown))
		{
			*ppvObject = reinterpret_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_ISequentialInStream)
		{
			*ppvObject = static_cast<ISequentialInStream*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_IInStream)
		{
			*ppvObject = static_cast<IInStream*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_IStreamGetSize)
		{
			*ppvObject = static_cast<IStreamGetSize*>(this);
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
}

namespace kxf::SevenZip::Private
{
	STDMETHODIMP InStreamWrapper::Read(void* data, UInt32 size, UInt32* processedSize)
	{
		uint32_t read = 0;
		HResult hr = DoRead(data, size, read);
		if (processedSize)
		{
			*processedSize = read;
			m_BytesRead += read;
		}

		if (m_EvtHandler && !SentWriteEvent())
		{
			return *HResult::Abort();
		}
		return hr ? *HResult::Success() : *hr;
	}
	STDMETHODIMP InStreamWrapper::Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition)
	{
		int64_t newPos = 0;
		HResult hr = DoSeek(offset, seekOrigin, newPos);
		if (newPosition)
		{
			*newPosition = newPos;
			m_BytesRead = *newPosition;
		}
		return *hr;
	}
	STDMETHODIMP InStreamWrapper::GetSize(UInt64* size)
	{
		int64_t streamSize = 0;
		HResult hr = DoGetSize(streamSize);
		if (hr && size)
		{
			*size = streamSize;
		}
		return *hr;
	}
}
