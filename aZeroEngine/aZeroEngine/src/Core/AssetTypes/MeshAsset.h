#pragma once
#include "../D3D12Wrappers/Resources/GPUBuffer.h"

namespace aZero
{
	namespace Asset
	{
		class Mesh
		{
		private:
			D3D12::Resource::GPUBuffer m_vertexBuffer;
			D3D12_VERTEX_BUFFER_VIEW m_vbView;
			D3D12_GPU_VIRTUAL_ADDRESS m_virtualAddress;
			int m_numVertices = -1;

		public:
			Mesh() = default;

			// TODO - Adapt to loading multiple submeshes which are then added to the same buffer with offsets etc...
			Mesh(ID3D12Device* const device, int numVertices, int vertexSize, D3D12::ResourceRecycler& resourceRecycler)
				:m_numVertices(numVertices)
			{
				m_vertexBuffer = std::move(D3D12::Resource::GPUBuffer(resourceRecycler, device, numVertices * vertexSize, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0));
			}

			void Upload(ID3D12GraphicsCommandList* const cmdList, const void* const data, int numVertices, int vertexSize, int stride, D3D12::ResourceRecycler& resourceRecycler)
			{
				if (m_vertexBuffer.GetResource())
				{
					int numBytes = numVertices * vertexSize;
					ID3D12Device* device = nullptr;
					m_vertexBuffer.GetResource()->GetDevice(IID_PPV_ARGS(&device));
					D3D12::Resource::UploadBuffer UploadResource = std::move(D3D12::Resource::UploadBuffer(resourceRecycler, device, numBytes, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0));
					
					D3D12::Resource::UpdateBuffer(UploadResource, 0, data, numBytes);
					D3D12::Resource::UpdateBuffer(m_vertexBuffer, 0, UploadResource, 0, numBytes, cmdList);

					m_virtualAddress = m_vertexBuffer.GetResource()->GetGPUVirtualAddress();
					m_vbView.BufferLocation = m_virtualAddress;
					m_vbView.SizeInBytes = numBytes;
					m_vbView.StrideInBytes = stride;
				}
			}

			int GetNumVertices() const { return m_numVertices; }
			
			const D3D12_VERTEX_BUFFER_VIEW* const GetVertexBufferView() const { return &m_vbView; }
		};
	}
}