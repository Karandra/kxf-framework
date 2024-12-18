#include "kxf-pch.h"
#include "SafeArray.h"
#include "COM.h"

namespace kxf
{
	HResult SafeArray::DoClear() noexcept
	{
		if (auto ptr = Detach())
		{
			return ::SafeArrayDestroy(ptr);
		}
		return HResult::False();
	}
	HResult SafeArray::DoCopy(const tagSAFEARRAY& other) noexcept
	{
		if (m_SafeArray != &other)
		{
			HResult hr = HResult::Fail();
			return ::SafeArrayCopy(const_cast<SAFEARRAY*>(&other), &m_SafeArray);
		}
		return HResult::False();
	}

	HResult SafeArray::AccessData(void**& data) noexcept
	{
		return ::SafeArrayAccessData(m_SafeArray, data);
	}
	void SafeArray::UnaccessData() noexcept
	{
		::SafeArrayUnaccessData(m_SafeArray);
	}

	SafeArray::SafeArray() noexcept = default;
	SafeArray::SafeArray(const tagSAFEARRAY& other) noexcept
	{
		DoCopy(other);
	}
	SafeArray::SafeArray(const SafeArray& other) noexcept
	{
		DoCopy(*other.m_SafeArray);
	}
	SafeArray::SafeArray(SafeArray&& other) noexcept
	{
		m_SafeArray = other.Detach();
	}
	SafeArray::~SafeArray() noexcept
	{
		DoClear();
	}

	bool SafeArray::IsNull() const noexcept
	{
		if (m_SafeArray)
		{
			VARTYPE type = VT_NULL;
			if (HResult result = ::SafeArrayGetVartype(m_SafeArray, &type))
			{
				return type == VT_NULL;
			}
			return false;
		}
		return true;
	}
	void SafeArray::Attach(tagSAFEARRAY* ptr) noexcept
	{
		DoClear();
		m_SafeArray = ptr;
	}
	tagSAFEARRAY* SafeArray::Detach() noexcept
	{
		auto ptr = m_SafeArray;
		m_SafeArray = nullptr;
		return ptr;
	}

	bool SafeArray::IsEmpty() const noexcept
	{
		if (m_SafeArray)
		{
			VARTYPE type = VT_NULL;
			if (HResult result = ::SafeArrayGetVartype(m_SafeArray, &type))
			{
				return type == VT_NULL || type == VT_EMPTY;
			}
			return true;
		}
		return true;
	}
	size_t SafeArray::GetSize(size_t dimension) const noexcept
	{
		if (m_SafeArray)
		{
			LONG lower = 0;
			LONG upper = 0;

			if (HResult(::SafeArrayGetLBound(m_SafeArray, static_cast<UINT>(dimension), &lower)) && (HResult(::SafeArrayGetUBound(m_SafeArray, static_cast<UINT>(dimension), &upper))))
			{
				return upper - lower + 1;
			}
		}
		return 0;
	}
	size_t SafeArray::GetDimensions() const noexcept
	{
		return m_SafeArray ? ::SafeArrayGetDim(m_SafeArray) : 0;
	}
	SafeArrayType SafeArray::GetType() const noexcept
	{
		if (!m_SafeArray)
		{
			return SafeArrayType::None;
		}

		VARTYPE type = VT_NULL;
		if (HResult result = ::SafeArrayGetVartype(m_SafeArray, &type))
		{
			switch (type)
			{
				case VARENUM::VT_NULL:
				case VARENUM::VT_EMPTY:
				{
					return SafeArrayType::None;
				}

				case VARENUM::VT_BOOL:
				{
					return SafeArrayType::Bool;
				}
				case VARENUM::VT_BSTR:
				case VARENUM::VT_LPSTR:
				case VARENUM::VT_LPWSTR:
				{
					return SafeArrayType::String;
				}
				case VARENUM::VT_FILETIME:
				case VARENUM::VT_DATE:
				{
					return SafeArrayType::DateTime;
				}
				case VARENUM::VT_ARRAY|VT_UI1:
				{
					return SafeArrayType::UUID;
				}

				case VARENUM::VT_I1:
				{
					return SafeArrayType::Int8;
				}
				case VARENUM::VT_UI1:
				{
					return SafeArrayType::UInt8;
				}
				case VARENUM::VT_I2:
				{
					return SafeArrayType::Int16;
				}
				case VARENUM::VT_UI2:
				{
					return SafeArrayType::UInt16;
				}
				case VARENUM::VT_I4:
				{
					return SafeArrayType::Int32;
				}
				case VARENUM::VT_UI4:
				{
					return SafeArrayType::UInt32;
				}
				case VARENUM::VT_I8:
				{
					return SafeArrayType::Int64;
				}
				case VARENUM::VT_UI8:
				{
					return SafeArrayType::UInt64;
				}

				case VARENUM::VT_R4:
				{
					return SafeArrayType::Float32;
				}
				case VARENUM::VT_R8:
				{
					return SafeArrayType::Float64;
				}
			};
		}
		return SafeArrayType::Unknown;
	}

	SafeArray& SafeArray::operator=(const SafeArray& other) noexcept
	{
		DoClear();
		DoCopy(*other.m_SafeArray);

		return *this;
	}
	SafeArray& SafeArray::operator=(SafeArray&& other) noexcept
	{
		Attach(other.Detach());
		return *this;
	}
}
