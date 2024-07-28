#include "DefaultHeapBuffer.h"
#include "UploadHeapBuffer.h"

void aZero::D3D12::DefaultHeapBuffer::Write(int DstOffset, const UploadHeapBuffer& Src, int SrcOffset, int NumBytes, ID3D12GraphicsCommandList* CmdList)
{
	CmdList->CopyBufferRegion(this->GetResource(), (UINT64)DstOffset, Src.GetResource(), SrcOffset, NumBytes);
}

void aZero::D3D12::DefaultHeapBuffer::Write(int DstOffset, void* Data, UploadHeapBuffer& SrcIntermediate, int SrcOffset, int NumBytes, ID3D12GraphicsCommandList* CmdList)
{
	SrcIntermediate.Write(SrcOffset, Data, NumBytes);
	CmdList->CopyBufferRegion(this->GetResource(), (UINT64)DstOffset, SrcIntermediate.GetResource(), SrcOffset, NumBytes);
}

void aZero::D3D12::DefaultHeapBuffer::Write(int DstOffset, const DefaultHeapBuffer& Src, int SrcOffset, int NumBytes, ID3D12GraphicsCommandList* CmdList)
{
	CmdList->CopyBufferRegion(this->GetResource(), (UINT64)DstOffset, Src.GetResource(), SrcOffset, NumBytes);
}