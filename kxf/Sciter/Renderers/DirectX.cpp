#include "kxf-pch.h"
#include "DirectX.h"
#include "kxf/Sciter/SciterAPI.h"
#include "kxf/Sciter/Host.h"
#include "kxf/Core/ErrorCode.h"
#include "kxf/System/NativeAPI.h"
#include <thread>
#include <chrono>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dcomp.lib")

namespace
{
	constexpr size_t g_SwapChainBufferCount = 2;
	constexpr size_t g_SwapChainSampleCount = 1;
	constexpr size_t g_SwapChainSampleQuality = 0;

	constexpr D3D_FEATURE_LEVEL g_FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	constexpr D3D_DRIVER_TYPE g_DriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP
	};

	// RGBA
	constexpr float g_ClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
}

namespace kxf::Sciter
{
	DirectX::DirectX(Host& host)
		:m_SciterHost(host), m_SciterWindow(host.GetWindow())
	{
	}
	DirectX::~DirectX()
	{
		if (m_DeviceContext)
		{
			m_DeviceContext->ClearState();
		}
	}

	bool DirectX::Create()
	{
		// If we don't have 'DCompositionCreateDevice' we can't create the renderer properly
		if (!NativeAPI::DComp::DCompositionCreateDevice)
		{
			return false;
		}

		const Size windowSize = Size(m_SciterWindow.GetClientSize());

		// D3D10_CREATE_DEVICE_BGRA_SUPPORT is required here
		constexpr uint32_t createDeviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT;

		// Create DXGI factory
		HResult hr = HResult::Fail();
		if (NativeAPI::DXGI::CreateDXGIFactory2)
		{
			hr = NativeAPI::DXGI::CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(IDXGIFactory2), m_DXGIFactory.GetAddress());
		}
		if (!hr)
		{
			return false;
		}

		// Create D3D device
		HResult result = HResult::Fail();
		for (D3D_DRIVER_TYPE driverType: g_DriverTypes)
		{
			COMPtr<ID3D11Device> d3dDevice;
			COMPtr<ID3D11DeviceContext> deviceContext;

			m_DriverType = driverType;
			result = D3D11CreateDevice(nullptr,
									   driverType,
									   nullptr,
									   createDeviceFlags,
									   g_FeatureLevels,
									   std::size(g_FeatureLevels),
									   D3D11_SDK_VERSION,
									   &d3dDevice,
									   &m_FeatureLevel,
									   &deviceContext);
			if (result)
			{
				d3dDevice->QueryInterface(&m_D3DDevice);
				deviceContext->QueryInterface(&m_DeviceContext);

				break;
			}
		}
		if (!result)
		{
			return false;
		}

		// Query DXGI device
		hr = m_D3DDevice->QueryInterface(&m_DXGIDevice);
		if (!hr)
		{
			return false;
		}

		// Create swap chain
		DXGI_SWAP_CHAIN_DESC1 description = {};
		description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		description.BufferCount = g_SwapChainBufferCount;
		description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
		description.Scaling = DXGI_SCALING_STRETCH;
		description.Stereo = FALSE;
		description.Flags = 0;
		description.Width = windowSize.GetWidth();
		description.Height = windowSize.GetHeight();
		description.SampleDesc.Count = g_SwapChainSampleCount;
		description.SampleDesc.Quality = g_SwapChainSampleQuality;

		hr = m_DXGIFactory->CreateSwapChainForComposition(m_DXGIDevice, &description, nullptr, &m_SwapChain);
		if (!hr)
		{
			return false;
		}
		GetSciterAPI()->SciterCreateOnDirectXWindow(m_SciterWindow.GetHandle(), m_SwapChain);

		// Create composition device
		hr = NativeAPI::DComp::DCompositionCreateDevice(m_DXGIDevice, __uuidof(IDCompositionDevice), m_CompositionDevice.GetAddress());
		if (!hr)
		{
			return false;
		}

		// Create render target
		hr = m_CompositionDevice->CreateTargetForHwnd(m_SciterWindow.GetHandle(), true, &m_CompositionTarget);
		if (!hr)
		{
			return false;
		}

		// Create composition visual
		hr = m_CompositionDevice->CreateVisual(&m_CompositionVisual);
		if (!hr)
		{
			return false;
		}

		// Set visual to use swap chain
		hr = m_CompositionVisual->SetContent(m_SwapChain);
		if (!hr)
		{
			return false;
		}

		// Set root of the composition target
		hr = m_CompositionTarget->SetRoot(m_CompositionVisual);
		if (!hr)
		{
			return false;
		}

		// We're done here
		hr = m_CompositionDevice->Commit();
		if (!hr)
		{
			return false;
		}
		return true;
	}
	void DirectX::Render()
	{
		if (m_SwapChain && !m_SciterWindow.IsFrozen())
		{
			auto fpsWatcher = m_FrameCounter.CreateWatcher();

			// Create a single-threaded Direct2D factory with debugging information
			COMPtr<ID2D1Factory2> d2Factory;
			D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, {D2D1_DEBUG_LEVEL_INFORMATION}, &d2Factory);

			// Create the Direct2D device that links back to the Direct3D device
			COMPtr<ID2D1Device1> d2Device;
			d2Factory->CreateDevice(m_DXGIDevice, &d2Device);

			// Retrieve the swap chain's back buffer as surface for Sciter and as texture for render target
			COMPtr<IDXGISurface2> surface;
			m_SwapChain->GetBuffer(0, __uuidof(IDXGISurface2), reinterpret_cast<void**>(&surface));
			
			COMPtr<ID3D11Texture2D> texture2D;
			m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&texture2D));

			// Create render target and set it as active
			COMPtr<ID3D11RenderTargetView> renderTarget;
			m_D3DDevice->CreateRenderTargetView(texture2D, nullptr, &renderTarget);
			m_DeviceContext->OMSetRenderTargets(1, &renderTarget, nullptr);

			// Clear the back buffer and render the document
			m_DeviceContext->ClearRenderTargetView(renderTarget, g_ClearColor);
			GetSciterAPI()->SciterRenderOnDirectXTexture(m_SciterWindow.GetHandle(), nullptr, surface);

			// Present our back buffer to our front buffer
			m_SwapChain->Present(0, 0);
		}
	}
	void DirectX::OnSize()
	{
		if (m_SwapChain)
		{
			// Preserve the existing buffer count and format.
			const Size size = Size(m_SciterWindow.GetClientSize());
			m_SwapChain->ResizeBuffers(0, size.GetWidth(), size.GetHeight(), DXGI_FORMAT_UNKNOWN, 0);

			// Get buffer and create a render-target-view
			COMPtr<ID3D11Texture2D> texture2D;
			m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&texture2D));

			// Make it active
			COMPtr<ID3D11RenderTargetView> renderTarget;
			m_D3DDevice->CreateRenderTargetView(texture2D, nullptr, &renderTarget);
			m_DeviceContext->OMSetRenderTargets(1, &renderTarget, nullptr);

			// Resize the viewport
			D3D11_VIEWPORT viewport;
			viewport.Width = size.GetWidth();
			viewport.Height = size.GetHeight();
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			m_DeviceContext->RSSetViewports(1, &viewport);
		}
	}
	void DirectX::OnIdle()
	{
		Render();
	}
}
