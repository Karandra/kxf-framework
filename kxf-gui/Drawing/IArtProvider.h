#pragma once
#include "Common.h"
#include "kxf/Core/ResourceID.h"
#include "kxf/Drawing/Geometry.h"

namespace kxf
{
	class BitmapImage;
	class ImageBundle;
}

namespace kxf
{
	class KXF_API IArtProvider: public RTTI::Interface<IArtProvider>
	{
		kxf_RTTI_DeclareIID(IArtProvider, {0xe4c441d, 0x68f, 0x4a15, {0xb0, 0x1e, 0xab, 0x30, 0x94, 0x19, 0xcf, 0x57}});

		public:
			virtual ~IArtProvider() = default;

		public:
			virtual BitmapImage GetImage(const ResourceID& id, const String& clientID = {}, const Size& size = Size::UnspecifiedSize()) = 0;
			virtual ImageBundle GetImageBundle(const ResourceID& id, const String& clientID = {}) = 0;
	};
}
