#pragma once
#include "DescriptorHeap.h"

namespace aZero
{
	namespace D3D12
	{
		class DescriptorManager
		{
		private:
			DescriptorHeap m_resourceHeap;
			DescriptorHeap m_samplerHeap;
			DescriptorHeap m_rtvHeap;
			DescriptorHeap m_dsvHeap;

		public:
			DescriptorManager() = default;

			DescriptorManager(ID3D12Device* const device)
			{
				Initialize(device);
			}

			~DescriptorManager()
			{

			}

			// TODO - Add heap expansion and remove hardcodeded values
			void Initialize(ID3D12Device* const device)
			{
				if (!m_resourceHeap.GetDescriptorHeap())
				{
					m_resourceHeap.Initialize(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 10000, true);
					m_samplerHeap.Initialize(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 10, true);
					m_rtvHeap.Initialize(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 100, false);
					m_dsvHeap.Initialize(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 100, false);
				}
			}

			// TODO - Define copy stuff etc...
			DescriptorManager(const DescriptorManager&) = delete;
			DescriptorManager(DescriptorManager&&) = delete;
			DescriptorManager operator=(const DescriptorManager&) = delete;
			DescriptorManager operator=(DescriptorManager&&) = delete;

			DescriptorHeap& GetResourceHeap() { return m_resourceHeap; }
			DescriptorHeap& GetSamplerHeap() { return m_samplerHeap; }
			DescriptorHeap& GetRtvHeap() { return m_rtvHeap; }
			DescriptorHeap& GetDsvHeap() { return m_dsvHeap; }
		};
	}
}