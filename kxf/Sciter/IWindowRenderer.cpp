#include "kxf-pch.h"
#include "IWindowRenderer.h"
#include "Renderers/DirectX.h"

namespace kxf::Sciter
{
	std::unique_ptr<IWindowRenderer> IWindowRenderer::CreateInstance(WindowRenderer type, Host& host)
	{
		switch (type)
		{
			case WindowRenderer::DirectX:
			{
				return std::make_unique<DirectX>(host);
			}
		};
		return nullptr;
	}
}
