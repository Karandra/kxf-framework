#pragma once
#include "Common.h"

namespace kxf
{
	class IGraphicsRenderer;
}

namespace kxf
{
	class KXF_API IGraphicsRendererAwareWidget: public RTTI::Interface<IGraphicsRendererAwareWidget>
	{
		kxf_RTTI_DeclareIID(IGraphicsRendererAwareWidget, {0x3dc68a96, 0xa7fe, 0x479d, {0xb0, 0x4, 0x14, 0x37, 0xf4, 0x62, 0xd4, 0xed}});

		public:
			virtual ~IGraphicsRendererAwareWidget() = default;

		public:
			virtual std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const = 0;
			virtual void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) = 0;
	};
}
