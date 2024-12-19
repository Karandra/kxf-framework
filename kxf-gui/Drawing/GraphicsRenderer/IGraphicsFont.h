#pragma once
#include "Common.h"
#include "IGraphicsObject.h"

namespace kxf
{
	class KXF_API IGraphicsFont: public RTTI::ExtendInterface<IGraphicsFont, IGraphicsObject>
	{
		kxf_RTTI_DeclareIID(IGraphicsFont, {0x392daaa5, 0xfc8a, 0x4868, {0xab, 0xa9, 0xfe, 0x6c, 0xd9, 0xda, 0xb1, 0x17}});

		public:
			virtual ~IGraphicsFont() = default;

		public:
			virtual String GetFaceName() const = 0;
			virtual void SetFaceName(const String& faceName) = 0;

			virtual FontFamily GetFamily() const = 0;
			virtual void SetFamily(FontFamily family) = 0;

			virtual FontMetricsF GetMetrics() const = 0;

			virtual float GetPointSize() const = 0;
			virtual void SetPointSize(float pointSize) = 0;

			virtual SizeF GetPixelSize() const = 0;
			virtual void SetPixelSize(const SizeF& pixelSize) = 0;

			virtual Font ToFont() const = 0;
			virtual bool FromFont(const Font& font) = 0;
	};
}
