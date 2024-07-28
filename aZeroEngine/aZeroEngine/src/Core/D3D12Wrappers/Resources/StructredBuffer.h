#pragma once
#include "DefaultHeapBuffer.h"
#include "UploadHeapBuffer.h"

namespace aZero
{
	namespace D3D12
	{
		template<typename ElementType>
		class UploadHeapStructuredBuffer
		{
		private:
			UploadHeapBuffer m_Buffer;
			int m_NumElements = -1;

		public:
			UploadHeapStructuredBuffer(int NumElements, DXGI_FORMAT Format, std::optional<ResourceRecycler> OptResourceRecycler)
			{
				this->Init(NumElements, Format, OptResourceRecycler);
			}

			UploadHeapStructuredBuffer(UploadHeapStructuredBuffer&& other) noexcept
			{
				m_Buffer = std::move(other.m_Buffer);
				m_NumElements = other.m_NumElements;
			}

			UploadHeapStructuredBuffer& operator=(UploadHeapStructuredBuffer&& other) noexcept
			{
				if (this != &other)
				{
					m_Buffer = std::move(other.m_Buffer);
					m_NumElements = other.m_NumElements;
				}

				return *this;
			}

			void Init(int NumElements, DXGI_FORMAT Format, std::optional<ResourceRecycler> OptResourceRecycler)
			{
				m_Buffer = std::move(UploadHeapBuffer(NumElements, sizeof(ElementType), Format, OptResourceRecycler));
				m_NumElements = NumElements;
			}

			template <typename BufferType>
			void Write(int StartIndex, int EndIndex, const BufferType& Src, int SrcOffset, ID3D12GraphicsCommandList* CmdList)
			{
				m_Buffer.Write(StartIndex, Src, SrcOffset, (EndIndex - StartIndex) * sizeof(ElementType), CmdList);
			}
			
			void Write(int StartIndex, int EndIndex, void* Data)
			{
				m_Buffer.Write(StartIndex, Data, (EndIndex - StartIndex) * sizeof(ElementType));
			}

			UploadHeapBuffer& GetInternalBuffer() { return m_Buffer; }

			int GetNumElements() const { return m_NumElements; }
		};

		template<typename ElementType>
		class DefaultHeapStructuredBuffer
		{
		private:
			DefaultHeapBuffer m_Buffer;
			int m_NumElements = -1;

		public:
			DefaultHeapStructuredBuffer(int NumElements, DXGI_FORMAT Format, std::optional<ResourceRecycler> OptResourceRecycler)
			{
				this->Init(NumElements, Format, OptResourceRecycler);
			}

			DefaultHeapStructuredBuffer(DefaultHeapStructuredBuffer&& other) noexcept
			{
				m_Buffer = std::move(other.m_Buffer);
				m_NumElements = other.m_NumElements;
			}

			DefaultHeapStructuredBuffer& operator=(DefaultHeapStructuredBuffer&& other) noexcept
			{
				if (this != &other)
				{
					m_Buffer = std::move(other.m_Buffer);
					m_NumElements = other.m_NumElements;
				}

				return *this;
			}

			void Init(int NumElements, DXGI_FORMAT Format, std::optional<ResourceRecycler> OptResourceRecycler)
			{
				m_Buffer = std::move(DefaultHeapBuffer(NumElements, sizeof(ElementType), Format, OptResourceRecycler));
				m_NumElements = NumElements;
			}

			template <typename BufferType>
			void Write(int StartIndex, int EndIndex, const BufferType& Src, int SrcOffset, ID3D12GraphicsCommandList* CmdList)
			{
				m_Buffer.Write(StartIndex, Src, SrcOffset, (EndIndex - StartIndex) * sizeof(ElementType), CmdList);
			}

			void Write(int StartIndex, int EndIndex, void* Data, UploadHeapBuffer& SrcIntermediate, int SrcOffset, ID3D12GraphicsCommandList* CmdList)
			{
				m_Buffer.Write(StartIndex, Data, SrcIntermediate, SrcOffset, (EndIndex - StartIndex) * sizeof(ElementType), CmdList);
			}

			DefaultHeapBuffer& GetInternalBuffer() { return m_Buffer; }

			int GetNumElements() const { return m_NumElements; }
		};
	}
}