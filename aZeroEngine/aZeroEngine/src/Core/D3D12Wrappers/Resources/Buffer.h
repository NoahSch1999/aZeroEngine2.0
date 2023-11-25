#pragma once
#include "ResourceBase.h"

namespace aZero
{
	namespace D3D12
	{
		namespace Resource
		{
			class Buffer : public ResourceBase
			{
			protected:
				void Initialize(ID3D12Device* const device, 
					const UINT numBytes,
					const DXGI_FORMAT format,
					const UINT alignment,
					const D3D12_HEAP_TYPE heapType,
					const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON)
				{
					D3D12_RESOURCE_DESC description;
					description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
					description.Height = 1;
					description.DepthOrArraySize = 1;
					description.MipLevels = 1;
					description.SampleDesc.Count = 1;
					description.SampleDesc.Quality = 0;
					description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
					description.Width = numBytes;
					description.Format = format;
					description.Alignment = alignment;

					ResourceBase::Initialize(device, description, heapType, nullptr, initialState);
				}

			public:

				Buffer() = default;

				Buffer(ID3D12Device* const device,
					const UINT numBytes,
					const DXGI_FORMAT format,
					const UINT alignment,
					const D3D12_HEAP_TYPE heapType,
					const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON)
				{
					Buffer::Initialize(device, numBytes, format, alignment, heapType, initialState);
				}

				virtual ~Buffer()
				{
					
				}
			};
		}
	}
}