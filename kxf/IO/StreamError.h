#pragma once
#include "Common.h"
#include "kxf/Core/IErrorCode.h"

namespace kxf
{
	enum class StreamErrorCode: uint32_t
	{
		UnknownError = std::numeric_limits<uint32_t>::max(),
		Success = 0,

		ReadError,
		WriteError,
		EndOfStream,
		ReadOnly
	};
}

namespace kxf
{
	class StreamError final: public RTTI::Implementation<StreamError, IErrorCode>
	{
		kxf_RTTI_DeclareIID(StreamError, {0x2c783dc3, 0x9106, 0x4dc9, {0x86, 0xb0, 0x5f, 0x94, 0xe1, 0xe4, 0x37, 0x8f}});

		public:
			static StreamError Success() noexcept
			{
				return StreamErrorCode::Success;
			}
			static StreamError Fail() noexcept
			{
				return StreamErrorCode::UnknownError;
			}

		private:
			StreamErrorCode m_Value = StreamErrorCode::UnknownError;

		public:
			StreamError(StreamErrorCode value) noexcept
				:m_Value(value)
			{
			}
			
		public:
			// IErrorCode
			bool IsSuccess() const noexcept override
			{
				return m_Value == StreamErrorCode::Success;
			}
			bool IsFail() const noexcept override
			{
				return m_Value != StreamErrorCode::Success;
			}

			uint32_t GetValue() const noexcept override
			{
				return static_cast<uint32_t>(m_Value);
			}
			void SetValue(uint32_t value) noexcept override
			{
				m_Value = static_cast<StreamErrorCode>(value);
			}

			String ToString() const override
			{
				return {};
			}
			String GetMessage(const Locale& locale = {}) const override
			{
				return {};
			}

			// StreamError
			StreamErrorCode GetCode() const noexcept
			{
				return m_Value;
			}
			void SetCode(StreamErrorCode value) noexcept
			{
				m_Value = value;
			}

		public:
			bool operator==(const StreamError& other) const noexcept
			{
				return m_Value == other.m_Value;
			}
			bool operator!=(const StreamError& other) const noexcept
			{
				return m_Value != other.m_Value;
			}
	};
}
