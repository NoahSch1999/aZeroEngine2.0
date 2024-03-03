#include "RenderSystem.h"

void aZero::ECS::RenderSystem::CalculateVisibility(const Camera& camera, std::vector<Entity>& visibleEntities, const std::vector<Entity>& allEntities)
{
	visibleEntities = allEntities;
}

void aZero::ECS::RenderSystem::Render(const Camera& camera, const std::vector<Entity>& entitiesToRender)
{
	ExecuteDeferredGeometryPass(camera, entitiesToRender);
	ExecuteDeferredLightPass(m_LightPassRenderTextures[m_frameIndex], m_LightPassRenderTextureDescriptors[m_frameIndex]);
}

void aZero::ECS::RenderSystem::CreateSamplers()
{
	D3D12_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;

	m_anisotropic16Sampler = m_descriptorManager->GetSamplerHeap().GetDescriptor();
	m_device->CreateSampler(&samplerDesc, m_anisotropic16Sampler.GetCPUHandle());

	samplerDesc.MaxAnisotropy = 8;
	m_anisotropic8Sampler = m_descriptorManager->GetSamplerHeap().GetDescriptor();
	m_device->CreateSampler(&samplerDesc, m_anisotropic8Sampler.GetCPUHandle());

	samplerDesc.MaxAnisotropy = 4;
	m_anisotropic4Sampler = m_descriptorManager->GetSamplerHeap().GetDescriptor();
	m_device->CreateSampler(&samplerDesc, m_anisotropic4Sampler.GetCPUHandle());

	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	m_linearSampler = m_descriptorManager->GetSamplerHeap().GetDescriptor();
	m_device->CreateSampler(&samplerDesc, m_linearSampler.GetCPUHandle());

	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	m_pointSampler = m_descriptorManager->GetSamplerHeap().GetDescriptor();
	m_device->CreateSampler(&samplerDesc, m_pointSampler.GetCPUHandle());
}

void aZero::ECS::RenderSystem::CreateDeferredGeometryPass()
{

	// TODO - Load and prep shaders for render test
	m_shaderCache->CompileAndStore("VS_DeferredGeometryPass");
	m_shaderCache->CompileAndStore("PS_DeferredGeometryPass");
	D3D12::Shader* vGeoShader = m_shaderCache->GetShader("VS_DeferredGeometryPass");
	vGeoShader->AddParameter(D3D12::Shader::RootConstant("World", 0, 16));
	vGeoShader->AddParameter(D3D12::Shader::RootConstant("Camera", 1, 16));

	D3D12::Shader* pGeoShader = m_shaderCache->GetShader("PS_DeferredGeometryPass");
	pGeoShader->AddParameter(D3D12::Shader::RootConstant("MaterialConstants", 0, 8));
	pGeoShader->AddParameter(D3D12::Shader::RootConstant("SamplerSpecs", 1, 4));
	pGeoShader->AddRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
	pGeoShader->AddRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SNORM);
	pGeoShader->AddRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT);

	D3D12::GraphicsPass::PassDescription desc;
	desc.TopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.BlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	desc.DepthStencilDesc.Description = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	desc.DepthStencilDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.RasterDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	desc.RasterDesc.CullMode = D3D12_CULL_MODE_FRONT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.VertexShader = *m_shaderCache->GetShader("VS_DeferredGeometryPass");
	desc.PixelShader = *m_shaderCache->GetShader("PS_DeferredGeometryPass");

	// NOTE - Should be generated automatically with the shader compilation/reflection
	InputLayout layout;
	layout.AddElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	layout.AddElement("UV", DXGI_FORMAT_R32G32_FLOAT);
	layout.AddElement("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	layout.AddElement("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT);
	desc.InputLayoutDesc.pInputElementDescs = layout.GetDescription();
	desc.InputLayoutDesc.NumElements = layout.GetNumElements();

	m_DeferredGeometryPass = std::move(D3D12::GraphicsPass(m_device, desc));

	m_geometryPassDSVClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_geometryPassDSVClearValue.DepthStencil.Depth = 1;
	m_geometryPassDSVClearValue.DepthStencil.Stencil = 0;

	D3D12::TextureResourceDesc GeoDSVDesc;
	GeoDSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	GeoDSVDesc.Dimensions = m_renderResolution;
	GeoDSVDesc.InitialState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	GeoDSVDesc.MSSampleCount = 1;
	GeoDSVDesc.NumMipLevels = 1;
	GeoDSVDesc.UsageFlags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	GeoDSVDesc.ClearValue = &m_geometryPassDSVClearValue;

	m_geometryPassDSV = std::move(
		D3D12::GPUResource(
			m_device, 
			*m_resourceRecycler, 
			GeoDSVDesc));

	m_geometryPassDSVDescriptor = m_descriptorManager->GetDsvHeap().GetDescriptor();

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	m_device->CreateDepthStencilView(m_geometryPassDSV.GetResource(), &dsvDesc, m_geometryPassDSVDescriptor.GetCPUHandle());
}

void aZero::ECS::RenderSystem::CreateDeferredLightPass()
{
	m_shaderCache->CompileAndStore("VS_DeferredLightPass");
	m_shaderCache->CompileAndStore("PS_DeferredLightPass");
	D3D12::Shader* pLightShader = m_shaderCache->GetShader("PS_DeferredLightPass");
	pLightShader->AddParameter(D3D12::Shader::RootConstant("DescriptorIndices", 0, 4));
	pLightShader->AddRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);

	// Pass
	D3D12::GraphicsPass::PassDescription desc;
	desc.TopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.BlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	desc.DepthStencilDesc.Description = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	desc.DepthStencilDesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	desc.RasterDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	desc.RasterDesc.CullMode = D3D12_CULL_MODE_BACK;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.VertexShader = *m_shaderCache->GetShader("VS_DeferredLightPass");
	desc.PixelShader = *m_shaderCache->GetShader("PS_DeferredLightPass");

	InputLayout layout;
	layout.AddElement("POSITION", DXGI_FORMAT_R32G32_FLOAT);
	layout.AddElement("UV", DXGI_FORMAT_R32G32_FLOAT);
	desc.InputLayoutDesc.pInputElementDescs = layout.GetDescription();
	desc.InputLayoutDesc.NumElements = layout.GetNumElements();

	m_DeferredLightPass = std::move(D3D12::GraphicsPass(m_device, desc));

	//

	struct LocalData
	{
		DXM::Vector2 Position;
		DXM::Vector2 UV;
	};

	LocalData QuadVertices[6] =
	{
		{ {-1.f, 1.f},  {0.f, 0.f} },
		{ {1.f, 1.f},   {1.f, 0.f} },
		{ {-1.f, -1.f}, {0.f, 1.f} },
		{ {1.f, 1.f},   {1.f, 0.f} },
		{ {1.f, -1.f},  {1.f, 1.f} },
		{ {-1.f, -1.f}, {0.f, 1.f} }
	};

	D3D12::BufferResourceDesc ScreenQuadVBDesc;
	ScreenQuadVBDesc.AccessType = D3D12::RESOURCE_ACCESS_TYPE::GPU_ONLY;
	ScreenQuadVBDesc.NumBytes = ARRAYSIZE(QuadVertices) * sizeof(LocalData);
	m_lightPassQuadBuffer = std::move(
		D3D12::GPUResource(
			m_device,
			*m_resourceRecycler,
			ScreenQuadVBDesc)
	);

	ScreenQuadVBDesc.AccessType = D3D12::RESOURCE_ACCESS_TYPE::CPU_WRITE;
	D3D12::GPUResource tempUpload = std::move(
		D3D12::GPUResource(
			m_device,
			*m_resourceRecycler,
			ScreenQuadVBDesc)
	);

	m_directContext.StartRecording();

	D3D12::GPUResource::CopyBufferToBuffer(
		m_directContext.GetCommandList(),
		m_lightPassQuadBuffer, 0,
		tempUpload, 0,
		ARRAYSIZE(QuadVertices) * sizeof(LocalData),
		QuadVertices
	);

	m_directContext.StopRecording();
	m_directQueue->ExecuteContext({ m_directContext });

	m_lightPassVBV.BufferLocation = m_lightPassQuadBuffer.GetResource()->GetGPUVirtualAddress();
	m_lightPassVBV.SizeInBytes = ARRAYSIZE(QuadVertices) * sizeof(LocalData);
	m_lightPassVBV.StrideInBytes = sizeof(LocalData);

	D3D12::TextureResourceDesc FrameBufferDesc;
	FrameBufferDesc.Dimensions = m_renderResolution;
	FrameBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	FrameBufferDesc.InitialState = D3D12_RESOURCE_STATE_RENDER_TARGET;
	FrameBufferDesc.MSSampleCount = 1;
	FrameBufferDesc.NumMipLevels = 1;
	FrameBufferDesc.UsageFlags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	for (int i = 0; i < NUM_FRAMEBUFFERS; i++)
	{
		m_LightPassRenderTextures.emplace_back(std::move(
			D3D12::GPUResource(
				m_device,
				*m_resourceRecycler,
				FrameBufferDesc)
		));

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2D;

		m_LightPassRenderTextureDescriptors.emplace_back(m_descriptorManager->GetRtvHeap().GetDescriptor());

		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_device->CreateRenderTargetView(
			m_LightPassRenderTextures[i].GetResource(),
			&rtvDesc,
			m_LightPassRenderTextureDescriptors[i].GetCPUHandle());
	}
}

void aZero::ECS::RenderSystem::ExecuteDeferredGeometryPass(const Camera& camera, const std::vector<Entity>& entitiesToRender)
{
	m_directContext.StartRecording();
	ID3D12GraphicsCommandList* const cmdList = m_directContext.GetCommandList();
	m_DeferredGeometryPass.BeginPass(cmdList, m_descriptorManager->GetResourceHeap().GetDescriptorHeap(), m_descriptorManager->GetSamplerHeap().GetDescriptorHeap());

	D3D12::GPUResource::TransitionState(cmdList, m_GBuffers.GetGBuffer(GBuffers::BASECOLOR, m_frameIndex).RenderTexture, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET);
	D3D12::GPUResource::TransitionState(cmdList, m_GBuffers.GetGBuffer(GBuffers::WORLDNORMAL, m_frameIndex).RenderTexture, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET);
	D3D12::GPUResource::TransitionState(cmdList, m_GBuffers.GetGBuffer(GBuffers::WORLDPOSITION, m_frameIndex).RenderTexture, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET);

	// TODO - Fix clear color for gbuffers
	cmdList->ClearRenderTargetView(m_GBuffers.GetGBuffer(GBuffers::BASECOLOR, m_frameIndex).RenderTexture.GetDescriptorRTVDSV().GetCPUHandle(), m_GBuffers.GetGBuffer(GBuffers::BASECOLOR, m_frameIndex).ClearValue.Color, 0, nullptr);
	cmdList->ClearRenderTargetView(m_GBuffers.GetGBuffer(GBuffers::WORLDNORMAL, m_frameIndex).RenderTexture.GetDescriptorRTVDSV().GetCPUHandle(), m_GBuffers.GetGBuffer(GBuffers::WORLDNORMAL, m_frameIndex).ClearValue.Color, 0, nullptr);
	cmdList->ClearRenderTargetView(m_GBuffers.GetGBuffer(GBuffers::WORLDPOSITION, m_frameIndex).RenderTexture.GetDescriptorRTVDSV().GetCPUHandle(), m_GBuffers.GetGBuffer(GBuffers::WORLDPOSITION, m_frameIndex).ClearValue.Color, 0, nullptr);
	cmdList->ClearDepthStencilView(m_geometryPassDSVDescriptor.GetCPUHandle(), D3D12_CLEAR_FLAG_DEPTH, m_geometryPassDSVClearValue.DepthStencil.Depth, 0, 0, nullptr);

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_geometryPassDSVDescriptor.GetCPUHandle();
	m_DeferredGeometryPass.SetOutputTargets(cmdList,
		{
			m_GBuffers.GetGBuffer(GBuffers::BASECOLOR, m_frameIndex).RenderTexture.GetDescriptorRTVDSV().GetCPUHandle(),
			m_GBuffers.GetGBuffer(GBuffers::WORLDNORMAL, m_frameIndex).RenderTexture.GetDescriptorRTVDSV().GetCPUHandle(),
			m_GBuffers.GetGBuffer(GBuffers::WORLDPOSITION, m_frameIndex).RenderTexture.GetDescriptorRTVDSV().GetCPUHandle()
		},
		&handle);

	// Set pass params
	static double xxx = 0.0;
	xxx += 0.001;
	DXM::Matrix tempWorld = DXM::Matrix::CreateRotationY(xxx) /** DXM::Matrix::CreateRotationX(xxx) */ * DXM::Matrix::CreateTranslation(0, 0, 2.f);
	m_DeferredGeometryPass.SetShaderRootConstant<D3D12::ShaderTypes::VertexShader>(cmdList, "World", &tempWorld);
	DXM::Matrix tempCamera = camera.GetViewProjectionMatrix();
	m_DeferredGeometryPass.SetShaderRootConstant<D3D12::ShaderTypes::VertexShader>(cmdList, "Camera", &tempCamera);

	struct TempMaterialData
	{
		int albedo = -1;
		int normal = -1;
	};
	TempMaterialData mat;
	mat.albedo = m_TextureCache->Get("goblintexture").GetTexture().GetBindlessIndexSRV();
	mat.normal = m_TextureCache->Get("Body_NM").GetTexture().GetBindlessIndexSRV();

	m_DeferredGeometryPass.SetShaderRootConstant<D3D12::ShaderTypes::PixelShader>(cmdList, "MaterialConstants", &mat);
	int samplerIndex = m_anisotropic16Sampler.GetHeapIndex();
	m_DeferredGeometryPass.SetShaderRootConstant<D3D12::ShaderTypes::PixelShader>(cmdList, "SamplerSpecs", &samplerIndex);

	D3D12_RECT scizzorRect = {};
	ZeroMemory(&scizzorRect, sizeof(D3D12_RECT));
	scizzorRect.bottom = m_renderResolution.y;
	scizzorRect.right = m_renderResolution.x;

	D3D12_VIEWPORT viewport = {};
	ZeroMemory(&viewport, sizeof(D3D12_VIEWPORT));
	viewport.Height = m_renderResolution.y;
	viewport.Width = m_renderResolution.x;
	viewport.MaxDepth = 1.f;

	cmdList->RSSetScissorRects(1, &scizzorRect);
	cmdList->RSSetViewports(1, &viewport);

	m_DeferredGeometryPass.SetIndexBuffer(cmdList, m_MeshCache->Get("goblin").GetIndexBufferView(), 0, 1);
	m_DeferredGeometryPass.SetVertexBuffer(cmdList, m_MeshCache->Get("goblin").GetVertexBufferView(), 0, 1);

	m_DeferredGeometryPass.DrawIndexedInstanced(cmdList, m_MeshCache->Get("goblin").GetNumIndices(), 1, 0, 0, 0);

	// Render each entity
	for (const Entity& entity : entitiesToRender)
	{
		// Set per-draw params

		//m_GeometryPass.SetVertexBuffer();

		m_DeferredGeometryPass.DrawInstanced(cmdList, 0, 0, 0, 0);
	}

	m_directContext.StopRecording();
	m_directQueue->ExecuteContext({ m_directContext });
}

void aZero::ECS::RenderSystem::ExecuteDeferredLightPass(D3D12::GPUResource& RenderTarget, const D3D12::Descriptor& RenderTargetDescriptor)
{
	m_directContext.StartRecording();
	ID3D12GraphicsCommandList* const cmdList = m_directContext.GetCommandList();

	m_DeferredLightPass.BeginPass(cmdList, m_descriptorManager->GetResourceHeap().GetDescriptorHeap(), m_descriptorManager->GetSamplerHeap().GetDescriptorHeap());

	D3D12::GPUResource::TransitionState(cmdList, m_GBuffers.GetGBuffer(GBuffers::BASECOLOR, m_frameIndex).RenderTexture, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	D3D12::GPUResource::TransitionState(cmdList, m_GBuffers.GetGBuffer(GBuffers::WORLDNORMAL, m_frameIndex).RenderTexture, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	D3D12::GPUResource::TransitionState(cmdList, m_GBuffers.GetGBuffer(GBuffers::WORLDPOSITION, m_frameIndex).RenderTexture, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	D3D12::GPUResource::TransitionState(cmdList, RenderTarget, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_DeferredLightPass.SetOutputTargets(cmdList, { RenderTargetDescriptor.GetCPUHandle() }, nullptr);

	struct PSIndices
	{
		int samplerIndex;
		int baseColorIndex;
		int worldNormalIndex;
		int worldPositionIndex;
	};
	PSIndices psIndices;
	psIndices.samplerIndex = m_anisotropic16Sampler.GetHeapIndex();
	psIndices.baseColorIndex = m_GBuffers.GetGBuffer(GBuffers::BASECOLOR, m_frameIndex).RenderTexture.GetBindlessIndexSRV();
	psIndices.worldNormalIndex = m_GBuffers.GetGBuffer(GBuffers::WORLDNORMAL, m_frameIndex).RenderTexture.GetBindlessIndexSRV();
	psIndices.worldPositionIndex = m_GBuffers.GetGBuffer(GBuffers::WORLDPOSITION, m_frameIndex).RenderTexture.GetBindlessIndexSRV();

	m_DeferredLightPass.SetShaderRootConstant<D3D12::ShaderTypes::PixelShader>(cmdList, "DescriptorIndices", &psIndices);

	D3D12_RECT scizzorRect = {};
	ZeroMemory(&scizzorRect, sizeof(D3D12_RECT));
	scizzorRect.bottom = m_renderResolution.y;
	scizzorRect.right = m_renderResolution.x;

	D3D12_VIEWPORT viewport = {};
	ZeroMemory(&viewport, sizeof(D3D12_VIEWPORT));
	viewport.Height = m_renderResolution.y;
	viewport.Width = m_renderResolution.x;
	viewport.MaxDepth = 1.f;

	cmdList->RSSetScissorRects(1, &scizzorRect);
	cmdList->RSSetViewports(1, &viewport);

	m_DeferredLightPass.SetVertexBuffer(cmdList, m_lightPassVBV, 0, 1);

	m_DeferredLightPass.DrawInstanced(cmdList, 6, 1, 0, 0);

	m_directContext.StopRecording();
	m_directQueue->ExecuteContext({ m_directContext });
}

aZero::ECS::RenderSystem::RenderSystem(ComponentManager& componentManager,
	/*temp...*/ShaderCache* const shaderCache, MeshCache* MeshCache, TextureCache* TextureCache,
	D3D12::DescriptorManager* descriptorManager,
	D3D12::CommandQueue* commandQueue, D3D12::ResourceRecycler* resourceRecycler, const DXM::Vector2& renderResolution)
	:System(componentManager),
	/*temp...*/
	m_descriptorManager(descriptorManager),
	m_directQueue(commandQueue),
	m_resourceRecycler(resourceRecycler),
	m_shaderCache(shaderCache), m_MeshCache(MeshCache), m_TextureCache(TextureCache),
	m_renderResolution(renderResolution)
{
	ID3D12Device* device = nullptr;
	commandQueue->GetCommandQueue()->GetDevice(IID_PPV_ARGS(&device));
	m_device = device;

	// Signature Setup
	m_componentMask.set(m_componentManager.GetComponentBit<Component::Transform>());
	m_componentMask.set(m_componentManager.GetComponentBit<Component::Mesh>());
	m_componentMask.set(m_componentManager.GetComponentBit<Component::Material>());

	m_directContext = std::move(D3D12::CommandContext(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT));

	m_GBuffers = std::move(GBuffers(m_device, *m_resourceRecycler, *m_descriptorManager, m_renderResolution, NUM_FRAMEBUFFERS));
	CreateDeferredGeometryPass();
	CreateDeferredLightPass();

	CreateSamplers();

	// Temp
	m_directContext.StartRecording();

	m_MeshCache->LoadFromFile("C:/Programming/aZeroEngine2.0/aZeroEngine/Assets/Meshes/goblin.fbx", m_directContext.GetCommandList());
	m_TextureCache->LoadFromFile("C:/Programming/aZeroEngine2.0/aZeroEngine/Assets/Textures/goblintexture.png", m_directContext.GetCommandList());
	m_TextureCache->LoadFromFile("C:/Programming/aZeroEngine2.0/aZeroEngine/Assets/Textures/Body_NM.png", m_directContext.GetCommandList());

	m_directContext.StopRecording();
	m_directQueue->ExecuteContext({ m_directContext });
	m_directQueue->FlushCommands();
	// Temp

}
