#pragma once
#include "GraphicsRenderer/Common.h"
#include "GraphicsRenderer/IGraphicsObject.h"
#include "GraphicsRenderer/IGraphicsRenderer.h"
#include "GraphicsRenderer/IGraphicsContext.h"
#include "GraphicsRenderer/IGraphicsTexture.h"
#include "GraphicsRenderer/IGraphicsPath.h"
#include "GraphicsRenderer/IGraphicsFont.h"
#include "GraphicsRenderer/IGraphicsBrush.h"
#include "GraphicsRenderer/IGraphicsPen.h"
#include "GraphicsRenderer/GraphicsAction.h"

#include "kxf/Core/CallbackFunction.h"

namespace kxf::Drawing
{
	KXF_API std::shared_ptr<IGraphicsRenderer> GetGDIRenderer();
	KXF_API std::shared_ptr<IGraphicsRenderer> GetGDIPlusRenderer();
	KXF_API std::shared_ptr<IGraphicsRenderer> GetDirect2DRenderer();
	KXF_API std::shared_ptr<IGraphicsRenderer> GetCairoRenderer();

	KXF_API std::shared_ptr<IGraphicsRenderer> GetDefaultRenderer();
	KXF_API void SetDefaultRenderer(std::shared_ptr<IGraphicsRenderer> renderer);

	KXF_API CallbackResult<void> EnumAvailableRenderers(CallbackFunction<std::shared_ptr<IGraphicsRenderer>> func);
	KXF_API std::shared_ptr<IGraphicsRenderer> GetRendererByName(const String& name);
}
