#include "kxf-pch.h"
#include "GraphicsRenderer.h"
#include "GDIRenderer/GDIGraphicsRenderer.h"
#include "WxRenderer/WxGraphicsRenderer.h"

namespace
{
	std::shared_ptr<kxf::IGraphicsRenderer> g_DefaultRenderer;

	std::shared_ptr<kxf::IGraphicsRenderer> GetWxRenderer(wxGraphicsRenderer* renderer)
	{
		if (renderer)
		{
			return std::make_shared<kxf::WxGraphicsRenderer>(*renderer);
		}
		return nullptr;
	}
}

namespace kxf::Drawing
{
	std::shared_ptr<IGraphicsRenderer> GetGDIRenderer()
	{
		static auto renderer = std::make_shared<GDIGraphicsRenderer>();
		return renderer;
	}
	std::shared_ptr<IGraphicsRenderer> GetGDIPlusRenderer()
	{
		static auto renderer = GetWxRenderer(wxGraphicsRenderer::GetGDIPlusRenderer());
		return renderer;
	}
	std::shared_ptr<IGraphicsRenderer> GetDirect2DRenderer()
	{
		static auto renderer = GetWxRenderer(wxGraphicsRenderer::GetDirect2DRenderer());
		return renderer;
	}
	std::shared_ptr<IGraphicsRenderer> GetCairoRenderer()
	{
		static auto renderer = GetWxRenderer(wxGraphicsRenderer::GetCairoRenderer());
		return renderer;
	}

	std::shared_ptr<IGraphicsRenderer> GetDefaultRenderer()
	{
		auto renderer = g_DefaultRenderer;
		if (!renderer)
		{
			renderer = GetDirect2DRenderer();
		}
		if (!renderer)
		{
			renderer = GetGDIPlusRenderer();
		}
		if (!renderer)
		{
			renderer = GetCairoRenderer();
		}
		if (!renderer)
		{
			renderer = GetGDIRenderer();
		}
		return renderer;
	}
	void SetDefaultRenderer(std::shared_ptr<IGraphicsRenderer> renderer)
	{
		g_DefaultRenderer = std::move(renderer);
	}

	size_t EnumAvailableRenderers(std::function<bool(std::shared_ptr<IGraphicsRenderer>)> func)
	{
		size_t count = 0;
		auto Try = [&](std::shared_ptr<IGraphicsRenderer> renderer)
		{
			if (renderer)
			{
				count++;
				if (func)
				{
					return std::invoke(func, std::move(renderer));
				}
			}
			return true;
		};

		Try(GetGDIRenderer()) && Try(GetGDIPlusRenderer()) && Try(GetDirect2DRenderer()) && Try(GetCairoRenderer());
		return count;
	}
	std::shared_ptr<IGraphicsRenderer> GetRendererByName(const String& name)
	{
		std::shared_ptr<IGraphicsRenderer> result;
		EnumAvailableRenderers([&](std::shared_ptr<IGraphicsRenderer> renderer)
		{
			if (renderer->GetName() == name)
			{
				result = std::move(renderer);
				return false;
			}
			return true;
		});

		return result;
	}
}
