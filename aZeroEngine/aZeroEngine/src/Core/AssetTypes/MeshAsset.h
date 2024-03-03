#pragma once
#include "../D3D12Wrappers/Resources/GPUResource.h"

namespace aZero
{
	namespace Asset
	{
		struct MeshVertex
		{
			DXM::Vector3 position;
			DXM::Vector2 uv;
			DXM::Vector3 normal;
			DXM::Vector3 tangent;
		};

		struct LoadedFBXFileInfo
		{
			struct MeshInfo
			{
				std::string Name = "";
				std::vector<MeshVertex> Vertices;
				std::vector<int> Indices;
				double BoundingSphereRadius = 0.0;
			};

			std::vector<MeshInfo> Meshes;
		};

		bool LoadFBXFile(LoadedFBXFileInfo& OutInfo, const std::string& filePath);

		class Mesh
		{
		private:
			std::string m_Name = "";

			D3D12::GPUResource m_VertexBuffer;
			D3D12::GPUResource m_IndexBuffer;

			D3D12_VERTEX_BUFFER_VIEW m_VbView;
			D3D12_INDEX_BUFFER_VIEW m_IbView;

			UINT m_NumVertices = 0;
			UINT m_NumIndices = 0;
			UINT m_VertexSize = 0;

			float m_BoundingSphereRadius = 0.0;

		private:
			void CreateBuffers(ID3D12Device* const device, const std::string& name, UINT numVertices, int vertexSize, UINT numIndices, D3D12::ResourceRecycler& resourceRecycler)
			{
				// NOTE - Need to account for if the input mesh has more indices than vertices (wants to reuse a vertex). 
				//		But this should be handled during loading since MeshAsset is only the container
				D3D12::BufferResourceDesc BufferDescriptionVB;
				BufferDescriptionVB.AccessType = D3D12::RESOURCE_ACCESS_TYPE::GPU_ONLY;
				BufferDescriptionVB.NumBytes = numVertices * vertexSize;
				;
				m_VertexBuffer = std::move(D3D12::GPUResource(device, resourceRecycler, BufferDescriptionVB));
				
				D3D12::BufferResourceDesc BufferDescriptionIB;
				BufferDescriptionIB.AccessType = D3D12::RESOURCE_ACCESS_TYPE::GPU_ONLY;
				BufferDescriptionIB.NumBytes = numVertices * sizeof(UINT);
				m_IndexBuffer = std::move(D3D12::GPUResource(device, resourceRecycler, BufferDescriptionIB));
#ifdef _DEBUG
				std::wstring debugName(name.begin(), name.end());
				m_VertexBuffer.GetResource()->SetName((debugName + L"_VB").c_str());
				m_IndexBuffer.GetResource()->SetName((debugName + L"_IB").c_str());
#endif
			}

		public:
			Mesh() = default;

			Mesh(ID3D12Device* const device, const std::string& name, UINT numVertices, int vertexSize, UINT numIndices, float BoundingSphereRadius, D3D12::ResourceRecycler& resourceRecycler)
				:m_Name(name), m_NumIndices(numIndices), m_NumVertices(numVertices), m_BoundingSphereRadius(BoundingSphereRadius), m_VertexSize(vertexSize)
			{
				CreateBuffers(device, name, numVertices, vertexSize, numIndices, resourceRecycler);
			}

			Mesh(ID3D12Device* const device, ID3D12GraphicsCommandList* const cmdList, const Asset::LoadedFBXFileInfo::MeshInfo& LoadedMeshData, D3D12::ResourceRecycler& resourceRecycler)
				:m_Name(LoadedMeshData.Name), m_NumIndices(LoadedMeshData.Indices.size()), m_NumVertices(LoadedMeshData.Vertices.size()), m_BoundingSphereRadius(LoadedMeshData.BoundingSphereRadius), m_VertexSize(sizeof(decltype(LoadedMeshData.Vertices[0])))
			{
				CreateBuffers(device, LoadedMeshData.Name, LoadedMeshData.Vertices.size(), m_VertexSize, LoadedMeshData.Indices.size(), resourceRecycler);

				Upload(cmdList, LoadedMeshData.Vertices.data(), LoadedMeshData.Indices.data(), resourceRecycler);
			}

			// TODO - Rework to allow subportion of intermediate resource that a mesh cache etc can use when it have allocated a 
			//		large buffer for multiple mesh assets and then mass-update them using the same intermediate buffer
			//			This allocation scheme would remove the need of extra resource allocations for each mesh asset PLUS the copy from intermediate to gpu-only
			//				since the entire (or atleast the range) of the intermediate can be copied to the gpu-only resource once all the data
			//					has been copied to the sysram (upload) buffer.
			void Upload(ID3D12GraphicsCommandList* const CmdList, 
				const void* VertexData,
				const void* IndexData,
				D3D12::ResourceRecycler& ResourceRecycler)
			{
				DEBUG_CHECK(m_VertexBuffer.GetResource());
				DEBUG_CHECK(CmdList);

				const UINT numBytesVB = m_NumVertices * m_VertexSize;
				const UINT numBytesIB = m_NumIndices * sizeof(UINT);
				const UINT numBytes = numBytesVB + numBytesIB;

				ID3D12Device* device = nullptr;
				m_VertexBuffer.GetResource()->GetDevice(IID_PPV_ARGS(&device));

				D3D12::BufferResourceDesc UploadBufferDesc;
				UploadBufferDesc.AccessType = D3D12::RESOURCE_ACCESS_TYPE::CPU_WRITE;
				UploadBufferDesc.NumBytes = numBytes;
				D3D12::GPUResource UploadResource= std::move(D3D12::GPUResource(device, ResourceRecycler, UploadBufferDesc));

				D3D12::GPUResource::CopyBufferToBuffer(CmdList, m_VertexBuffer, 0, UploadResource, 0, numBytesVB, VertexData);
				D3D12::GPUResource::CopyBufferToBuffer(CmdList, m_IndexBuffer, 0, UploadResource, numBytesVB, numBytesIB, IndexData);

				m_VbView.BufferLocation = m_VertexBuffer.GetResource()->GetGPUVirtualAddress();
				m_VbView.SizeInBytes = numBytesVB;
				m_VbView.StrideInBytes = m_VertexSize;
				
				m_IbView.BufferLocation = m_IndexBuffer.GetResource()->GetGPUVirtualAddress();
				m_IbView.SizeInBytes = numBytesIB;
				m_IbView.Format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
			}

			const std::string& GetName() const { return m_Name; }

			int GetNumIndices() const { return m_NumIndices; }
			
			const D3D12_VERTEX_BUFFER_VIEW& const GetVertexBufferView() const { return m_VbView; }
			
			const D3D12_INDEX_BUFFER_VIEW& const GetIndexBufferView() const { return m_IbView; }
		};
	}
}