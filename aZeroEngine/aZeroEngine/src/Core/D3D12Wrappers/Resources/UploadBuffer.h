#pragma once
#include "Buffer.h"

namespace aZero
{
	namespace D3D12
	{
		namespace Resource
		{
			class UploadBuffer : public Buffer
			{
			private:
				void* m_mappedPtr = nullptr;

			public:

				UploadBuffer() = default;

				UploadBuffer(ResourceRecycler& resourceRecycler, ID3D12Device* const device,
					const UINT numBytes,
					const DXGI_FORMAT format,
					const UINT alignment,
					const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON)
				{
					UploadBuffer::Initialize(resourceRecycler, device, numBytes, format, alignment, initialState);
				}

				void Initialize(ResourceRecycler& resourceRecycler, ID3D12Device* const device,
					const UINT numBytes,
					const DXGI_FORMAT format,
					const UINT alignment,
					const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON)
				{
					if (!m_resource)
					{
						Buffer::Initialize(resourceRecycler, device, numBytes, format, alignment, D3D12_HEAP_TYPE_UPLOAD, initialState);
						m_resource->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedPtr));
					}
				}

				UploadBuffer(UploadBuffer&& other) noexcept
					:Buffer(std::move(other))
				{
					m_mappedPtr = other.m_mappedPtr;
				}

				UploadBuffer& operator=(UploadBuffer&& other) noexcept
				{
					Buffer::operator=(std::move(other));
					return *this;
				}

				virtual ~UploadBuffer()
				{

				}

				void* const GetMappedPtr() const { return m_mappedPtr; }

			};
		}
	}
}