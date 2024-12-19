#pragma once
#include "../Common.h"
#include "../IGraphicsBrush.h"

namespace kxf::Drawing
{
	class KXF_API NullGraphicsBrush final: public IGraphicsBrush
	{
		public:
			NullGraphicsBrush() noexcept = default;

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

			// IGraphicsBrush
			bool IsTransparent() const override
			{
				return true;
			}

			Color GetColor() const override
			{
				return {};
			}
			void SetColor(const Color& color) override
			{
			}
	};
}
namespace kxf
{
	inline const Drawing::NullGraphicsBrush NullGraphicsBrush;
}
