#pragma once
#include "Common.h"
#include "kxf/Core/IErrorCode.h"

namespace kxf
{
	class KXF_API ErrorCode final: public RTTI::Implementation<ErrorCode, IErrorCode>
	{
		private:
			IID m_IID;
			uint32_t m_Value = std::numeric_limits<uint32_t>::max();

		public:
			ErrorCode(uint32_t errorCode = std::numeric_limits<uint32_t>::max()) noexcept
				:m_Value(errorCode)
			{
			}

			template<std::derived_from<IErrorCode> T>
			ErrorCode(const T& errorCode) noexcept
				:m_IID(RTTI::GetInterfaceID<T>()), m_Value(errorCode.GetValue())
			{
			}

			ErrorCode(ErrorCode&&) noexcept = default;
			ErrorCode(const ErrorCode&) = delete;

		public:
			// IErrorCode
			bool IsSuccess() const noexcept override;
			bool IsFail() const noexcept override;

			uint32_t GetValue() const noexcept override
			{
				return m_Value;
			}
			void SetValue(uint32_t value) noexcept override
			{
				m_Value = value;
			}

			String ToString() const override;
			String GetMessage(const Locale& locale = {}) const;

			// ErrorCode
			bool IsTyped() const noexcept
			{
				return !m_IID.IsNull();
			}
			IID GetIID() const noexcept
			{
				return m_IID;
			}

			bool IsSameAs(const ErrorCode& other) const noexcept;
			bool IsSameAs(const IErrorCode& other) const noexcept;

			template<std::derived_from<IErrorCode> T>
			bool IsSameAs(const T& other) const noexcept
			{
				if (m_IID && RTTI::GetInterfaceID<T>() == m_IID)
				{
					return m_Value == other.GetValue();
				}
				return false;
			}

			std::shared_ptr<IErrorCode> CreateTyped() const;

			template<std::derived_from<IErrorCode> T>
			requires(std::is_constructible_v<T, uint32_t>)
			std::optional<T> ConvertToTyped() const
			{
				if (m_IID && RTTI::GetInterfaceID<T>() == m_IID)
				{
					return T(m_Value);
				}
				return {};
			}

		public:
			ErrorCode& operator=(ErrorCode&&) noexcept = default;
			ErrorCode& operator=(const ErrorCode&) = delete;
	};
}
