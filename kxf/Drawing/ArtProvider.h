#pragma once
#include "Common.h"
#include "IArtProvider.h"
#include "kxf/Core/StdIcon.h"

namespace kxf
{
	class BitmapImage;
	class ImageBundle;
}

namespace kxf::ArtProviderClient
{
	extern const String ToolBar;
	extern const String Menu;
	extern const String List;
	extern const String Button;
	extern const String FrameIcon;
	extern const String HelpBrowser;
	extern const String CommonDialog;
	extern const String MessageBox;
}

namespace kxf::ArtProvider
{
	KXF_API void PushProvider(std::shared_ptr<IArtProvider> artProvider);
	KXF_API void RemoveProvider(const IArtProvider& artProvider);
}

namespace kxf::ArtProvider
{
	KXF_API BitmapImage GetResource(const ResourceID& id, const String& clientID = {}, const Size& size = Size::UnspecifiedSize());
	KXF_API ImageBundle GetResourceBundle(const ResourceID& id, const String& clientID = {});

	KXF_API ResourceID GetMessageBoxResourceIDs(StdIcon iconID);
	KXF_API BitmapImage GetMessageBoxResource(StdIcon iconID);
}
