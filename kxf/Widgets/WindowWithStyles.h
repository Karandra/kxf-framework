#pragma once
#include "Common.h"
#include <wx/window.h>

namespace kxf::UI
{
	template<class TWindow, class TEnum>
	class WindowWithStyles
	{
		private:
			TWindow& Self() noexcept
			{
				return static_cast<TWindow&>(*this);
			}
			const TWindow& Self() const noexcept
			{
				return static_cast<const TWindow&>(*this);
			}

		public:
			FlagSet<TEnum> GetWindowStyle() const
			{
				return FlagSet<TEnum>().FromInt(Self().GetWindowStyleFlag());
			}
			bool ContainsWindowStyle(FlagSet<TEnum> styles) const
			{
				return GetWindowStyle().Contains(styles);
			}
			
			TWindow& SetWindowStyle(FlagSet<TEnum> styles)
			{
				Self().SetWindowStyleFlag(styles.ToInt());
				return Self();
			}
			TWindow& ModWindowStyle(FlagSet<TEnum> styles, bool enable)
			{
				SetWindowStyle(GetWindowStyle().Mod(styles, enable));
				return Self();
			}
			TWindow& ToggleWindowStyle(FlagSet<TEnum> styles)
			{
				SetWindowStyle(GetWindowStyle().Toggle(styles));
				return Self();
			}
	};

	template<class TWindow, class TEnum>
	class WindowWithExtraStyles
	{
		private:
			TWindow& Self() noexcept
			{
				return static_cast<TWindow&>(*this);
			}
			const TWindow& Self() const noexcept
			{
				return static_cast<const TWindow&>(*this);
			}

		public:
			FlagSet<TEnum> GetWindowExStyle() const
			{
				return FlagSet<TEnum>().FromInt(Self().GetExtraStyle());
			}
			bool ContainsWindowExStyle(FlagSet<TEnum> styles) const
			{
				return GetWindowExStyle().Contains(styles);
			}
			
			TWindow& SetWindowExStyle(FlagSet<TEnum> styles)
			{
				Self().SetExtraStyle(styles.ToInt());
				return Self();
			}
			TWindow& ModWindowExStyle(FlagSet<TEnum> styles, bool enable)
			{
				SetWindowExStyle(GetWindowExStyle().Mod(styles, enable));
				return Self();
			}
			TWindow& ToggleWindowExStyle(FlagSet<TEnum> styles)
			{
				SetWindowExStyle(GetWindowExStyle().Toggle(styles));
				return Self();
			}
	};
}
