#pragma once
#include "../Common.h"
#include "kxf/Core/String.h"
#include "kxf/Core/CallbackFunction.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class SystemProcess;
	class SystemWindow;
}

namespace kxf
{
	class SystemThreadPriority final
	{
		public:
			static SystemThreadPriority Idle() noexcept;
			static SystemThreadPriority Lowest() noexcept;
			static SystemThreadPriority BelowNormal() noexcept;
			static SystemThreadPriority Normal() noexcept;
			static SystemThreadPriority AboveNormal() noexcept;
			static SystemThreadPriority Highest() noexcept;
			static SystemThreadPriority TimeCritical() noexcept;

		private:
			int m_Value = std::numeric_limits<int>::min();

		public:
			SystemThreadPriority() noexcept = default;
			SystemThreadPriority(int value) noexcept
				:m_Value(value)
			{
			}

		public:
			bool IsNull() const noexcept
			{
				return m_Value == std::numeric_limits<int>::min() || m_Value == std::numeric_limits<int>::max();
			}

			int GetValue() const noexcept
			{
				return m_Value;
			}
			void SetValue(int value) noexcept
			{
				m_Value = value;
			}
			bool IsStandardValue() const noexcept;

			bool IsIdle() const noexcept
			{
				return *this == Idle();
			}
			bool IsLowest() const noexcept
			{
				return *this == Lowest();
			}
			bool IsBelowNormal() const noexcept
			{
				return *this == BelowNormal();
			}
			bool IsNormal() const noexcept
			{
				return *this == Normal();
			}
			bool IsAboveNormal() const noexcept
			{
				return *this == AboveNormal();
			}
			bool IsHighest() const noexcept
			{
				return *this == Highest();
			}
			bool IsTimeCritical() const noexcept
			{
				return *this == TimeCritical();
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
			int operator*() const noexcept
			{
				return m_Value;
			}

			bool operator==(const SystemThreadPriority& other) const noexcept
			{
				return m_Value == other.m_Value;
			}
			bool operator!=(const SystemThreadPriority& other) const noexcept
			{
				return m_Value != other.m_Value;
			}
			auto operator<=>(const SystemThreadPriority& other) const noexcept
			{
				return m_Value <=> other.m_Value;
			}
	};
}

namespace kxf
{
	class KXF_API ISystemThread: public RTTI::Interface<ISystemThread>
	{
		kxf_RTTI_DeclareIID(ISystemThread, {0x22ff5e15, 0xd0b3, 0x4ed0, {0x83, 0x89, 0x48, 0xd0, 0xbd, 0x10, 0x72, 0xa1}});

		public:
			virtual ~ISystemThread() = default;

		public:
			virtual bool IsNull() const = 0;
			virtual bool IsCurrent() const = 0;
			virtual bool Is64Bit() const = 0;
			virtual uint32_t GetID() const = 0;
			virtual SystemProcess GetOwningProcess() const = 0;

			virtual SystemThreadPriority GetPriority() const = 0;
			virtual bool SetPriority(SystemThreadPriority priority) = 0;

			virtual String GetDescription() const = 0;
			virtual bool SetDescription(const String& description) = 0;

			virtual bool IsRunning() const = 0;
			virtual std::optional<uint32_t> GetExitCode() const = 0;
			virtual bool Terminate(uint32_t exitCode) = 0;

			virtual bool Suspend() = 0;
			virtual bool Resume() = 0;

			virtual size_t EnumWindows(std::function<CallbackCommand(SystemWindow)> func) const = 0;
	};
}
