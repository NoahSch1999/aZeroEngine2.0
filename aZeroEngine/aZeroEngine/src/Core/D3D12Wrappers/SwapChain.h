#pragma once
#include "../../D3D12Core.h"
#include "../D3D12Wrappers/Descriptors/Descriptor.h"
#include "../Singleton/Singleton.h"
#include "Commands/CommandQueue.h"
#include "Resources/GPUResource.h"

namespace aZero
{
	namespace D3D12
	{
		// Renderer should create and own descriptors when the window is registered
		class SwapChain
		{
		private:
			Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain = nullptr; // NOTE - Change to SwapChain3
			Microsoft::WRL::ComPtr<IDXGIFactory5> m_dxgiFactory = nullptr;

			std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_backBuffers;

			ResourceRecycler* m_resourceRecycler = nullptr;

		public:
			SwapChain(HWND windowHandle,
				const CommandQueue& graphicsQueue,
				DXGI_FORMAT backBufferFormat);
				

			// NOTE - Critical to flush the GPU commands using a CPU side wait before the SwapChain gets destroyed!
			~SwapChain();

			void Present();

			void Resize(const DXM::Vector2& NewDimensions);

			ID3D12Resource* GetBackBufferResource(int index) { return m_backBuffers[index].Get(); }

			SwapChain(const SwapChain&) = delete;
			SwapChain(SwapChain&&) = delete;
			SwapChain operator=(const SwapChain&) = delete;
			SwapChain operator=(SwapChain&&) = delete;
		};
	}
}