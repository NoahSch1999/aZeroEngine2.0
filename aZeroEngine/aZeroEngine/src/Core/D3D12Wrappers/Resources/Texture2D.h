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

				Texture2D(ResourceRecycler& resourceRecycler, ID3D12Device* const device,
					const DXGI_FORMAT format,
					const D3D12_RESOURCE_FLAGS resourceFlags,
					const DXM::Vector2& dimensions,
					const int mipLevels, const int sampleCount,
					const D3D12_CLEAR_VALUE* const clearValue = nullptr,
					const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON)
				{
					Texture2D::Initialize(resourceRecycler, device, format, resourceFlags, dimensions, mipLevels, sampleCount, clearValue, initialState);
				}

				void Initialize(ResourceRecycler& resourceRecycler, ID3D12Device* const device,
					const DXGI_FORMAT format,
					const D3D12_RESOURCE_FLAGS resourceFlags,
					const DXM::Vector2& dimensions,
					const int mipLevels, const int sampleCount,
					const D3D12_CLEAR_VALUE* const clearValue = nullptr,
					const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON)
				{
					D3D12_RESOURCE_DESC description = {};
					description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
					description.Width = dimensions.x;
					description.Height = dimensions.y;
					description.DepthOrArraySize = 1;
					description.MipLevels = mipLevels;
					description.Format = format;
					description.SampleDesc.Count = sampleCount;
					description.SampleDesc.Quality = 0;
					description.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
					description.Flags = resourceFlags;

					ResourceBase::Initialize(resourceRecycler, device, description, D3D12_HEAP_TYPE_DEFAULT, clearValue, initialState);
				}

				Texture2D(Texture2D&& other) noexcept
					:ResourceBase(std::move(other))
				{

				}

				Texture2D& operator=(Texture2D&& other) noexcept
				{
					ResourceBase::operator=(std::move(other));
					return *this;
				}

				virtual ~Texture2D()
				{

				}
			};
		}
	}
}