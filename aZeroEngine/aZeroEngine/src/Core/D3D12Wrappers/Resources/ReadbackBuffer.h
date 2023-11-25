#pragma once
#include "Buffer.h"

namespace aZero
{
	namespace D3D12
	{
		namespace Resource
		{
			class ReadbackBuffer : public Buffer
			{
			private:
				void* m_mappedPtr = nullptr;

			public:
				ReadbackBuffer() = default;

				ReadbackBuffer(ID3D12Device* const device,
					const UINT numBytes,
					const DXGI_FORMAT format,
					const UINT alignment,
					const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON)
				{
					ReadbackBuffer::Initialize(device, numBytes, format, alignment, initialState);
				}

				void Initialize(ID3D12Device* const device,
					const UINT numBytes,
					const DXGI_FORMAT format,
					const UINT alignment,
					const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON)
				{
					if (!m_resource)
					{
						Buffer::Initialize(device, numBytes, format, alignment, D3D12_HEAP_TYPE_UPLOAD, initialState);
						m_resource->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedPtr));
					}
				}

				virtual ~ReadbackBuffer()
				{
					if (!m_resource)
					{
						m_resource->Unmap(0, nullptr);
					}
				}

				void* const GetMappedPtr() const { return m_mappedPtr; }
			};
		}
	}
}