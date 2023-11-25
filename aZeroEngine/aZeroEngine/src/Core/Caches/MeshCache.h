#pragma once
#include "AssetCacheBase.h"
#include "../AssetTypes/MeshAsset.h"

#define MESH_ASSET_DIRECTORY ""

namespace aZero
{
	class MeshCache : public AssetCacheBase<Asset::Mesh>
	{
	private:
		virtual void ImplLoad(std::ifstream& inFile, Asset::Mesh& dstAsset) override
		{

		}

		virtual void ImplSave(std::ofstream& outFile, const Asset::Mesh& srcAsset) const override
		{

		}

		virtual void ImplRemove(const std::string& key) override
		{

		}

	public:
		MeshCache()
			:AssetCacheBase(10, 10)
		{

		}

		// TODO - Implement move and copy constructors / operators
		MeshCache(const MeshCache&) = delete;
		MeshCache(MeshCache&&) = delete;
		MeshCache operator=(const MeshCache&) = delete;
		MeshCache operator=(MeshCache&&) = delete;
	};
}