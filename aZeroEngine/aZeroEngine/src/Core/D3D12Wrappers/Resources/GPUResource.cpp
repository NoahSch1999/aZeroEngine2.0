#include "GPUResource.h"

void aZero::D3D12::GPUResource_Deprecated::Initialize(ID3D12Device* const Device, const D3D12_RESOURCE_DESC& Description, const RESOURCE_ACCESS_TYPE AccessType, const D3D12_CLEAR_VALUE* ClearValue, const D3D12_RESOURCE_STATES InitialState)
{
	m_AccessType = AccessType;

	if (m_Resource)
	{
		m_ResourceRecycler->RecycleResource(m_Resource);
		m_Resource = nullptr;
		m_MappedPtr = nullptr;
		m_ResourceState = InitialState;
	}

	m_ResourceState = InitialState;

	// TODO - Add memory pool and other D3D12_HEAP_PROPERTIES settings
	//DEBUG_CHECK(AccessType != RESOURCE_ACCESS_TYPE::INVALID);
	D3D12_HEAP_PROPERTIES HeapProps = {};
	if (AccessType == RESOURCE_ACCESS_TYPE::GPU_ONLY)
	{
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	}
	else if (AccessType == RESOURCE_ACCESS_TYPE::CPU_WRITE)
	{
		HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
	}
	else if (AccessType == RESOURCE_ACCESS_TYPE::CPU_READ)
	{
		HeapProps.Type = D3D12_HEAP_TYPE_READBACK;
	}

	const HRESULT hr = Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE, &Description, m_ResourceState, ClearValue, IID_PPV_ARGS(&m_Resource));
	DEBUG_CHECK(SUCCEEDED(hr));

	if (AccessType == RESOURCE_ACCESS_TYPE::CPU_READ || AccessType == RESOURCE_ACCESS_TYPE::CPU_WRITE)
	{
		m_Resource->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedPtr));
	}
#ifdef _DEBUG
	static int UniqueDebugIndexCount = 0;
	UniqueDebugIndex = UniqueDebugIndexCount++;
	std::wstring DebugName(L"GPUResource: " + std::to_wstring(UniqueDebugIndex));
	m_Resource->SetName(DebugName.c_str());
#endif // _DEBUG
}

aZero::D3D12::GPUResource_Deprecated::GPUResource_Deprecated(ID3D12Device* Device, ResourceRecycler& ResourceRecycler, const TextureResourceDesc& Description)
	:m_ResourceRecycler(&ResourceRecycler)
{
	if (Description.ClearValue)
	{
		m_ClearValue = std::make_optional<D3D12_CLEAR_VALUE>(*Description.ClearValue);
	}

	D3D12_RESOURCE_DESC D3Desc;
	D3Desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	D3Desc.Format = Description.Format;
	D3Desc.MipLevels = Description.NumMipLevels;
	D3Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	D3Desc.Width = Description.Dimensions.x;
	D3Desc.Height = Description.Dimensions.y;
	D3Desc.DepthOrArraySize = 1;
	D3Desc.Alignment = 0;
	D3Desc.SampleDesc.Count = Description.MSSampleCount;
	D3Desc.SampleDesc.Quality = 0;
	D3Desc.Flags = Description.UsageFlags;

	this->Initialize(
		Device,
		D3Desc,
		RESOURCE_ACCESS_TYPE::GPU_ONLY,
		Description.ClearValue,
		Description.InitialState);
}

aZero::D3D12::GPUResource_Deprecated::GPUResource_Deprecated(ID3D12Device* Device, ResourceRecycler& ResourceRecycler, const BufferResourceDesc& Description)
	:m_ResourceRecycler(&ResourceRecycler)
{
	D3D12_RESOURCE_DESC D3Desc;
	D3Desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	D3Desc.Format = DXGI_FORMAT_UNKNOWN;
	D3Desc.MipLevels = 1;
	D3Desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	D3Desc.Width = Description.NumBytes;
	D3Desc.Height = 1;
	D3Desc.DepthOrArraySize = 1;
	D3Desc.Alignment = 0;
	D3Desc.SampleDesc.Count = 1;
	D3Desc.SampleDesc.Quality = 0;
	D3Desc.Flags = Description.UsageFlags;

	this->Initialize(
		Device,
		D3Desc,
		Description.AccessType,
		nullptr, D3D12_RESOURCE_STATE_COMMON);
}

aZero::D3D12::GPUResource_Deprecated::GPUResource_Deprecated(GPUResource_Deprecated&& other) noexcept
{
	m_Resource = other.m_Resource;
	m_ResourceRecycler = other.m_ResourceRecycler;
	m_ResourceState = other.m_ResourceState;
	m_MappedPtr = other.m_MappedPtr;
	m_AccessType = other.m_AccessType;
	m_ClearValue = other.m_ClearValue;
	m_SrvDescriptor = other.m_SrvDescriptor;
	m_UavDescriptor = other.m_UavDescriptor;
	m_DsvRtvDescriptor = other.m_DsvRtvDescriptor;
	m_DescriptorManager = other.m_DescriptorManager;

#ifdef _DEBUG
	UniqueDebugIndex = other.UniqueDebugIndex;
#endif // _DEBUG

	other.m_DescriptorManager = nullptr;
	other.m_MappedPtr = nullptr;
	other.m_Resource = nullptr;
	other.m_ClearValue.reset();
}

aZero::D3D12::GPUResource_Deprecated& aZero::D3D12::GPUResource_Deprecated::operator=(GPUResource_Deprecated&& other) noexcept
{

	if (this != &other)
	{
		m_Resource = other.m_Resource;
		m_ResourceRecycler = other.m_ResourceRecycler;
		m_ResourceState = other.m_ResourceState;
		m_MappedPtr = other.m_MappedPtr;
		m_AccessType = other.m_AccessType;
		m_ClearValue = other.m_ClearValue;
		m_SrvDescriptor = other.m_SrvDescriptor;
		m_UavDescriptor = other.m_UavDescriptor;
		m_DsvRtvDescriptor = other.m_DsvRtvDescriptor;
		m_DescriptorManager = other.m_DescriptorManager;

#ifdef _DEBUG
		UniqueDebugIndex = other.UniqueDebugIndex;
#endif // _DEBUG

		other.m_DescriptorManager = nullptr;
		other.m_MappedPtr = nullptr;
		other.m_Resource = nullptr;
		other.m_ClearValue.reset();
	}
	return *this;
}

aZero::D3D12::GPUResource_Deprecated::~GPUResource_Deprecated()
{
	if (m_Resource)
	{
		if (m_DescriptorManager)
		{
			if (m_SrvDescriptor.GetHeapIndex() != -1)
			{
				m_DescriptorManager->GetResourceHeap().RecycleDescriptor(m_SrvDescriptor);
			}

			if (m_UavDescriptor.GetHeapIndex() != -1)
			{
				m_DescriptorManager->GetResourceHeap().RecycleDescriptor(m_UavDescriptor);
			}

			if (m_DsvRtvDescriptor.GetHeapIndex() != -1)
			{
				const D3D12_RESOURCE_DESC& ResourceDesc = m_Resource->GetDesc();
				if ((ResourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) == D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
				{
					m_DescriptorManager->GetRtvHeap().RecycleDescriptor(m_DsvRtvDescriptor);
				}
				else if ((ResourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) == D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
				{
					m_DescriptorManager->GetDsvHeap().RecycleDescriptor(m_DsvRtvDescriptor);
				}
			#ifdef _DEBUG
				else
				{
					DebugBreak();
				}
			#endif // _DEBUG
			}
		}

		m_ResourceRecycler->RecycleResource(m_Resource);
	}
}

void aZero::D3D12::GPUResource_Deprecated::ResizeTexture(const DXM::Vector2& NewDimensions)
{
	DEBUG_CHECK((m_Resource != nullptr));

	ID3D12Device* Device = nullptr;
	m_Resource->GetDevice(IID_PPV_ARGS(&Device));

	const D3D12_RESOURCE_DESC& ResourceDesc = m_Resource->GetDesc();

	D3D12_RESOURCE_DESC D3Desc;
	D3Desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	D3Desc.Format = ResourceDesc.Format;
	D3Desc.MipLevels = ResourceDesc.MipLevels;
	D3Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	D3Desc.Width = NewDimensions.x;
	D3Desc.Height = NewDimensions.y;
	D3Desc.DepthOrArraySize = 1;
	D3Desc.Alignment = 0;
	D3Desc.SampleDesc.Count = ResourceDesc.SampleDesc.Count;
	D3Desc.SampleDesc.Quality = 0;
	D3Desc.Flags = ResourceDesc.Flags;
	
	Initialize(Device, D3Desc, m_AccessType, m_ClearValue.has_value() ? &m_ClearValue.value() : nullptr, m_ResourceState);

	if (m_DsvRtvDescriptor.GetHeapIndex() != -1)
	{
		if ((ResourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) == D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
		{
			CreateTextureRTV(*m_DescriptorManager); // TODO - Handle specific mip level
		}
		else if((ResourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) == D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
		{
			CreateTextureDSV(*m_DescriptorManager); // TODO - Handle specific mip level
		}
	}

	if (m_SrvDescriptor.GetHeapIndex() != -1)
	{
		CreateTextureSRV(*m_DescriptorManager); // TODO - Handle specific mip level
	}

	if (m_UavDescriptor.GetHeapIndex() != -1)
	{
		CreateTextureUAV(*m_DescriptorManager); // TODO - Handle specific mip level
	}
}

void aZero::D3D12::GPUResource_Deprecated::CreateTextureSRV(D3D12::DescriptorManager& DescriptorManager, int MaxMipLevel)
{
	DEBUG_CHECK(m_Resource);
	m_DescriptorManager = &DescriptorManager;

	ID3D12Device* Device = nullptr;
	m_Resource->GetDevice(IID_PPV_ARGS(&Device));

	if (m_SrvDescriptor.GetHeapIndex() == -1)
	{
		m_SrvDescriptor = DescriptorManager.GetResourceHeap().GetDescriptor();
	}

	const D3D12_RESOURCE_DESC& ResourceDesc = m_Resource->GetDesc();
	DEBUG_CHECK(ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D);

	D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc;

	SrvDesc.Format = ResourceDesc.Format;
	SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	if (ResourceDesc.SampleDesc.Count > 1)
	{
		SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SrvDesc.Texture2D.MipLevels = ResourceDesc.MipLevels;
		SrvDesc.Texture2D.PlaneSlice = 0;
		SrvDesc.Texture2D.ResourceMinLODClamp = 0.f;
		SrvDesc.Texture2D.MostDetailedMip = MaxMipLevel;
	}

	Device->CreateShaderResourceView(m_Resource.Get(), &SrvDesc, m_SrvDescriptor.GetCPUHandle());
}

void aZero::D3D12::GPUResource_Deprecated::CreateBufferSRV(D3D12::DescriptorManager& DescriptorManager, int ElementSizeBytes, int NumElements, int FirstElementIndex)
{
	DEBUG_CHECK(m_Resource);
	m_DescriptorManager = &DescriptorManager;

	ID3D12Device* Device = nullptr;
	m_Resource->GetDevice(IID_PPV_ARGS(&Device));

	if (m_SrvDescriptor.GetHeapIndex() == -1)
	{
		m_SrvDescriptor = DescriptorManager.GetResourceHeap().GetDescriptor();
	}

	const D3D12_RESOURCE_DESC& ResourceDesc = m_Resource->GetDesc();
	DEBUG_CHECK(ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER);

	D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc;
	SrvDesc.Buffer.FirstElement = FirstElementIndex;
	SrvDesc.Buffer.NumElements = NumElements;
	SrvDesc.Buffer.StructureByteStride = ElementSizeBytes;
	SrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	SrvDesc.Shader4ComponentMapping = D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0;
	SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;

	Device->CreateShaderResourceView(m_Resource.Get(), &SrvDesc, m_SrvDescriptor.GetCPUHandle());
}

void aZero::D3D12::GPUResource_Deprecated::CreateTextureUAV(D3D12::DescriptorManager& DescriptorManager, int MipIndex)
{
	DEBUG_CHECK(m_Resource);
	m_DescriptorManager = &DescriptorManager;

	ID3D12Device* Device = nullptr;
	m_Resource->GetDevice(IID_PPV_ARGS(&Device));

	if (m_UavDescriptor.GetHeapIndex() == -1)
	{
		m_UavDescriptor = DescriptorManager.GetResourceHeap().GetDescriptor();
	}

	const D3D12_RESOURCE_DESC& ResourceDesc = m_Resource->GetDesc();
	DEBUG_CHECK(ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D);

	D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc;
	UavDesc.Format = ResourceDesc.Format;
	UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

	if (ResourceDesc.SampleDesc.Count > 1)
	{
		UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		UavDesc.Texture2D.MipSlice = MipIndex;
		UavDesc.Texture2D.PlaneSlice = 0;
	}

	Device->CreateUnorderedAccessView(GetResource(), nullptr, &UavDesc, m_UavDescriptor.GetCPUHandle());
}

void aZero::D3D12::GPUResource_Deprecated::CreateBufferUAV(D3D12::DescriptorManager& DescriptorManager, int ElementSizeBytes, int NumElements, int FirstElementIndex)
{
	DEBUG_CHECK(m_Resource);
	m_DescriptorManager = &DescriptorManager;

	ID3D12Device* Device = nullptr;
	m_Resource->GetDevice(IID_PPV_ARGS(&Device));

	if (m_UavDescriptor.GetHeapIndex() == -1)
	{
		m_UavDescriptor = DescriptorManager.GetResourceHeap().GetDescriptor();
	}

	const D3D12_RESOURCE_DESC& ResourceDesc = m_Resource->GetDesc();
	DEBUG_CHECK(ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER);

	D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc;
	UavDesc.Format = ElementSizeBytes == 0 ? ResourceDesc.Format : DXGI_FORMAT_UNKNOWN;
	UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	UavDesc.Buffer.FirstElement = FirstElementIndex;
	UavDesc.Buffer.NumElements = NumElements;
	UavDesc.Buffer.StructureByteStride = ElementSizeBytes;
	UavDesc.Buffer.CounterOffsetInBytes = 0;
	UavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	UavDesc.Texture2D.PlaneSlice = 0;

	if (ResourceDesc.SampleDesc.Count > 1)
	{
		UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	}

	Device->CreateUnorderedAccessView(GetResource(), nullptr, &UavDesc, m_UavDescriptor.GetCPUHandle());
}

void aZero::D3D12::GPUResource_Deprecated::CreateTextureRTV(D3D12::DescriptorManager& DescriptorManager, int MipIndex)
{
	DEBUG_CHECK(m_Resource);
	m_DescriptorManager = &DescriptorManager;

	ID3D12Device* Device = nullptr;
	m_Resource->GetDevice(IID_PPV_ARGS(&Device));

	if (m_DsvRtvDescriptor.GetHeapIndex() == -1)
	{
		m_DsvRtvDescriptor = DescriptorManager.GetRtvHeap().GetDescriptor();
	}

	const D3D12_RESOURCE_DESC& ResourceDesc = m_Resource->GetDesc();
	DEBUG_CHECK(ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D);

	D3D12_RENDER_TARGET_VIEW_DESC RtvDesc;
	RtvDesc.Format = ResourceDesc.Format;

	if (ResourceDesc.SampleDesc.Count > 1)
	{
		RtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		RtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		RtvDesc.Texture2D.MipSlice = MipIndex;
		RtvDesc.Texture2D.PlaneSlice = 0;
	}

	Device->CreateRenderTargetView(m_Resource.Get(), &RtvDesc, m_DsvRtvDescriptor.GetCPUHandle());
}

void aZero::D3D12::GPUResource_Deprecated::CreateTextureDSV(D3D12::DescriptorManager& DescriptorManager, int MipIndex)
{
	DEBUG_CHECK(m_Resource);
	m_DescriptorManager = &DescriptorManager;

	ID3D12Device* Device = nullptr;
	m_Resource->GetDevice(IID_PPV_ARGS(&Device));

	if (m_DsvRtvDescriptor.GetHeapIndex() == -1)
	{
		m_DsvRtvDescriptor = DescriptorManager.GetDsvHeap().GetDescriptor();
	}

	const D3D12_RESOURCE_DESC& ResourceDesc = m_Resource->GetDesc();
	DEBUG_CHECK(ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D);

	D3D12_DEPTH_STENCIL_VIEW_DESC DsvDesc;
	DsvDesc.Format = ResourceDesc.Format;

	if (ResourceDesc.SampleDesc.Count > 1)
	{
		DsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		DsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		DsvDesc.Texture2D.MipSlice = MipIndex;
	}

	Device->CreateDepthStencilView(m_Resource.Get(), &DsvDesc, m_DsvRtvDescriptor.GetCPUHandle());
}

void aZero::D3D12::GPUResource_Deprecated::UploadToTexture(ID3D12GraphicsCommandList* CmdList, GPUResource_Deprecated& DstResource, const std::vector<D3D12_SUBRESOURCE_DATA>& SubresourceData)
{
	//DEBUG_CHECK(DstResource.m_Resource->GetDesc().Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D);

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(DstResource.m_Resource.Get(), 0, 1);

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

	auto desc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	ID3D12Device* Device = nullptr;
	CmdList->GetDevice(IID_PPV_ARGS(&Device));

	Microsoft::WRL::ComPtr<ID3D12Resource> StagingBuffer;
	Device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(StagingBuffer.GetAddressOf()));

	UpdateSubresources(CmdList, DstResource.m_Resource.Get(), StagingBuffer.Get(),
		0, 0, SubresourceData.size(), SubresourceData.data());

	DstResource.m_ResourceRecycler->RecycleResource(StagingBuffer);
}

void aZero::D3D12::GPUResource_Deprecated::CopyBufferToBuffer(ID3D12GraphicsCommandList* CmdList, GPUResource_Deprecated& DstResource, int DstOffset, GPUResource_Deprecated& SrcResource, int SrcOffset, int NumBytes, const void* OptionalData, int OptionalDataOffset)
{
	if (DstResource.m_AccessType == RESOURCE_ACCESS_TYPE::GPU_ONLY || SrcResource.m_AccessType == RESOURCE_ACCESS_TYPE::GPU_ONLY)
	{
		if (OptionalData && (SrcResource.m_AccessType == RESOURCE_ACCESS_TYPE::CPU_WRITE))
		{
			memcpy((char*)SrcResource.m_MappedPtr + SrcOffset, (char*)OptionalData + OptionalDataOffset, NumBytes);
		}
		//DEBUG_CHECK(CmdList);
		CmdList->CopyBufferRegion(DstResource.GetResource(), DstOffset, SrcResource.GetResource(), SrcOffset, NumBytes);
	}
	else
	{
		DEBUG_CHECK(DstResource.m_MappedPtr);
		DEBUG_CHECK(SrcResource.m_MappedPtr);

		if (OptionalData)
		{
			memcpy((char*)SrcResource.m_MappedPtr + SrcOffset, OptionalData, NumBytes);
		}

		memcpy((char*)DstResource.m_MappedPtr + DstOffset, (char*)SrcResource.m_MappedPtr + SrcOffset, NumBytes);
	}
}

void aZero::D3D12::GPUResource_Deprecated::CopyTextureToTexture(ID3D12GraphicsCommandList* CmdList, GPUResource_Deprecated& DstResource, const DXM::Vector2& DstOffset, GPUResource_Deprecated& SrcResource, const DXM::Vector2& SrcOffset)
{
	const D3D12_RESOURCE_DESC& DstResourceDesc = DstResource.m_Resource->GetDesc();
	const D3D12_RESOURCE_DESC& SrcResourceDesc = SrcResource.m_Resource->GetDesc();

	if (
		DstResourceDesc.Width == SrcResourceDesc.Width 
		&& 
		DstResourceDesc.Height == SrcResourceDesc.Height
		)
	{
		GPUResource_Deprecated::TransitionState(CmdList, DstResource.m_Resource.Get(), DstResource.m_ResourceState, D3D12_RESOURCE_STATE_COPY_DEST);
		GPUResource_Deprecated::TransitionState(CmdList, SrcResource.m_Resource.Get(), SrcResource.m_ResourceState, D3D12_RESOURCE_STATE_COPY_SOURCE);

		// NOTE - Copying causes the resources to go decay into D3D12_RESOURCE_STATE_COMMON once the GPU has executed the recorded command
		CmdList->CopyResource(DstResource.m_Resource.Get(), SrcResource.m_Resource.Get());
	}
	else
	{
		DEBUG_CHECK(false); // TODO - Fix texture copying with unequal dimensions
	}
}

void aZero::D3D12::GPUResource_Deprecated::TransitionState(ID3D12GraphicsCommandList* CmdList, GPUResource_Deprecated& Resource, D3D12_RESOURCE_STATES NewState)
{
	if (NewState != Resource.m_ResourceState)
	{
		D3D12_RESOURCE_BARRIER Barrier(CD3DX12_RESOURCE_BARRIER::Transition(Resource.m_Resource.Get(), Resource.m_ResourceState, NewState));
		CmdList->ResourceBarrier(1, &Barrier);
		Resource.m_ResourceState = NewState;
	}
}

void aZero::D3D12::GPUResource_Deprecated::TransitionState(ID3D12GraphicsCommandList* CmdList, ID3D12Resource* Resource, D3D12_RESOURCE_STATES OldState, D3D12_RESOURCE_STATES NewState)
{
	if (NewState != OldState)
	{
		D3D12_RESOURCE_BARRIER Barrier(CD3DX12_RESOURCE_BARRIER::Transition(Resource, OldState, NewState));
		CmdList->ResourceBarrier(1, &Barrier);
	}
}

void aZero::D3D12::GPUResource_Deprecated::TransitionState(ID3D12GraphicsCommandList* CmdList, std::vector<std::pair<D3D12_RESOURCE_STATES, GPUResource_Deprecated*>>& Resources)
{
	std::vector<D3D12_RESOURCE_BARRIER> Barriers;
	Barriers.reserve(Resources.size());
	for (auto& [NewState, Resource] : Resources)
	{
		D3D12_RESOURCE_STATES CurrentState = Resource->m_ResourceState;
		if (NewState != CurrentState)
		{
			Barriers.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(Resource->m_Resource.Get(), CurrentState, NewState));
			Resource->m_ResourceState = NewState;
		}
	}
	
	// TODO - Check that this still works even though "barriers" is local and might have been overwritten in memory before the commandlist is executed
	CmdList->ResourceBarrier(Barriers.size(), Barriers.data());
}