#pragma once
#include "Common.h"
#include "IWidget.h"

namespace kxf
{
	class KXF_API IImageViewWidget: public RTTI::ExtendInterface<IImageViewWidget, IWidget>
	{
		kxf_RTTI_DeclareIID(IImageViewWidget, {0x648cbcaa, 0xf464, 0x4156, {0x92, 0x7d, 0xa2, 0xb4, 0x46, 0xcf, 0xc5, 0x26}});

		public:
			enum class BackgroundMode
			{
				Solid,
				Gradient,
				Checker
			};
			enum class ScaleMode
			{
				None,
				Fill,
				AspectFit,
				AspectFill,
			};

		public:
			virtual BackgroundMode GetBackgroundMode() = 0;
			virtual void SetBackgroundMode(BackgroundMode mode) = 0;

			virtual Direction GetGradientDirection() const = 0;
			virtual void SetGradientDirection(Direction direction) = 0;

			virtual ScaleMode GetScaleMode() const = 0;
			virtual void SetScaleMode(ScaleMode mode) = 0;

			virtual float GetScaleFactor() const = 0;
			virtual void SetScaleFactor(float factor) = 0;

			virtual void SetImage(const IImage2D& image) = 0;
	};
}
