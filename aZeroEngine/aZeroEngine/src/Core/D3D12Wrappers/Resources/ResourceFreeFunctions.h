#pragma once
#include "GPUBuffer.h"
#include "UploadBuffer.h"
#include "ReadbackBuffer.h"
#include "Texture2D.h"
#include "../../Helpers/HelperFunctions.h"

namespace aZero
{
	namespace D3D12
	{
		namespace Resource
		{
			void UpdateBuffer(
				const Buffer& dstBuffer, const int dstOffset,
				const Buffer& srcBuffer, const int srcOffset,
				const int numBytes,
				ID3D12GraphicsCommandList* const commandList);

			void UpdateBuffer(const UploadBuffer& dstBuffer, const int dstOffset, const void* const data, const int numBytes);

			void UploadToGPUBuffer(
				const GPUBuffer& dstBuffer, const int dstOffset,
				const UploadBuffer& srcBuffer, const int srcBufferOffset,
				const void* const data, const int numBytes,
				ID3D12GraphicsCommandList* const commandList);
		
			void UploadToTexture(const Texture2D& dstTexture,
				const UploadBuffer& stagingBuffer,
				const int stagingBufferOffset,
				const void* const data,
				const D3D12_SUBRESOURCE_DATA& subresourceData,
				ID3D12GraphicsCommandList* const commandList
			);

			void TransitionResource(ID3D12GraphicsCommandList* const commandList, ResourceBase& resource, D3D12_RESOURCE_STATES newState);

			void TransitionResource(ID3D12GraphicsCommandList* const commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState);

			void TransitionResources(
				ID3D12GraphicsCommandList* const commandList,
				std::vector<std::pair<D3D12_RESOURCE_STATES, ResourceBase*>>& resources);

			void CopyTextureToTexture(Texture2D& dstTexture, Texture2D& srcTexture, ID3D12GraphicsCommandList* const commandList);

			void CopyTextureToTexture(ID3D12Resource* const dstTexture, Texture2D& srcTexture, ID3D12GraphicsCommandList* const commandList);
		}
	}
}