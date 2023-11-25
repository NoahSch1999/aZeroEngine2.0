#pragma once
#include "AssetCacheBase.h"
#include "../AssetTypes/TextureAsset.h"

#define TEXTURE_ASSET_DIRECTORY ""

namespace aZero
{
	class TextureCache : public AssetCacheBase<Asset::Texture>
	{
	private:
		virtual void ImplLoad(std::ifstream& inFile, Asset::Texture& dstAsset) override
		{

		}

		virtual void ImplSave(std::ofstream& outFile, const Asset::Texture& srcAsset) const override
		{

		}

		virtual void ImplRemove(const std::string& key) override
		{

		}

	public:
		TextureCache()
			:AssetCacheBase(10, 10)
		{

		}

		// TODO - Implement move and copy constructors / operators
		TextureCache(const TextureCache&) = delete;
		TextureCache(TextureCache&&) = delete;
		TextureCache operator=(const TextureCache&) = delete;
		TextureCache operator=(TextureCache&&) = delete;
	};
}