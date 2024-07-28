#include "UploadHeapBuffer.h"
#include "DefaultHeapBuffer.h"

void aZero::D3D12::UploadHeapBuffer::Write(int DstOffset, void* Data, int NumBytes)
{
	memcpy((char*)this->GetMappedPtr() + DstOffset, Data, NumBytes);
}

void aZero::D3D12::UploadHeapBuffer::Write(int DstOffset, const UploadHeapBuffer& Src, int SrcOffset, int NumBytes, ID3D12GraphicsCommandList* CmdList)
{
	CmdList->CopyBufferRegion(this->GetResource(), (UINT64)DstOffset, Src.GetResource(), SrcOffset, NumBytes);
}

void aZero::D3D12::UploadHeapBuffer::Write(int DstOffset, const DefaultHeapBuffer& Src, int SrcOffset, int NumBytes, ID3D12GraphicsCommandList* CmdList)
{
	CmdList->CopyBufferRegion(this->GetResource(), (UINT64)DstOffset, Src.GetResource(), SrcOffset, NumBytes);
}
