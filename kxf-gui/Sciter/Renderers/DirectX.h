#pragma once
#include "kxf/System/COM.h"
#include "kxf/Win32/Include-DirectX.h"
#include "../IWindowRenderer.h"
#include "FPSCounter.h"

namespace kxf::Sciter
{
	class KXF_API DirectX: public IWindowRenderer
	{
		private:
			Host& m_SciterHost;
			wxWindow& m_SciterWindow;
			FPSCounter m_FrameCounter;

			D3D_DRIVER_TYPE m_DriverType = D3D_DRIVER_TYPE_NULL;
			D3D_FEATURE_LEVEL m_FeatureLevel = D3D_FEATURE_LEVEL_1_0_CORE;

			COMPtr<IDXGIFactory2> m_DXGIFactory;
			COMPtr<IDXGIDevice> m_DXGIDevice;
			COMPtr<IDXGISwapChain1> m_SwapChain;
			COMPtr<ID3D11Device1> m_D3DDevice;
			COMPtr<ID3D11DeviceContext1> m_DeviceContext;

			COMPtr<IDCompositionDevice> m_CompositionDevice;
			COMPtr<IDCompositionTarget> m_CompositionTarget;
			COMPtr<IDCompositionVisual> m_CompositionVisual;

		public:
			DirectX(Host& host);
			~DirectX();

		public:
			bool Create() override;
			void Render() override;
			void OnSize() override;
			void OnIdle() override;

			double GetFPS() const override
			{
				return m_FrameCounter.GetCount();
			}
	};
}