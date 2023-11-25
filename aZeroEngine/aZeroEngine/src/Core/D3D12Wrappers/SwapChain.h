#pragma once
#include "../../D3D12Core.h"
#include "../D3D12Wrappers/Descriptors/Descriptor.h"
#include "../Singleton/Singleton.h"

namespace aZero
{
	namespace D3D12
	{
		class SwapChain
		{
		private:
			struct BackBuffers
			{
				std::vector<D3D12_RESOURCE_STATES> m_resourceStates = { };
				std::vector<Descriptor> m_descriptors = { };
				std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_resources = { };
			};

			Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain = nullptr;
			Microsoft::WRL::ComPtr<IDXGIFactory2> m_dxgiFactory = nullptr;

			DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT::DXGI_FORMAT_FORCE_UINT;

			BackBuffers m_backBuffers;

		public:
			SwapChain()
			{

			}

			~SwapChain()
			{
				for (Microsoft::WRL::ComPtr<ID3D12Resource> buffer : m_backBuffers.m_resources)
				{
					if (buffer)
					{
						Singleton::ResourceRecycler->Get().RecycleResource(buffer);
					}
				}
			}

			SwapChain(const SwapChain&) = delete;
			SwapChain(SwapChain&&) = delete;
			SwapChain operator=(const SwapChain&) = delete;
			SwapChain operator=(SwapChain&&) = delete;
		};
	}
}