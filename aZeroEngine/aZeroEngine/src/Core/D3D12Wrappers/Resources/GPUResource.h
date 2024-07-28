#pragma once
#include <span>
#include <optional>

#include "../../Singleton/Singleton.h"
#include "../../../Macros/DebugMacros.h"
#include "../../D3D12Wrappers/Descriptors/DescriptorManager.h"
#include "../ResourceRecycler.h"

namespace aZero
{
	namespace D3D12
	{
		class GPUBuffer;
		class GPUTexture;

		class GPUResourceBase
		{
		private:
			ResourceRecycler* m_ResourceRecycler = nullptr;
			Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
			void* m_MappedPtr = nullptr;

		private:
			void MoveOp(GPUResourceBase& other)
			{
				m_Resource = other.m_Resource;
				m_ResourceRecycler = other.m_ResourceRecycler;
				m_MappedPtr = other.m_MappedPtr;

				other.m_MappedPtr = nullptr;
				other.m_Resource = nullptr;
				other.m_ResourceRecycler = nullptr;
			}
			
		public:
			GPUResourceBase() = default;

			GPUResourceBase(
				D3D12_RESOURCE_DESC ResourceDesc,
				D3D12_HEAP_PROPERTIES HeapProperties, 
				D3D12_HEAP_FLAGS HeapFlags, 
				D3D12_RESOURCE_STATES InitialResourceState,
				std::optional<D3D12_CLEAR_VALUE> OptOptimizedClearValue,
				std::optional<ResourceRecycler*> OptResourceRecycler
			)
			{
				this->Init(ResourceDesc, HeapProperties, HeapFlags, InitialResourceState, OptOptimizedClearValue, OptResourceRecycler);
			}

			GPUResourceBase(GPUResourceBase&& other) noexcept
			{
				MoveOp(other);
			}

			GPUResourceBase& operator=(GPUResourceBase&& other) noexcept
			{
				if (this != &other)
				{
					MoveOp(other);
				}

				return *this;
			}


			~GPUResourceBase()
			{
				if (m_Resource)
				{
					if (m_ResourceRecycler)
					{
						m_ResourceRecycler->RecycleResource(m_Resource);
					}
				}
			}

			void Init(
				D3D12_RESOURCE_DESC ResourceDesc,
				D3D12_HEAP_PROPERTIES HeapProperties,
				D3D12_HEAP_FLAGS HeapFlags,
				D3D12_RESOURCE_STATES InitialResourceState,
				std::optional<D3D12_CLEAR_VALUE> OptOptimizedClearValue,
				std::optional<ResourceRecycler*> OptResourceRecycler
			)
			{
				if (m_Resource)
				{
					if (m_ResourceRecycler)
					{
						m_ResourceRecycler->RecycleResource(m_Resource);
					}

					m_Resource = nullptr;
				}

				m_ResourceRecycler = OptResourceRecycler.has_value() ? OptResourceRecycler.value() : nullptr;

				const HRESULT HResult = gDevice->CreateCommittedResource(
					&HeapProperties,
					D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
					&ResourceDesc,
					InitialResourceState,
					OptOptimizedClearValue.has_value() ? &OptOptimizedClearValue.value() : nullptr,
					IID_PPV_ARGS(m_Resource.GetAddressOf()));

				if (FAILED(HResult))
				{
					throw;
				}

				if (HeapProperties.Type == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD
					|| HeapProperties.Type == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_READBACK)
				{
					m_Resource->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedPtr));
				}
			}

			ID3D12Resource* GetResource() const { return m_Resource.Get(); }
			void* GetMappedPtr() const { return m_MappedPtr; }
		};

			enum RESOURCE_ACCESS_TYPE { GPU_ONLY = 1, CPU_WRITE = 2, CPU_READ = 3, INVALID = 1337 };

			struct TextureResourceDesc
			{
				DXM::Vector2 Dimensions = DXM::Vector2(-1.f, -1.f);
				DXGI_FORMAT Format = DXGI_FORMAT::DXGI_FORMAT_FORCE_UINT;
				D3D12_RESOURCE_FLAGS UsageFlags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
				D3D12_RESOURCE_STATES InitialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
				int NumMipLevels = 1;
				int MSSampleCount = 1;
				D3D12_CLEAR_VALUE* ClearValue = nullptr;
			};

			struct BufferResourceDesc
			{
				int NumBytes = -1;
				RESOURCE_ACCESS_TYPE AccessType = RESOURCE_ACCESS_TYPE::INVALID;
				D3D12_RESOURCE_FLAGS UsageFlags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
			};

			class GPUResource_Deprecated
			{
			private:
	#ifdef _DEBUG
				int UniqueDebugIndex = -1;
	#endif // _DEBUG

				ResourceRecycler* m_ResourceRecycler = nullptr;

				Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
				void* m_MappedPtr = nullptr;
				RESOURCE_ACCESS_TYPE m_AccessType = RESOURCE_ACCESS_TYPE::INVALID;
				std::optional<D3D12_CLEAR_VALUE> m_ClearValue;

				D3D12_RESOURCE_STATES m_ResourceState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;

				D3D12::DescriptorManager* m_DescriptorManager = nullptr;
				D3D12::Descriptor m_SrvDescriptor;
				D3D12::Descriptor m_UavDescriptor;
				D3D12::Descriptor m_DsvRtvDescriptor;

				void Initialize(ID3D12Device* const Device,
					const D3D12_RESOURCE_DESC& Description,
					const RESOURCE_ACCESS_TYPE AccessType,
					const D3D12_CLEAR_VALUE* ClearValue,
					const D3D12_RESOURCE_STATES InitialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON);

			public:
				GPUResource_Deprecated() = default;

				GPUResource_Deprecated(ID3D12Device* Device, ResourceRecycler& ResourceRecycler, const TextureResourceDesc& Description);

				GPUResource_Deprecated(ID3D12Device* Device, ResourceRecycler& ResourceRecycler, const BufferResourceDesc& Description);

				GPUResource_Deprecated(GPUResource_Deprecated&& other) noexcept;

				GPUResource_Deprecated& operator=(GPUResource_Deprecated&& other) noexcept;

				~GPUResource_Deprecated();

				GPUResource_Deprecated(const GPUResource_Deprecated&& other) = delete;
				GPUResource_Deprecated& operator=(const GPUResource_Deprecated& other) = delete;

				void ResizeTexture(const DXM::Vector2& NewDimensions);

				void CreateTextureSRV(D3D12::DescriptorManager& DescriptorManager, int MaxMipLevel = 0);

				void CreateBufferSRV(D3D12::DescriptorManager& DescriptorManager, int ElementSizeBytes, int NumElements, int FirstElementIndex = 0);

				void CreateTextureUAV(D3D12::DescriptorManager& DescriptorManager, int MipIndex = 0);

				void CreateBufferUAV(D3D12::DescriptorManager& DescriptorManager, int ElementSizeBytes, int NumElements, int FirstElementIndex = 0);

				void CreateTextureRTV(D3D12::DescriptorManager& DescriptorManager, int MipIndex = 0);

				void CreateTextureDSV(D3D12::DescriptorManager& DescriptorManager, int MipIndex = 0);

				ID3D12Resource* const GetResource() const { return m_Resource.Get(); }
				void* GetMappedPtr() const { return m_MappedPtr; }
				RESOURCE_ACCESS_TYPE GetAccessType() const { return m_AccessType; }

				D3D12_RESOURCE_STATES GetResourceState() const { return m_ResourceState; }
				void SetResourceState(const D3D12_RESOURCE_STATES newState) { m_ResourceState = newState; }

				int GetBindlessIndexSRV() const { return m_SrvDescriptor.GetHeapIndex(); }
				int GetBindlessIndexUAV() const { return m_UavDescriptor.GetHeapIndex(); }

				const Descriptor& GetDescriptorSRV() const { return m_SrvDescriptor; }
				const Descriptor& GetDescriptorUAV() const { return m_UavDescriptor; }
				const Descriptor& GetDescriptorRTVDSV() const { return m_DsvRtvDescriptor; }

				std::optional<D3D12_CLEAR_VALUE> GetClearValue() { return m_ClearValue; }

				static void UploadToTexture(ID3D12GraphicsCommandList* CmdList, GPUResource_Deprecated& DstResource, const std::vector<D3D12_SUBRESOURCE_DATA>& SubresourceData);

				static void CopyBufferToBuffer(ID3D12GraphicsCommandList* CmdList, GPUResource_Deprecated& DstResource, int DstOffset, GPUResource_Deprecated& SrcResource, int SrcOffset, int NumBytes, const void* OptionalData = nullptr, int OptionalDataOffset = 0);
			
				static void CopyTextureToTexture(ID3D12GraphicsCommandList* CmdList, GPUResource_Deprecated& DstResource, const DXM::Vector2& DstOffset, GPUResource_Deprecated& SrcResource, const DXM::Vector2& SrcOffset);

				static void TransitionState(ID3D12GraphicsCommandList* CmdList, GPUResource_Deprecated& Resource, D3D12_RESOURCE_STATES NewState);
				
				static void TransitionState(ID3D12GraphicsCommandList* CmdList, ID3D12Resource* Resource, D3D12_RESOURCE_STATES OldState, D3D12_RESOURCE_STATES NewState);
				
				static void TransitionState(ID3D12GraphicsCommandList* CmdList, std::vector<std::pair<D3D12_RESOURCE_STATES, GPUResource_Deprecated*>>& Resources);
				};
	}
}