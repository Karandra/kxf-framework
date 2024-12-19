#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class IGraphicsRenderer;
}

namespace kxf
{
	class KXF_API IGraphicsObject: public RTTI::Interface<IGraphicsObject>
	{
		kxf_RTTI_DeclareIID(IGraphicsObject, {0x92c11e79, 0x18c8, 0x4b68, {0x83, 0xf6, 0x3b, 0x55, 0x50, 0xb5, 0x3f, 0x6e}});

		public:
			virtual ~IGraphicsObject() = default;

		public:
			virtual bool IsNull() const = 0;
			virtual bool IsSameAs(const IGraphicsObject& other) const = 0;
			virtual std::shared_ptr<IGraphicsObject> CloneGraphicsObject() const = 0;

			virtual IGraphicsRenderer& GetRenderer() = 0;
			virtual void* GetNativeHandle() const = 0;

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}

			bool operator==(const IGraphicsObject& other) const
			{
				return IsSameAs(other);
			}
			bool operator!=(const IGraphicsObject& other) const
			{
				return !IsSameAs(other);
			}
	};
}
