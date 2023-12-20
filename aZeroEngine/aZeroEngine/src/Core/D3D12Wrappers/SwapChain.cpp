#include "SwapChain.h"

aZero::D3D12::SwapChain::SwapChain(HWND windowHandle, const CommandQueue& graphicsQueue, DescriptorManager& descriptorManager, DXGI_FORMAT backBufferFormat)
	: m_descriptorManager(&descriptorManager), m_backBufferFormat(backBufferFormat)
{
	ID3D12Device* device = nullptr;
	if (FAILED(graphicsQueue.GetCommandQueue()->GetDevice(IID_PPV_ARGS(&device))))
	{
		throw;
	}

	if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(m_dxgiFactory.GetAddressOf()))))
	{
		throw;
	}

	// NOTE - Enable and maybe handle UMA vs NUMA?
	//D3D12_FEATURE_DATA_ARCHITECTURE memoryTypeInfo;
	//if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &memoryTypeInfo, sizeof(memoryTypeInfo))))
	//{
	//	throw;
	//}

	//bool isUMA = memoryTypeInfo.UMA;

	DXGI_SWAP_CHAIN_DESC1 scDesc;
	scDesc.Width = 0;
	scDesc.Height = 0;
	scDesc.Format = backBufferFormat;
	scDesc.Stereo = false;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferCount = 3;
	scDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	scDesc.Scaling = DXGI_SCALING_STRETCH;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	scDesc.Flags = (DXGI_SWAP_CHAIN_FLAG)(DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
	//scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY; // NOTE - Test it!

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {};
	DEVMODEA displayInfo;
	EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &displayInfo);

	fullscreenDesc.RefreshRate.Numerator = displayInfo.dmDisplayFrequency;
	fullscreenDesc.RefreshRate.Denominator = 1;
	fullscreenDesc.Windowed = true;
	fullscreenDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	if (FAILED(m_dxgiFactory->CreateSwapChainForHwnd(graphicsQueue.GetCommandQueue(), windowHandle,
		&scDesc, &fullscreenDesc, nullptr, m_swapChain.GetAddressOf())))
	{
		throw;
	}

	m_backBuffers.resize(3);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;
	rtvDesc.Format = m_backBufferFormat;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < 3; i++)
	{
		m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_backBuffers[i].m_resource.GetAddressOf()));
		m_backBuffers[i].m_descriptor = m_descriptorManager->GetRtvHeap().GetDescriptor();

		device->CreateRenderTargetView(m_backBuffers[i].m_resource.Get(), &rtvDesc, m_backBuffers[i].m_descriptor.GetCPUHandle());
	}
}

aZero::D3D12::SwapChain::~SwapChain()
{
	for (BackBuffer& backBuffer : m_backBuffers)
	{
		if (backBuffer.m_resource)
		{
			backBuffer.m_resource = nullptr;
			m_descriptorManager->GetRtvHeap().RecycleDescriptor(backBuffer.m_descriptor);
		}
	}
}

void aZero::D3D12::SwapChain::ResolveRenderSurface(ID3D12GraphicsCommandList* cmdList, int backBufferIndex, D3D12::Resource::Texture2D& renderSource)
{
	if (backBufferIndex > m_backBuffers.size())
	{
		throw;
	}

	D3D12::SwapChain::BackBuffer& backBuffer = m_backBuffers[backBufferIndex];

	D3D12::Resource::TransitionResource(cmdList, backBuffer.m_resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

	D3D12::Resource::CopyTextureToTexture(backBuffer.m_resource.Get(), renderSource, cmdList); // NOTE - Backbuffer auto-decays to COMMON/PRESENT

	D3D12::Resource::TransitionResource(cmdList, backBuffer.m_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
}

void aZero::D3D12::SwapChain::Present()
{
	m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
}
