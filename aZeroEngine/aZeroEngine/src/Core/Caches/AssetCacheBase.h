#pragma once
#include <fstream>
#include "../../DataStructures/PackedLookupArray.h"

namespace aZero
{
	template<typename AssetType>
	class AssetCacheBase
	{
	private:
		DataStructures::PackedLookupArray<std::string, AssetType> m_assets;

		virtual void ImplLoad(std::ifstream& inFile, AssetType& dstAsset) = 0;
		virtual void ImplSave(std::ofstream& outFile, const AssetType& srcAsset) const = 0;
		virtual void ImplRemove(const std::string& key) = 0;

	public:
		AssetCacheBase(std::size_t preAllocatedNumElements = 1, std::size_t numPerIncreaseInternal = 1)
			:m_assets(preAllocatedNumElements, numPerIncreaseInternal)
		{

		}

		// TODO - Implement move and copy constructors / operators
		AssetCacheBase(const AssetCacheBase&) = delete;
		AssetCacheBase(AssetCacheBase&&) = delete;
		AssetCacheBase operator=(const AssetCacheBase&) = delete;
		AssetCacheBase operator=(AssetCacheBase&&) = delete;

		// TODO - Add version that takes in an already opened file
		void Load(const std::string& key, const std::string& path)
		{
			std::ifstream file(path, std::ios::binary);

			if (file.is_open())
			{
				AssetType asset;
				ImplLoad(file, asset);

				m_assets.Add(key, std::move(asset));
			}
		}

		// TODO - Add version that takes in an already opened file
		void Save(const std::string& key, const std::string& path) const
		{
			std::ofstream file(path, std::ios::binary | std::ios::trunc);
			if (file.is_open())
			{
				const AssetType& asset(m_assets.GetElementConstRef(key));
				ImplSave(file, asset);
			}
		}

		void Remove(const std::string& key)
		{
			ImplRemove(key);
		}

		void Store(const std::string& key, AssetType&& asset)
		{
			m_assets.Add(key, std::forward<AssetType>(asset));
		}

		bool Exists(const std::string& key) const { return m_assets.Exists(key); }
	};
}