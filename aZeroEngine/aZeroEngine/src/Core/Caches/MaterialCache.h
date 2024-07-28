#pragma once
#include <typeindex>

#include "TextureCache.h"
#include "../AssetTypes/MaterialAsset.h"

#define MATERIAL_ASSET_DIRECTORY ""

namespace aZero
{
	class MaterialCache : public AssetCacheBase<std::string, std::unique_ptr<Asset::MaterialBase>>
	{
	private:
		TextureCache& m_TextureCache;

		void WriteMaterialToFile()
		{

		}

	public:
		MaterialCache(TextureCache& TextureCache)
			:m_TextureCache(TextureCache)
		{

		}

		void LoadFromFile(const std::string& FilePath, ID3D12GraphicsCommandList* CmdList)
		{


			// It loads textures if any neccessary texture isnt in the texture cache
		}

		// TODO - Implement move and copy constructors / operators
		MaterialCache(const MaterialCache&) = delete;
		MaterialCache(MaterialCache&&) = delete;
		MaterialCache operator=(const MaterialCache&) = delete;
		MaterialCache operator=(MaterialCache&&) = delete;
	};

	//template<typename ...MaterialTypes>
	//class MaterialManager
	//{
	//private:
	//	std::tuple<MaterialCache<MaterialTypes>...> m_MaterialCaches;
	//	TextureCache& m_TextureCache;

	//	void WriteMaterialToFile()
	//	{

	//	}

	//public:
	//	MaterialManager(TextureCache& TextureCache)
	//		:m_TextureCache(TextureCache)
	//	{

	//	}
	//	
	//	void LoadFromFile(const std::string& FilePath, ID3D12GraphicsCommandList* CmdList)
	//	{
	//		// Loads material file and creates a Asset::Material template specialization depending on the typeid that the material file contains
	//		
	//		
	//		// It loads textures if any neccessary texture isnt in the texture cache
	//	}

	//	template<typename MaterialType>
	//	void SaveMaterial(const std::string& Key)
	//	{
	//		MaterialType& Material = GetMaterial<MaterialType>(Key);
	//		if constexpr (std::is_same_v<MaterialType, Asset::ToonMaterial>)
	//		{
	//			Asset::ToonMaterial& ToonMaterial = Material;

	//			std::type_index TypeInfo = typeid(Asset::ToonMaterial);
	//			TypeInfo.name(); // use name to save what type

	//			// Save material to file

	//		}
	//	}

	//	template<typename MaterialType>
	//	void SaveMaterial(const std::string& Key, const MaterialType& Material)
	//	{
	//		if constexpr (std::is_same_v<MaterialType, Asset::ToonMaterial>)
	//		{
	//			const Asset::ToonMaterial& ToonMaterial = Material;

	//			std::type_index TypeInfo = typeid(Asset::ToonMaterial);
	//			TypeInfo.name(); // use name to save what type

	//			// Save material to file

	//		}
	//	}

	//	template<typename MaterialType>
	//	MaterialType& GetMaterial(const std::string& Key)
	//	{
	//		return std::get<MaterialCache<MaterialTypes>...>(m_MaterialCaches).Get(Key);
	//	}

	//	auto& GetMaterial(const std::string& Key, Asset::MATERIAL_TYPE MaterialType)
	//	{
	//		return std::get<MaterialCache<MaterialTypes>...>(m_MaterialCaches).Get(Key);
	//	}

	//	template<typename MaterialType>
	//	void StoreMaterial(const std::string& Key, MaterialType&& Material)
	//	{
	//		std::get<MaterialCache<MaterialTypes>...>(m_MaterialCaches).Store(Key, std::forward<MaterialType>(Material));
	//	}

	//	template<typename MaterialType>
	//	MaterialCache<MaterialTypes...>& GetCache()
	//	{
	//		return std::get<MaterialCache<MaterialTypes>...>(m_MaterialCaches);
	//	}

	//	// TODO - Implement move and copy constructors / operators
	//	MaterialManager(const MaterialManager&) = delete;
	//	MaterialManager(MaterialManager&&) = delete;
	//	MaterialManager operator=(const MaterialManager&) = delete;
	//	MaterialManager operator=(MaterialManager&&) = delete;
	//};
}