#include "kxf-pch.h"
#include "VariantProperty.h"
#include "COM.h"
#include "kxf/IO/IStream.h"
#include "kxf/Core/Any.h"
#include "kxf/Utility/ScopeGuard.h"
#include <propvarutil.h>
#include <propidlbase.h>
#pragma comment(lib, "Propsys.lib")

namespace
{
	using namespace kxf;

	HResult ClearPropVariant(PROPVARIANT& prop) noexcept
	{
		if (prop.vt == VT_EMPTY)
		{
			return S_OK;
		}

		switch (prop.vt)
		{
			case VT_UI1:
			case VT_I1:
			case VT_I2:
			case VT_UI2:
			case VT_BOOL:
			case VT_I4:
			case VT_UI4:
			case VT_R4:
			case VT_INT:
			case VT_UINT:
			case VT_ERROR:
			case VT_FILETIME:
			case VT_UI8:
			case VT_R8:
			case VT_CY:
			case VT_DATE:
			{
				prop.vt = VT_EMPTY;
				prop.wReserved1 = 0;
				return S_OK;
			}
		};
		return ::VariantClear(reinterpret_cast<VARIANTARG*>(&prop));
	}
	HResult CopyPropVariant(PROPVARIANT& destination, const PROPVARIANT& source)
	{
		::VariantClear(reinterpret_cast<VARIANTARG*>(&destination));
		destination.vt = VT_EMPTY;

		switch (source.vt)
		{
			case VT_UI1:
			case VT_I1:
			case VT_I2:
			case VT_UI2:
			case VT_BOOL:
			case VT_I4:
			case VT_UI4:
			case VT_R4:
			case VT_INT:
			case VT_UINT:
			case VT_ERROR:
			case VT_FILETIME:
			case VT_UI8:
			case VT_R8:
			case VT_CY:
			case VT_DATE:
			{
				std::memmove(reinterpret_cast<PROPVARIANT*>(&destination), &source, sizeof(PROPVARIANT));
				return S_OK;
			}
		};

		HResult hr = ::VariantCopy(reinterpret_cast<VARIANTARG*>(&destination), reinterpret_cast<const VARIANTARG*>(&source));
		if (!hr)
		{
			if (*hr == E_OUTOFMEMORY)
			{
				throw std::bad_alloc();
			}
			destination.vt = VT_ERROR;
			destination.scode = *hr;
		}
		return hr;
	}
	
	template<class T>
	constexpr int SimpleTriWayCompare(T left, T right) noexcept
	{
		if (left == right)
		{
			return 0;
		}
		else
		{
			return left < right ? -1 : 1;
		}
	}
}

namespace kxf
{
	HResult VariantProperty::DoClear() noexcept
	{
		HResult hr = ::ClearPropVariant(*m_PropVariant);
		if (!hr)
		{
			m_PropVariant->vt = VT_ERROR;
			m_PropVariant->scode = *hr;
		}
		return hr;
	}
	HResult VariantProperty::DoCopy(const tagPROPVARIANT& other)
	{
		return CopyPropVariant(*m_PropVariant, other);
	}
	HResult VariantProperty::DoMove(tagPROPVARIANT&& other) noexcept
	{
		if (&other != &m_PropVariant)
		{
			std::memcpy(reinterpret_cast<PROPVARIANT*>(&m_PropVariant), &other, sizeof(PROPVARIANT));

			std::memset(&other, 0, sizeof(PROPVARIANT));
			other.vt = VT_EMPTY;
			other.wReserved1 = 0;
		}
		return HResult::Success();
	}
	HResult VariantProperty::DoConvertToVariant(tagVARIANT& variant) const noexcept
	{
		return ::PropVariantToVariant(&m_PropVariant, &variant);
	}
	HResult VariantProperty::DoConvertFromVariant(const tagVARIANT& variant) noexcept
	{
		return ::VariantToPropVariant(&variant, &m_PropVariant);
	}

	uint64_t VariantProperty::Serialize(IOutputStream& stream) const
	{
		VARIANT variant;
		if (DoConvertToVariant(variant))
		{
			unsigned long rpcData = 0;
			uint64_t bufferSize = ::VARIANT_UserSize64(&rpcData, 0, &variant);

			std::vector<uint8_t> buffer;
			buffer.resize(static_cast<size_t>(bufferSize));
			auto result = ::VARIANT_UserMarshal64(&rpcData, buffer.data(), &variant);
			if (HResult(static_cast<HRESULT>(reinterpret_cast<size_t>(result))))
			{
				uint64_t written = Serialization::WriteObject(stream, bufferSize);
				written += stream.Write(buffer.data(), buffer.size()).LastWrite().ToBytes();

				if (stream.LastWrite().ToBytes() != bufferSize)
				{
					throw kxf::BinarySerializerException(__FUNCTION__ ": Could not write all bytes to the stream");
				}
				return written;
			}
		}
		return Serialization::WriteObject(stream, static_cast<uint64_t>(0));
	}
	uint64_t VariantProperty::Deserialize(IInputStream& stream)
	{
		uint64_t bufferSize = 0;
		uint64_t read = Serialization::ReadObject(stream, bufferSize);
		if (bufferSize == 0)
		{
			DoClear();
		}
		else
		{
			std::vector<uint8_t> buffer;
			buffer.resize(bufferSize);

			read += stream.Read(buffer.data(), bufferSize).LastRead().ToBytes();;
			if (stream.LastRead() != bufferSize)
			{
				throw kxf::BinarySerializerException(__FUNCTION__ ": Could not read the required amount of bytes");
			}

			VARIANT variant;
			unsigned long rpcData = 0;
			auto result = ::VARIANT_UserUnmarshal64(&rpcData, buffer.data(), &variant);
			if (HResult(static_cast<HRESULT>(reinterpret_cast<size_t>(result))))
			{
				DoConvertFromVariant(variant);
			}
			else
			{
				DoClear();
			}
		}
		return read;
	}

	void VariantProperty::AssignBool(bool value) noexcept
	{
		AssignSimpleValue<VT_BOOL>(m_PropVariant->boolVal, value ? VARIANT_TRUE : VARIANT_FALSE);
	}
	void VariantProperty::AssignUUID(const NativeUUID& value)
	{
		::InitVariantFromBuffer(&value, sizeof(value), reinterpret_cast<VARIANT*>(&m_PropVariant));
	}
	void VariantProperty::AssignString(std::string_view value)
	{
		DoClear();

		m_PropVariant->vt = VT_BSTR;
		m_PropVariant->wReserved1 = 0;
		m_PropVariant->bstrVal = ::SysAllocStringByteLen(nullptr, static_cast<UINT>(value.size() * sizeof(OLECHAR)));
		if (!m_PropVariant->bstrVal)
		{
			throw std::bad_alloc();
		}
		else
		{
			for (size_t i = 0; i < value.size(); i++)
			{
				m_PropVariant->bstrVal[i] = value[i];
			}
			m_PropVariant->bstrVal[value.size()] = 0;
		}
	}
	void VariantProperty::AssignString(std::wstring_view value)
	{
		DoClear();

		m_PropVariant->vt = VT_BSTR;
		m_PropVariant->wReserved1 = 0;
		m_PropVariant->bstrVal = ::SysAllocString(value.data());
		if (!m_PropVariant->bstrVal)
		{
			throw std::bad_alloc();
		}
	}
	void VariantProperty::AssignDateTime(const DateTime& value) noexcept
	{
		if (value)
		{
			AssignSimpleValue<VT_FILETIME>(m_PropVariant->filetime, value.GetFileTime(TimeZone::UTC));
		}
		else
		{
			DoClear();
		}
	}

	void VariantProperty::AssignInt(int8_t value) noexcept
	{
		AssignSimpleValue<VT_I1>(m_PropVariant->cVal, value);
	}
	void VariantProperty::AssignInt(int16_t value) noexcept
	{
		AssignSimpleValue<VT_I2>(m_PropVariant->iVal, value);
	}
	void VariantProperty::AssignInt(int32_t value) noexcept
	{
		AssignSimpleValue<VT_I4>(m_PropVariant->lVal, value);
	}
	void VariantProperty::AssignInt(int64_t value) noexcept
	{
		AssignSimpleValue<VT_I8>(m_PropVariant->hVal.QuadPart, value);
	}
	void VariantProperty::AssignInt(uint8_t value) noexcept
	{
		AssignSimpleValue<VT_UI1>(m_PropVariant->bVal, value);
	}
	void VariantProperty::AssignInt(uint16_t value) noexcept
	{
		AssignSimpleValue<VT_UI2>(m_PropVariant->uiVal, value);
	}
	void VariantProperty::AssignInt(uint32_t value) noexcept
	{
		AssignSimpleValue<VT_UI4>(m_PropVariant->ulVal, value);
	}
	void VariantProperty::AssignInt(uint64_t value) noexcept
	{
		AssignSimpleValue<VT_UI8>(m_PropVariant->uhVal.QuadPart, value);
	}

	void VariantProperty::AssignFloat(float value) noexcept
	{
		AssignSimpleValue<VT_R4>(m_PropVariant->fltVal, value);
	}
	void VariantProperty::AssignFloat(double value) noexcept
	{
		AssignSimpleValue<VT_R8>(m_PropVariant->dblVal, value);
	}

	std::optional<int8_t> VariantProperty::RetrieveInt8() const noexcept
	{
		return RetrieveSimpleValue<VT_I1>(m_PropVariant->cVal);
	}
	std::optional<int16_t> VariantProperty::RetrieveInt16() const noexcept
	{
		return RetrieveSimpleValue<VT_I2>(m_PropVariant->iVal);
	}
	std::optional<int32_t> VariantProperty::RetrieveInt32() const noexcept
	{
		return RetrieveSimpleValue<VT_I4>(m_PropVariant->lVal);
	}
	std::optional<int64_t> VariantProperty::RetrieveInt64() const noexcept
	{
		return RetrieveSimpleValue<VT_I8>(m_PropVariant->hVal.QuadPart);
	}
	std::optional<uint8_t> VariantProperty::RetrieveUInt8() const noexcept
	{
		return RetrieveSimpleValue<VT_UI1>(m_PropVariant->bVal);
	}
	std::optional<uint16_t> VariantProperty::RetrieveUInt16() const noexcept
	{
		return RetrieveSimpleValue<VT_UI2>(m_PropVariant->uiVal);
	}
	std::optional<uint32_t> VariantProperty::RetrieveUInt32() const noexcept
	{
		return RetrieveSimpleValue<VT_UI4>(m_PropVariant->ulVal);
	}
	std::optional<uint64_t> VariantProperty::RetrieveUInt64() const noexcept
	{
		return RetrieveSimpleValue<VT_UI8>(m_PropVariant->uhVal.QuadPart);
	}

	std::optional<float> VariantProperty::RetrieveFloat32() const noexcept
	{
		return RetrieveSimpleValue<VT_R4>(m_PropVariant->fltVal);
	}
	std::optional<double> VariantProperty::RetrieveFloat64() const noexcept
	{
		return RetrieveSimpleValue<VT_R8>(m_PropVariant->fltVal);
	}

	VariantProperty::VariantProperty() noexcept
	{
		m_PropVariant->vt = VT_EMPTY;
		m_PropVariant->wReserved1 = 0;
	}
	VariantProperty::~VariantProperty() noexcept
	{
		DoClear();
	}

	int VariantProperty::GetNativeType() const noexcept
	{
		return m_PropVariant->vt;
	}
	void VariantProperty::SetNativeType(int nativeType) noexcept
	{
		m_PropVariant->vt = nativeType;
	}
	
	bool VariantProperty::IsEmpty() const noexcept
	{
		return m_PropVariant->vt == VARENUM::VT_EMPTY;
	}
	VariantPropertyType VariantProperty::GetType() const noexcept
	{
		switch (m_PropVariant->vt)
		{
			case VARENUM::VT_EMPTY:
			{
				return VariantPropertyType::None;
			}

			case VARENUM::VT_BOOL:
			{
				return VariantPropertyType::Bool;
			}
			case VARENUM::VT_BSTR:
			case VARENUM::VT_LPSTR:
			case VARENUM::VT_LPWSTR:
			{
				return VariantPropertyType::String;
			}
			case VARENUM::VT_FILETIME:
			case VARENUM::VT_DATE:
			{
				return VariantPropertyType::DateTime;
			}
			case VARENUM::VT_ARRAY|VT_UI1:
			{
				return VariantPropertyType::UUID;
			}

			case VARENUM::VT_I1:
			{
				return VariantPropertyType::Int8;
			}
			case VARENUM::VT_UI1:
			{
				return VariantPropertyType::UInt8;
			}
			case VARENUM::VT_I2:
			{
				return VariantPropertyType::Int16;
			}
			case VARENUM::VT_UI2:
			{
				return VariantPropertyType::UInt16;
			}
			case VARENUM::VT_I4:
			{
				return VariantPropertyType::Int32;
			}
			case VARENUM::VT_UI4:
			{
				return VariantPropertyType::UInt32;
			}
			case VARENUM::VT_I8:
			{
				return VariantPropertyType::Int64;
			}
			case VARENUM::VT_UI8:
			{
				return VariantPropertyType::UInt64;
			}

			case VARENUM::VT_R4:
			{
				return VariantPropertyType::Float32;
			}
			case VARENUM::VT_R8:
			{
				return VariantPropertyType::Float64;
			}
		}
		return VariantPropertyType::Unknown;
	}
	int VariantProperty::Compare(const VariantProperty& other) const noexcept
	{
		auto CompareDefault = [&]()
		{
			return ::PropVariantCompareEx(*m_PropVariant, *other.m_PropVariant, PVCU_DEFAULT, PVCF_USESTRCMP);
		};

		if (m_PropVariant->vt != other.m_PropVariant->vt)
		{
			return CompareDefault();
		}

		switch (m_PropVariant->vt)
		{
			case VT_EMPTY:
			{
				return 0;
			}

			case VT_BOOL:
			{
				return -SimpleTriWayCompare(m_PropVariant->boolVal, other.m_PropVariant->boolVal);
			}
			case VT_BSTR:
			{
				if (m_PropVariant->bstrVal && other.m_PropVariant->bstrVal)
				{
					using Traits = std::char_traits<OLECHAR>;
					return Traits::compare(m_PropVariant->bstrVal, other.m_PropVariant->bstrVal, std::min(Traits::length(m_PropVariant->bstrVal), Traits::length(other.m_PropVariant->bstrVal)));
				}
				return SimpleTriWayCompare(m_PropVariant->bstrVal, other.m_PropVariant->bstrVal);
			}
			case VT_LPSTR:
			{
				return ::PropVariantCompareEx(*m_PropVariant, *other.m_PropVariant, PVCU_DEFAULT, PVCF_USESTRCMPC);
			}
			case VT_LPWSTR:
			{
				return ::PropVariantCompareEx(*m_PropVariant, *other.m_PropVariant, PVCU_DEFAULT, PVCF_USESTRCMP);
			}
			case VT_FILETIME:
			{
				return ::CompareFileTime(&m_PropVariant->filetime, &other.m_PropVariant->filetime);
			}
			case VT_DATE:
			{
				return ::SimpleTriWayCompare(m_PropVariant->date, other.m_PropVariant->date);
			}
			case VT_CLSID:
			{
				if (m_PropVariant->puuid && other.m_PropVariant->puuid)
				{
					if (COM::FromGUID(*m_PropVariant->puuid) == COM::FromGUID(*other.m_PropVariant->puuid))
					{
						return 0;
					}
				}
				return ::SimpleTriWayCompare(m_PropVariant->puuid, other.m_PropVariant->puuid);
			}

			case VT_I1:
			{
				return SimpleTriWayCompare(m_PropVariant->cVal, other.m_PropVariant->cVal);
			}
			case VT_UI1:
			{
				return SimpleTriWayCompare(m_PropVariant->bVal, other.m_PropVariant->bVal);
			}
			case VT_I2:
			{
				return SimpleTriWayCompare(m_PropVariant->iVal, other.m_PropVariant->iVal);
			}
			case VT_UI2:
			{
				return SimpleTriWayCompare(m_PropVariant->uiVal, other.m_PropVariant->uiVal);
			}
			case VT_I4:
			{
				return SimpleTriWayCompare(m_PropVariant->lVal, other.m_PropVariant->lVal);
			}
			case VT_UI4:
			{
				return SimpleTriWayCompare(m_PropVariant->ulVal, other.m_PropVariant->ulVal);
			}
			case VT_I8:
			{
				return SimpleTriWayCompare(m_PropVariant->hVal.QuadPart, other.m_PropVariant->hVal.QuadPart);
			}
			case VT_UI8:
			{
				return SimpleTriWayCompare(m_PropVariant->uhVal.QuadPart, other.m_PropVariant->uhVal.QuadPart);
			}

			case VT_R4:
			{
				return SimpleTriWayCompare(m_PropVariant->fltVal, other.m_PropVariant->fltVal);
			}
			case VT_R8:
			{
				return SimpleTriWayCompare(m_PropVariant->dblVal, other.m_PropVariant->dblVal);
			}
		};
		return CompareDefault();
	}
	HResult VariantProperty::CopyToNative(tagPROPVARIANT& nativeProperty) const
	{
		return CopyPropVariant(nativeProperty, *m_PropVariant);
	}

	std::optional<bool> VariantProperty::ToBool() const noexcept
	{
		if (m_PropVariant->vt == VT_BOOL)
		{
			return m_PropVariant->boolVal != VARIANT_FALSE;
		}
		return {};
	}
	std::optional<NativeUUID> VariantProperty::ToUUID() const noexcept
	{
		if (m_PropVariant->vt == (VT_ARRAY|VT_I1))
		{
			NativeUUID uuid;
			if (HResult(::PropVariantToBuffer(*m_PropVariant, &uuid, sizeof(uuid))))
			{
				return uuid;
			}
		}
		return {};
	}
	std::optional<String> VariantProperty::ToString() const
	{
		switch (m_PropVariant->vt)
		{
			case VT_BSTR:
			{
				return m_PropVariant->bstrVal;
			}
			case VT_LPSTR:
			{
				return m_PropVariant->pszVal;
			}
			case VT_LPWSTR:
			{
				return m_PropVariant->pwszVal;
			}
		};
		return {};
	}
	std::optional<DateTime> VariantProperty::ToDateTime() const noexcept
	{
		FILETIME fileTime = {};
		if (HResult(::PropVariantToFileTime(*m_PropVariant, PSTF_LOCAL, &fileTime)))
		{
			return DateTime().SetFileTime(fileTime);
		}
		return {};
	}

	Any VariantProperty::ToAny() const
	{
		switch (GetType())
		{
			case VariantPropertyType::Bool:
			{
				return *ToBool();
			}
			case VariantPropertyType::String:
			{
				return *ToString();
			}
			case VariantPropertyType::DateTime:
			{
				return *ToDateTime();
			}
			case VariantPropertyType::UUID:
			{
				return *ToUUID();
			}

			case VariantPropertyType::Int8:
			{
				return *RetrieveInt8();
			}
			case VariantPropertyType::Int16:
			{
				return *RetrieveInt16();
			}
			case VariantPropertyType::Int32:
			{
				return *RetrieveInt32();
			}
			case VariantPropertyType::Int64:
			{
				return *RetrieveInt64();
			}

			case VariantPropertyType::UInt8:
			{
				return *RetrieveUInt8();
			}
			case VariantPropertyType::UInt16:
			{
				return *RetrieveUInt16();
			}
			case VariantPropertyType::UInt32:
			{
				return *RetrieveUInt32();
			}
			case VariantPropertyType::UInt64:
			{
				return *RetrieveUInt64();
			}

			case VariantPropertyType::Float32:
			{
				return *RetrieveFloat32();
			}
			case VariantPropertyType::Float64:
			{
				return *RetrieveFloat64();
			}
		};
		return {};
	}
}
