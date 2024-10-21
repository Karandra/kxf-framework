#include "KxfPCH.h"
#include "ErrorCode.h"

namespace
{
	template<class TFunc, class T = kxf::IErrorCode>
	bool CreateAndCall(const kxf::IID& iid, uint32_t value, TFunc&& func)
	{
		if (auto classInfo = kxf::RTTI::GetClassInfoByInterfaceID(iid))
		{
			std::byte buffer[256] = {};
			if (auto ptr = classInfo->CreateObjectInstanceAt<T>(buffer, sizeof(buffer)))
			{
				ptr->SetValue(value);
				std::invoke(func, *ptr);
				std::destroy_at(ptr);

				return true;
			}
		}
		return false;
	}
}

namespace kxf
{
	// IErrorCode
	bool ErrorCode::IsSuccess() const noexcept
	{
		if (m_IID)
		{
			bool result = false;
			CreateAndCall(m_IID, m_Value, [&](IErrorCode& errorCode)
			{
				result = errorCode.IsSuccess();
			});

			return result;
		}
		else
		{
			return m_Value == 0;
		}
	}
	bool ErrorCode::IsFail() const noexcept
	{
		if (m_IID)
		{
			bool result = true;
			CreateAndCall(m_IID, m_Value, [&](IErrorCode& errorCode)
			{
				result = errorCode.IsFail();
			});

			return result;
		}
		else
		{
			return m_Value != 0;
		}
	}

	String ErrorCode::ToString() const
	{
		if (m_IID)
		{
			String result;
			CreateAndCall(m_IID, m_Value, [&](IErrorCode& errorCode)
			{
				result = errorCode.ToString();
			});

			return result;
		}
		return {};
	}
	String ErrorCode::GetMessage(const Locale& locale) const
	{
		if (m_IID)
		{
			String result;
			CreateAndCall(m_IID, m_Value, [&](IErrorCode& errorCode)
			{
				result = errorCode.GetMessage();
			});

			return result;
		}
		return {};
	}

	// ErrorCode
	bool ErrorCode::IsSameAs(const ErrorCode& other) const noexcept
	{
		if (this == &other)
		{
			return true;
		}
		else if ((!m_IID && !other.m_IID) || (m_IID == other.m_IID))
		{
			return m_Value == other.m_Value;
		}
		return false;
	}
	bool ErrorCode::IsSameAs(const IErrorCode& other) const noexcept
	{
		if (this == &other)
		{
			return true;
		}
		else if (m_IID)
		{
			if (auto classInfo = other.QueryInterface<RTTI::ClassInfo>(); classInfo && classInfo->GetIID() == m_IID)
			{
				return m_Value == other.GetValue();
			}
		}
		return false;
	}

	std::shared_ptr<IErrorCode> ErrorCode::CreateTyped() const
	{
		if (m_IID)
		{
			if (auto classInfo = RTTI::GetClassInfoByInterfaceID(m_IID))
			{
				if (auto ptr = classInfo->CreateObjectInstance<IErrorCode>())
				{
					ptr->SetValue(m_Value);
					return ptr;
				}
			}
		}
		return {};
	}
}
