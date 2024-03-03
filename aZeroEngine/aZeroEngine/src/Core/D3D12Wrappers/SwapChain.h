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
		class SwapChain
		{
		public:
			struct BackBuffer
			{
				Descriptor m_descriptor;
				Microsoft::WRL::ComPtr<ID3D12Resource> m_resource = nullptr;
			};

		private:
			Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain = nullptr; // NOTE - Change to SwapChain3
			Microsoft::WRL::ComPtr<IDXGIFactory5> m_dxgiFactory = nullptr;

			DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT::DXGI_FORMAT_FORCE_UINT;

			std::vector<BackBuffer> m_backBuffers;

			ResourceRecycler* m_resourceRecycler = nullptr;
			DescriptorManager* m_descriptorManager = nullptr;

		public:
			SwapChain(HWND windowHandle,
				const CommandQueue& graphicsQueue,
				DescriptorManager& descriptorManager,
				DXGI_FORMAT backBufferFormat);
				

			// NOTE - Critical to flush the GPU commands using a CPU side wait before the SwapChain gets destroyed!
			~SwapChain();

			void ResolveRenderSurface(ID3D12GraphicsCommandList* cmdList, int backBufferIndex, D3D12::GPUResource& renderSource);

			void Present();

			SwapChain(const SwapChain&) = delete;
			SwapChain(SwapChain&&) = delete;
			SwapChain operator=(const SwapChain&) = delete;
			SwapChain operator=(SwapChain&&) = delete;
		};
	}
}