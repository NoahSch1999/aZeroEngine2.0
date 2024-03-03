#pragma once
#include "AssetCacheBase.h"
#include "../AssetTypes/TextureAsset.h"

namespace aZero
{
	class TextureCache : public AssetCacheBase<std::string, Asset::Texture>
	{
	private:
		D3D12::ResourceRecycler& m_ResourceRecycler;
		D3D12::DescriptorManager& m_DescriptorManager;
	public:
		TextureCache(D3D12::ResourceRecycler& ResourceRecycler, D3D12::DescriptorManager& DescriptorManager)
			:AssetCacheBase(10), m_ResourceRecycler(ResourceRecycler), m_DescriptorManager(DescriptorManager)
		{

		}

		void LoadFromFile(const std::string& FilePath, ID3D12GraphicsCommandList* CmdList, D3D12_RESOURCE_FLAGS ResourceUsageFlags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE)
		{
			ID3D12Device* Device = nullptr;
			CmdList->GetDevice(IID_PPV_ARGS(&Device));

			Asset::LoadedTextureData LoadedTextureData;
			if (Asset::LoadTexture2DFromFile(Device, FilePath, LoadedTextureData))
			{
				const int LastSlashIndex = LoadedTextureData.FilePath.find_last_of('/');
				const int SuffixStartIndex = LoadedTextureData.FilePath.find_last_of('.');
				const std::string TextureName(LoadedTextureData.FilePath.substr(LastSlashIndex + 1, LoadedTextureData.FilePath.length() - LastSlashIndex - (LoadedTextureData.FilePath.length() - SuffixStartIndex + 1)));
				Store(TextureName, Asset::Texture(m_ResourceRecycler, CmdList, LoadedTextureData, ResourceUsageFlags, 1, nullptr));
				Get(TextureName).GetTexture().CreateTextureSRV(m_DescriptorManager);
			}
		}

		// TODO - Implement move and copy constructors / operators
		TextureCache(const TextureCache&) = delete;
		TextureCache(TextureCache&&) = delete;
		TextureCache operator=(const TextureCache&) = delete;
		TextureCache operator=(TextureCache&&) = delete;
	};
}