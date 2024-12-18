#pragma once
#include "Common.h"
#include "Win32Error.h"

namespace kxf
{
	class KXF_API DisableWOW64FSRedirection final
	{
		private:
			void* m_Value = nullptr;
			Win32Error m_ErrorCode = Win32Error::Fail();

		public:
			DisableWOW64FSRedirection() noexcept;
			DisableWOW64FSRedirection(const DisableWOW64FSRedirection&) = delete;
			DisableWOW64FSRedirection(DisableWOW64FSRedirection&& other)
			{
				*this = std::move(other);
			}
			~DisableWOW64FSRedirection() noexcept;

		public:
			explicit operator bool() const noexcept
			{
				return m_ErrorCode.IsSuccess();
			}
			bool operator!() const noexcept
			{
				return m_ErrorCode.IsFail();
			}

			DisableWOW64FSRedirection& operator=(const DisableWOW64FSRedirection&) = delete;
			DisableWOW64FSRedirection& operator=(DisableWOW64FSRedirection&& other) noexcept
			{
				m_Value = other.m_Value;
				other.m_Value = nullptr;

				m_ErrorCode = other.m_ErrorCode;
				other.m_ErrorCode = Win32Error::Fail();

				return *this;
			}
	};
}
