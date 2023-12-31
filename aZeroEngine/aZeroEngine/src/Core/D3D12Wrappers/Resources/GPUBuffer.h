#pragma once
#include "Buffer.h"

namespace aZero
{
	namespace D3D12
	{
		namespace Resource
		{
			class GPUBuffer : public Buffer
			{
			public:
				GPUBuffer() = default;

				GPUBuffer(ResourceRecycler& resourceRecycler, ID3D12Device* const device,
					const UINT numBytes,
					const DXGI_FORMAT format,
					const UINT alignment,
					const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON)
				{
					GPUBuffer::Initialize(resourceRecycler, device, numBytes, format, alignment, initialState);
				}

				void Initialize(ResourceRecycler& resourceRecycler, ID3D12Device* const device,
					const UINT numBytes,
					const DXGI_FORMAT format,
					const UINT alignment,
					const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON)
				{
					if (!m_resource)
					{
						Buffer::Initialize(resourceRecycler, device, numBytes, format, alignment, D3D12_HEAP_TYPE_DEFAULT, initialState);
					}
				}

				GPUBuffer(GPUBuffer&& other) noexcept
					:Buffer(std::move(other))
				{

				}

				GPUBuffer& operator=(GPUBuffer&& other) noexcept
				{
					Buffer::operator=(std::move(other));
					return *this;
				}

				virtual ~GPUBuffer()
				{

				}
			};
		}
	}
}