#include "TextureAsset.h"
#include "DirectXTK12/Inc/WICTextureLoader.h"
#include "DirectXTK12/Inc/DDSTextureLoader.h"

bool aZero::Asset::LoadTexture2DFromFile(ID3D12Device* Device, const std::string& FilePath, LoadedTextureData& OutData)
{
	const std::wstring FilePathWStr(FilePath.begin(), FilePath.end());

	if (FilePath.ends_with(".dds"))
	{
		OutData.FilePath = FilePath;
		return FAILED(DirectX::LoadDDSTextureFromFile(Device, FilePathWStr.c_str(), OutData.Resource.GetAddressOf(), OutData.Data, OutData.SubresourceData));
	}
	else
	{
		D3D12_SUBRESOURCE_DATA subData;
		if (FAILED(DirectX::LoadWICTextureFromFile(Device, FilePathWStr.c_str(), OutData.Resource.GetAddressOf(), OutData.Data, subData)))
		{
			return false;
		}

		OutData.FilePath = FilePath;
		OutData.SubresourceData.push_back(subData);
	}

	return true;
}