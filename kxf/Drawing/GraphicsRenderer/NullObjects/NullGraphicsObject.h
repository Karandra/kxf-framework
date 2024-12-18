#pragma once
#include "../Common.h"
#include "../IGraphicsObject.h"

namespace kxf::Drawing
{
	class KXF_API NullGraphicsObject final: public IGraphicsObject
	{
		public:
			NullGraphicsObject() noexcept = default;

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
			void* GetNativeHandle() const override
			{
				return nullptr;
			}
	};
}
namespace kxf
{
	inline const Drawing::NullGraphicsObject NullGraphicsObject;
}
