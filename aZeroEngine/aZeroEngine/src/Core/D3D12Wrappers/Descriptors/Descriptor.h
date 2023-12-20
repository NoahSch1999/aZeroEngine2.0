#pragma once
#include "../../D3D12Core.h"

namespace aZero
{
	namespace D3D12
	{
		class Descriptor
		{
			friend class DescriptorHeap;
		private:
			D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle = { 0 };
			D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle = { 0 };
			int m_heapIndex = -1;

			Descriptor(const D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
				const D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle,
				const int heapIndex)
				:m_cpuHandle(cpuHandle), m_gpuHandle(gpuHandle), m_heapIndex(heapIndex)
			{

			}

		public:
			Descriptor() = default;

			D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return m_cpuHandle; }
			D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return m_gpuHandle; }
			int GetHeapIndex() const { return m_heapIndex; }
		};
	}
}