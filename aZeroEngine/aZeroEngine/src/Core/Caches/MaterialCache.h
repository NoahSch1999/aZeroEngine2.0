#pragma once
#include "AssetCacheBase.h"
#include "../AssetTypes/MaterialAsset.h"

#define MATERIAL_ASSET_DIRECTORY ""

namespace aZero
{
	class MaterialCache : public AssetCacheBase<Asset::Material>
	{
	private:
		virtual void ImplLoad(std::ifstream& inFile, Asset::Material& dstAsset) override
		{

		}

		virtual void ImplSave(std::ofstream& outFile, const Asset::Material& srcAsset) const override
		{

		}

		virtual void ImplRemove(const std::string& key) override
		{

		}

	public:
		MaterialCache()
			:AssetCacheBase(10, 10)
		{

		}

		// TODO - Implement move and copy constructors / operators
		MaterialCache(const MaterialCache&) = delete;
		MaterialCache(MaterialCache&&) = delete;
		MaterialCache operator=(const MaterialCache&) = delete;
		MaterialCache operator=(MaterialCache&&) = delete;
	};
}