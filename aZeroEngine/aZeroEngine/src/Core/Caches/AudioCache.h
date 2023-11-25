#pragma once
#include "AssetCacheBase.h"
#include "../AssetTypes/AudioAsset.h"

#define AUDIO_ASSET_DIRECTORY ""

namespace aZero
{
	class AudioCache : public AssetCacheBase<Asset::Audio>
	{
	private:
		virtual void ImplLoad(std::ifstream& inFile, Asset::Audio& dstAsset) override
		{

		}

		virtual void ImplSave(std::ofstream& outFile, const Asset::Audio& srcAsset) const override
		{

		}

		virtual void ImplRemove(const std::string& key) override
		{

		}

	public:
		AudioCache()
			:AssetCacheBase(10, 10)
		{

		}

		// TODO - Implement move and copy constructors / operators
		AudioCache(const AudioCache&) = delete;
		AudioCache(AudioCache&&) = delete;
		AudioCache operator=(const AudioCache&) = delete;
		AudioCache operator=(AudioCache&&) = delete;
	};
}