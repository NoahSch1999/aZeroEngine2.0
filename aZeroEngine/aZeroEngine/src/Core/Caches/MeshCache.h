#pragma once
#include "AssetCacheBase.h"
#include "../AssetTypes/MeshAsset.h"

namespace aZero
{
	class MeshCache : public AssetCacheBase<std::string, Asset::Mesh>
	{
	private:
		D3D12::ResourceRecycler& m_ResourceRecycler;

	public:
		MeshCache(D3D12::ResourceRecycler& ResourceRecycler)
			:AssetCacheBase(10), m_ResourceRecycler(ResourceRecycler)
		{

		}

		void LoadFromFile(const std::string& FilePath, ID3D12GraphicsCommandList* CmdList)
		{
			if (FilePath.ends_with(".azMesh")) // Custom mesh file format
			{

			}
			else if (FilePath.ends_with(".fbx"))
			{
				Asset::LoadedFBXFileInfo LoadedFBX;
				const bool IsLoaded = Asset::LoadFBXFile(LoadedFBX, FilePath);
				if (IsLoaded)
				{
					ID3D12Device* Device = nullptr;
					CmdList->GetDevice(IID_PPV_ARGS(&Device));
					for (int MeshIndex = 0; MeshIndex < LoadedFBX.Meshes.size(); MeshIndex++)
					{
						const Asset::LoadedFBXFileInfo::MeshInfo& Mesh = LoadedFBX.Meshes[MeshIndex];
						Store(Mesh.Name, Asset::Mesh(Device, CmdList, Mesh, m_ResourceRecycler));
					}
				}
			}
		}

		// TODO - Implement move and copy constructors / operators
		MeshCache(const MeshCache&) = delete;
		MeshCache(MeshCache&&) = delete;
		MeshCache operator=(const MeshCache&) = delete;
		MeshCache operator=(MeshCache&&) = delete;
	};
}