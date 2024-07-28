#pragma once
#include "../D3D12Wrappers/Resources/GPUResource.h"
#include "../D3D12Wrappers/Descriptors/Descriptor.h"
#include "../D3D12Wrappers/Resources/GPUResource.h"

namespace aZero
{
	namespace Asset
	{
		struct LoadedTextureData
		{
			std::string FilePath;
			std::vector<D3D12_SUBRESOURCE_DATA> SubresourceData;
			std::unique_ptr<uint8_t[]> Data = nullptr;
			Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
		};

		bool LoadTexture2DFromFile(ID3D12Device* Device, const std::string& FilePath, LoadedTextureData& OutData);

		class Texture
		{
		private:
			std::string Name = "";
			D3D12::GPUResource_Deprecated m_Texture;

		public:
			Texture() = default;

			Texture(D3D12::ResourceRecycler& ResourceRecycler, ID3D12GraphicsCommandList* const CmdList, const LoadedTextureData& LoadedTextureData, D3D12_RESOURCE_FLAGS ResourceUsageFlags, int SampleCount = 1, D3D12_CLEAR_VALUE* ClearValue = nullptr)
			{
				Name = LoadedTextureData.FilePath;

				const D3D12_RESOURCE_DESC& ResourceDesc = LoadedTextureData.Resource->GetDesc();

				DEBUG_CHECK(ResourceDesc.Width != 0);
				DEBUG_CHECK(ResourceDesc.Height != 0);
				DEBUG_CHECK(LoadedTextureData.Data);

				ID3D12Device* Device = nullptr;
				CmdList->GetDevice(IID_PPV_ARGS(&Device));

				D3D12::TextureResourceDesc TextureDesc;
				TextureDesc.Dimensions = { float(ResourceDesc.Width), float(ResourceDesc.Height) };
				TextureDesc.ClearValue = ClearValue;
				TextureDesc.Format = ResourceDesc.Format;
				TextureDesc.MSSampleCount = SampleCount;
				TextureDesc.NumMipLevels = ResourceDesc.MipLevels;
				TextureDesc.UsageFlags = ResourceUsageFlags;

				m_Texture = std::move(D3D12::GPUResource_Deprecated(Device, ResourceRecycler, TextureDesc));

				// TODO - Create subdata and fix the upload from upload buffer to texture
				D3D12_SUBRESOURCE_DATA SubData;
				D3D12::GPUResource_Deprecated::UploadToTexture(CmdList, m_Texture, LoadedTextureData.SubresourceData);

			#ifdef _DEBUG
				std::wstring DebugName(Name.begin(), Name.end());
				m_Texture.GetResource()->SetName(DebugName.c_str());
			#endif
			}

			D3D12::GPUResource_Deprecated& GetTexture()
			{
				return m_Texture;
			}
		};
	}
}