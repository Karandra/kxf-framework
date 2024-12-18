#pragma once
#include "../Common.h"
#include "../IGraphicsFont.h"
#include "../../Font.h"

namespace kxf::Drawing
{
	class KXF_API NullGraphicsFont final: public IGraphicsFont
	{
		public:
			NullGraphicsFont() noexcept = default;

		public:
			// IGraphicsObject
			bool IsNull() const override
			{
				return true;
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				return other.IsNull();
			}
			std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return nullptr;
			}

			IGraphicsRenderer& GetRenderer() override
			{
				return Drawing::Private::GetNullGraphicsRenderer();
			}
			void* GetNativeHandle() const
			{
				return nullptr;
			}

			// IGraphicsFont
			String GetFaceName() const override
			{
				return {};
			}
			void SetFaceName(const String& faceName) override
			{
			}

			FontFamily GetFamily() const override
			{
				return FontFamily::None;
			}
			void SetFamily(FontFamily family) override
			{
			}

			FontMetricsF GetMetrics() const override
			{
				return {};
			}

			float GetPointSize() const override
			{
				return 0;
			}
			void SetPointSize(float pointSize) override
			{
			}

			SizeF GetPixelSize() const override
			{
				return SizeF::UnspecifiedSize();
			}
			void SetPixelSize(const SizeF& pixelSize) override
			{
				NullGraphicsFont a;
				if (a)
				{

				}
			}

			Font ToFont() const override
			{
				return {};
			}
			bool FromFont(const Font& font) override
			{
				return false;
			}
	};
}
namespace kxf
{
	inline const Drawing::NullGraphicsFont NullGraphicsFont;
}
