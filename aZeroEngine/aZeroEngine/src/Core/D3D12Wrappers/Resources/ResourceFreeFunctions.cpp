#include "ResourceFreeFunctions.h"

void aZero::D3D12::Resource::UpdateBuffer(const Buffer& dstBuffer, const int dstOffset, const Buffer& srcBuffer, const int srcOffset, const int numBytes, ID3D12GraphicsCommandList* const commandList)
{
	commandList->CopyBufferRegion(dstBuffer.GetResource(), dstOffset, srcBuffer.GetResource(), srcOffset, numBytes);
}

void aZero::D3D12::Resource::UpdateBuffer(const UploadBuffer& dstBuffer, const int dstOffset, const void* const data, const int numBytes)
{
	memcpy((char*)dstBuffer.GetMappedPtr() + dstOffset, data, numBytes);
}

void aZero::D3D12::Resource::UploadToGPUBuffer(const GPUBuffer& dstBuffer, const int dstOffset, const UploadBuffer& srcBuffer, const int srcBufferOffset, const void* const data, const int numBytes, ID3D12GraphicsCommandList* const commandList)
{
	UpdateBuffer(srcBuffer, srcBufferOffset, data, numBytes);
	UpdateBuffer(dstBuffer, dstOffset, srcBuffer, srcBufferOffset, numBytes, commandList);
}

void aZero::D3D12::Resource::UploadToTexture(const Texture2D& dstTexture, const UploadBuffer& stagingBuffer, const int stagingBufferOffset, const void* const data, const D3D12_SUBRESOURCE_DATA& subresourceData, ID3D12GraphicsCommandList* const commandList)
{
	//UpdateSubresources(commandList, dstTexture.GetResource(), stagingBuffer.GetResource(), stagingBufferOffset, 0, 1, data, subresourceData);
}

void aZero::D3D12::Resource::TransitionResource(ID3D12GraphicsCommandList* const commandList, ResourceBase& resource, D3D12_RESOURCE_STATES newState)
{
	if (newState != resource.GetResourceState())
	{
		D3D12_RESOURCE_BARRIER barrier(CD3DX12_RESOURCE_BARRIER::Transition(resource.GetResource(), resource.GetResourceState(), newState));
		commandList->ResourceBarrier(1, &barrier);
		resource.ForceResourceState(newState);
	}
}

void aZero::D3D12::Resource::TransitionResource(ID3D12GraphicsCommandList* const commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState)
{
	if (newState != oldState)
	{
		D3D12_RESOURCE_BARRIER barrier(CD3DX12_RESOURCE_BARRIER::Transition(resource, oldState, newState));
		commandList->ResourceBarrier(1, &barrier);
	}
}

void aZero::D3D12::Resource::TransitionResources(ID3D12GraphicsCommandList* const commandList, std::vector<std::pair<D3D12_RESOURCE_STATES, ResourceBase*>>& resources)
{
	std::vector<D3D12_RESOURCE_BARRIER> barriers;
	barriers.reserve(resources.size());
	for (auto& [newState, resource] : resources)
	{
		D3D12_RESOURCE_STATES currentState = resource->GetResourceState();
		if (newState != currentState)
		{
			barriers.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(resource->GetResource(), currentState, newState));
			resource->ForceResourceState(newState);
		}
	}

	// TODO - Check that this still works even though "barriers" is local and might have been overwritten in memory before the commandlist is executed
	commandList->ResourceBarrier(barriers.size(), barriers.data());
}

void aZero::D3D12::Resource::CopyTextureToTexture(Texture2D& dstTexture, Texture2D& srcTexture, ID3D12GraphicsCommandList* const commandList)
{
	TransitionResource(commandList, dstTexture, D3D12_RESOURCE_STATE_COPY_DEST);
	TransitionResource(commandList, srcTexture, D3D12_RESOURCE_STATE_COPY_SOURCE);

	// NOTE - Copying causes the resources to go decay into D3D12_RESOURCE_STATE_COMMON once the GPU has executed the recorded command
	commandList->CopyResource(dstTexture.GetResource(), srcTexture.GetResource());
}

void aZero::D3D12::Resource::CopyTextureToTexture(ID3D12Resource* const dstTexture, Texture2D& srcTexture, ID3D12GraphicsCommandList* const commandList)
{
	TransitionResource(commandList, srcTexture, D3D12_RESOURCE_STATE_COPY_SOURCE);

	// NOTE - Copying causes the resources to go decay into D3D12_RESOURCE_STATE_COMMON once the GPU has executed the recorded command
	commandList->CopyResource(dstTexture, srcTexture.GetResource());
}
