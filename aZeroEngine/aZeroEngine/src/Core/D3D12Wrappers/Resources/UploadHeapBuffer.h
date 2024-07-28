#pragma once
#include "GPUBuffer.h"

namespace aZero
{
	namespace D3D12
	{
		class DefaultHeapBuffer;

		class UploadHeapBuffer : public GPUBuffer
		{
		private:

		public:
			UploadHeapBuffer() = default;

			UploadHeapBuffer(int NumElements, int BytesPerElement, DXGI_FORMAT Format, std::optional<ResourceRecycler*> OptResourceRecycler)
			{
				this->Init(NumElements, BytesPerElement, Format, OptResourceRecycler);
			}

			void Init(int NumElements, int BytesPerElement, DXGI_FORMAT Format, std::optional<ResourceRecycler*> OptResourceRecycler)
			{
				GPUBuffer::Init(NumElements, BytesPerElement, Format, D3D12_HEAP_TYPE_UPLOAD, OptResourceRecycler);
			}

			void Write(int DstOffset, void* Data, int NumBytes);
			void Write(int DstOffset, const UploadHeapBuffer& Src, int SrcOffset, int NumBytes, ID3D12GraphicsCommandList* CmdList);
			void Write(int DstOffset, const DefaultHeapBuffer& Src, int SrcOffset, int NumBytes, ID3D12GraphicsCommandList* CmdList);
		};
	}
}