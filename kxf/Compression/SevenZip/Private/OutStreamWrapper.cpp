#include "kxf-pch.h"
#include "OutStreamWrapper.h"
#include "Utility.h"

namespace kxf::SevenZip::Private
{
	HRESULT STDMETHODCALLTYPE OutStream::QueryInterface(const ::IID& iid, void** ppvObject)
	{
		if (iid == __uuidof(IUnknown))
		{
			*ppvObject = static_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_ISequentialOutStream)
		{
			*ppvObject = static_cast<ISequentialOutStream*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_IOutStream)
		{
			*ppvObject = static_cast<IOutStream*>(this);
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
}

namespace kxf::SevenZip::Private
{
	STDMETHODIMP OutStreamWrapper::Write(const void* data, UInt32 size, UInt32* written)
	{
		uint32_t writtenBase = 0;
		HResult hr = DoWrite(data, size, writtenBase);
		m_BytesWritten += writtenBase;

		if (written)
		{
			*written = writtenBase;
		}
		if (m_BytesTotal != -1)
		{
			m_BytesTotal += writtenBase;
		}

		if (m_EvtHandler && !SentWriteEvent())
		{
			return *HResult::Abort();
		}
		return *hr;
	}
	STDMETHODIMP OutStreamWrapper::Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition)
	{
		int64_t newPos = 0;
		HResult hr = DoSeek(offset, seekOrigin, newPos);
		if (newPosition)
		{
			*newPosition = newPos;
		}
		return *hr;
	}
	STDMETHODIMP OutStreamWrapper::SetSize(UInt64 newSize)
	{
		m_BytesTotal = newSize;
		HResult hr = DoSetSize(newSize);

		if (m_EvtHandler && !SentWriteEvent())
		{
			return *HResult::Abort();
		}
		return *hr;
	}
}
