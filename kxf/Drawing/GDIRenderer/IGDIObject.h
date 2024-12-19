#pragma once
#include "Common.h"
#include "../Geometry.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class KXF_API IGDIObject: public RTTI::Interface<IGDIObject>
	{
		kxf_RTTI_DeclareIID(IGDIObject, {0x773303ba, 0xe1a6, 0x47af, {0xa5, 0xa9, 0x19, 0x3, 0xf7, 0xcd, 0x8f, 0xb6}});

		public:
			virtual ~IGDIObject() = default;

		public:
			virtual bool IsNull() const = 0;
			virtual bool IsSameAs(const IGDIObject& other) const = 0;
			virtual std::shared_ptr<IGDIObject> CloneGDIObject() const = 0;

			virtual void* GetHandle() const = 0;
			virtual void* DetachHandle() = 0;
			virtual void AttachHandle(void* handle) = 0;

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}

			bool operator==(const IGDIObject& other) const
			{
				return IsSameAs(other);
			}
			bool operator!=(const IGDIObject& other) const
			{
				return !IsSameAs(other);
			}
	};
}
