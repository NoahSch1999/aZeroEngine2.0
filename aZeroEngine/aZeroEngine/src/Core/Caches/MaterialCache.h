#pragma once
#include "TextureCache.h"
#include "../AssetTypes/MaterialAsset.h"

#define MATERIAL_ASSET_DIRECTORY ""

namespace aZero
{
	class MaterialCache : public AssetCacheBase<std::string, Asset::Material>
	{
	private:
		TextureCache& m_TextureCache;

	public:
		MaterialCache(TextureCache& TextureCache)
			:AssetCacheBase(10), m_TextureCache(TextureCache)
		{

		}
		
		void LoadFromFile(const std::string& FilePath, ID3D12GraphicsCommandList* CmdList)
		{
			// Loads material file
			
			// It loads textures if any neccessary texture isnt in the texture cache
		}

		// TODO - Implement move and copy constructors / operators
		MaterialCache(const MaterialCache&) = delete;
		MaterialCache(MaterialCache&&) = delete;
		MaterialCache operator=(const MaterialCache&) = delete;
		MaterialCache operator=(MaterialCache&&) = delete;
	};
}