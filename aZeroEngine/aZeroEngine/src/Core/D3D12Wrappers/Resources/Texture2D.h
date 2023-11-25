#pragma once
#include "ResourceBase.h"

namespace aZero
{
	namespace D3D12
	{
		namespace Resource
		{
			class Texture2D : public ResourceBase
			{
			public:
				Texture2D() = default;

				Texture2D(ID3D12Device* const device,
					const DXGI_FORMAT format,
					const D3D12_RESOURCE_FLAGS resourceFlags,
					const int width, const int height,
					const int mipLevels, const int sampleCount,
					const D3D12_CLEAR_VALUE* const clearValue = nullptr,
					const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON)
				{
					Texture2D::Initialize(device, format, resourceFlags, width, height, mipLevels, sampleCount, clearValue, initialState);
				}

				void Initialize(ID3D12Device* const device,
					const DXGI_FORMAT format,
					const D3D12_RESOURCE_FLAGS resourceFlags,
					const int width, const int height,
					const int mipLevels, const int sampleCount,
					const D3D12_CLEAR_VALUE* const clearValue = nullptr,
					const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON)
				{
					D3D12_RESOURCE_DESC description = {};
					description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
					description.Width = width;
					description.Height = height;
					description.DepthOrArraySize = 1;
					description.MipLevels = mipLevels;
					description.Format = format;
					description.SampleDesc.Count = sampleCount;
					description.SampleDesc.Quality = 0;
					description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
					description.Flags = resourceFlags;

					ResourceBase::Initialize(device, description, D3D12_HEAP_TYPE_DEFAULT, clearValue, initialState);
				}

				virtual ~Texture2D()
				{

				}
			};
		}
	}
}