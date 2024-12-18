#pragma once
#include "Common.h"

namespace kxf::Private
{
	template<class TDerived>
	class WidgetDIP
	{
		private:
			TDerived& Self() noexcept
			{
				return static_cast<TDerived&>(*this);
			}
			const TDerived& Self() const noexcept
			{
				return static_cast<const TDerived&>(*this);
			}

			void DoFromDIP(int& x, int& y) const
			{
				Self().FromDIP(x, y);
			}
			void DoToDIP(int& x, int& y) const
			{
				Self().ToDIP(x, y);
			}

		public:
			int FromDIP(int x) const
			{
				int y = Geometry::DefaultCoord;
				DoFromDIP(x, y);

				return x;
			}
			Size FromDIP(Size size) const
			{
				DoFromDIP(size.Width(), size.Height());
				return size;
			}
			Point FromDIP(Point point) const
			{
				DoFromDIP(point.X(), point.Y());
				return point;
			}

			template<std::constructible_from<int, int> T>
			T FromDIP(int x, int y) const
			{
				DoFromDIP(x, y);
				return {x, y};
			}

			int ToDIP(int x) const
			{
				int y = Geometry::DefaultCoord;
				DoToDIP(x, y);

				return x;
			}
			Size ToDIP(Size size) const
			{
				DoToDIP(size.Width(), size.Height());
				return size;
			}
			Point ToDIP(Point point) const
			{
				DoToDIP(point.X(), point.Y());
				return point;
			}

			template<std::constructible_from<int, int> T>
			T ToDIP(int x, int y) const
			{
				DoToDIP(x, y);
				return {x, y};
			}
	};

	template<class TDerived>
	class WidgetDLU
	{
		private:
			TDerived& Self() noexcept
			{
				return static_cast<TDerived&>(*this);
			}
			const TDerived& Self() const noexcept
			{
				return static_cast<const TDerived&>(*this);
			}

			void DoDialogUnitsToPixels(int& x, int& y) const
			{
				Self().DialogUnitsToPixels(x, y);
			}
			void DoPixelsToDialogUnits(int& x, int& y) const
			{
				Self().PixelsToDialogUnits(x, y);
			}

		public:
			Size DialogUnitsToPixels(Size size) const
			{
				DoDialogUnitsToPixels(size.Width(), size.Height());
				return size;
			}
			Point DialogUnitsToPixels(Point point) const
			{
				DoDialogUnitsToPixels(point.X(), point.Y());
				return point;
			}

			template<std::constructible_from<int, int> T>
			T DialogUnitsToPixels(int x, int y) const
			{
				DoDialogUnitsToPixels(x, y);
				return {x, y};
			}

			Size PixelsToDialogUnits(Size size) const
			{
				DoPixelsToDialogUnits(size.Width(), size.Height());
				return size;
			}
			Point PixelsToDialogUnits(Point point) const
			{
				DoPixelsToDialogUnits(point.X(), point.Y());
				return point;
			}

			template<std::constructible_from<int, int> T>
			T PixelsToDialogUnits(int x, int y) const
			{
				DoPixelsToDialogUnits(x, y);
				return {x, y};
			}
	};
}
