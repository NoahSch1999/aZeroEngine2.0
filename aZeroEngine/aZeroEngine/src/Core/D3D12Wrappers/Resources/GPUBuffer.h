#pragma once
#include "../ResourceRecycler.h"
#include "GPUResource.h"

namespace aZero
{
	namespace D3D12
	{
		class GPUBuffer : public GPUResourceBase
		{
		private:
			int m_NumElements = -1;

		public:
			GPUBuffer() = default;

			GPUBuffer(
				int NumElements, 
				int BytesPerElement, 
				DXGI_FORMAT Format, 
				D3D12_HEAP_TYPE HeapType, 
				std::optional<ResourceRecycler*> OptResourceRecycler
			)
			{
				this->Init(NumElements, BytesPerElement, Format, HeapType, OptResourceRecycler);
			}

			void Init(
				int NumElements, 
				int BytesPerElement, 
				DXGI_FORMAT Format, 
				D3D12_HEAP_TYPE HeapType, 
				std::optional<ResourceRecycler*> OptResourceRecycler
			)
			{
				D3D12_RESOURCE_DESC ResourceDesc;
				ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
				ResourceDesc.MipLevels = 1;
				ResourceDesc.DepthOrArraySize = 1;
				ResourceDesc.Height = 1;
				ResourceDesc.Width = NumElements * BytesPerElement;
				ResourceDesc.SampleDesc.Count = 1;
				ResourceDesc.SampleDesc.Quality = 0;
				ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
				ResourceDesc.Alignment = 0;
				ResourceDesc.Format = Format;

				D3D12_HEAP_PROPERTIES HeapProperties;
				HeapProperties.Type = HeapType;

				D3D12_HEAP_FLAGS HeapFlags = D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE;

				D3D12_RESOURCE_STATES InitialResourceState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;

				GPUResourceBase::Init(ResourceDesc, HeapProperties, HeapFlags, InitialResourceState, {}, OptResourceRecycler);
			}
		};
	}
}