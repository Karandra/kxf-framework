#pragma once
#include "kxf/Core/StdID.h"
#include "kxf/Utility/Numeric.h"

namespace kxf
{
	class WidgetID final
	{
		private:
			using TInt = std::underlying_type_t<StdID>;

		private:
			static constexpr TInt CastToInt(StdID id) noexcept
			{
				return static_cast<TInt>(id);
			}

		private:
			StdID m_ID = StdID::None;

		public:
			constexpr WidgetID() noexcept = default;
			constexpr WidgetID(StdID id) noexcept
				:m_ID(id)
			{
			}
			constexpr WidgetID(TInt id) noexcept
				:m_ID(static_cast<StdID>(id))
			{
			}

		public:
			constexpr StdID GetValue() const noexcept
			{
				return m_ID;
			}

			constexpr bool IsAny() const noexcept
			{
				return m_ID == StdID::Any;
			}
			constexpr bool IsNone() const noexcept
			{
				return m_ID == StdID::None;
			}
			constexpr bool IsSeparator() const noexcept
			{
				return m_ID == StdID::Separator;
			}
			constexpr bool IsAutomatic() const noexcept
			{
				return Utility::TestRange(CastToInt(m_ID), CastToInt(StdID::WX_AUTO_LOWEST), CastToInt(StdID::WX_AUTO_HIGHEST));
			}
			constexpr bool IsStandard() const noexcept
			{
				return IsWxStandard() || Utility::TestRange(CastToInt(m_ID), CastToInt(StdID::KX_LOWEST), CastToInt(StdID::KX_HIGHEST));
			}
			constexpr bool IsWxStandard() const noexcept
			{
				return Utility::TestRange(CastToInt(m_ID), CastToInt(StdID::WX_LOWEST), CastToInt(StdID::WX_HIGHEST));
			}
			constexpr bool IsUserDefined() const noexcept
			{
				return !IsAutomatic() && !IsStandard();
			}

			template<std::integral T = TInt>
			constexpr T ToInt() const noexcept
			{
				return static_cast<T>(CastToInt(m_ID));
			}

		public:
			constexpr auto operator<=>(const WidgetID&) const noexcept = default;
			constexpr bool operator==(const WidgetID&) const noexcept = default;

			constexpr StdID operator*() const noexcept
			{
				return m_ID;
			}
	};
}
