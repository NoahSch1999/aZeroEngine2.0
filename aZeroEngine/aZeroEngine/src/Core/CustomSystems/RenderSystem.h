#pragma once
#include "../../ECS/System.h"
#include "../ComponentTypes/ComponentTypes.h"
#include "../../Helpers/NoneCopyable.h"
#include "../Camera.h"
#include "../D3D12Wrappers/Pipeline/Graphics/GraphicsPass.h"
#include "../D3D12Wrappers/Descriptors/DescriptorHeap.h"
#include "../D3D12Wrappers/Commands/CommandQueue.h"
#include "../D3D12Wrappers/Resources/ResourceFreeFunctions.h"
#include "../AssetTypes/MeshAsset.h"

namespace aZero
{
	namespace ECS
	{
		class RenderSystem : public System, public Helpers::NoneCopyable
		{
		private:
			void CreateSamplers()
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

			void ExecuteGeometryPass(const std::vector<Entity>& entitiesToRender)
			{
				m_directContext.StartRecording();
				ID3D12GraphicsCommandList* const cmdList = m_directContext.GetCommandList();
				m_GeometryPass.BeginPass(cmdList, m_descriptorManager->GetResourceHeap().GetDescriptorHeap(), m_descriptorManager->GetSamplerHeap().GetDescriptorHeap());

				D3D12::Resource::TransitionResource(cmdList, m_renderTargetResources[m_frameIndex], D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET);

				cmdList->ClearRenderTargetView(m_renderTargetDescriptors[m_frameIndex].GetCPUHandle(), m_rtClearValue.Color, 0, nullptr);
				cmdList->ClearDepthStencilView(m_depthStencilDescriptor.GetCPUHandle(), D3D12_CLEAR_FLAG_DEPTH, 0, 0, 0, nullptr);

				m_GeometryPass.SetOutputTargets(cmdList, { m_renderTargetDescriptors[m_frameIndex].GetCPUHandle() }, m_depthStencilDescriptor.GetCPUHandle());

				// Set pass params
				DXM::Matrix tempWorld = /*DXM::Matrix::CreateScale(2.0) **/ DXM::Matrix::CreateTranslation(0, 0, 10.f);
				m_GeometryPass.SetShaderRootConstant<D3D12::ShaderTypes::VertexShader>(cmdList, "World", &tempWorld);
				DXM::Matrix tempCamera = DXM::Matrix::CreatePerspectiveFieldOfView(3.14 / 2.f, 800.0 / 600.0, 0.1f, 100.f);
				m_GeometryPass.SetShaderRootConstant<D3D12::ShaderTypes::VertexShader>(cmdList, "Camera", &tempCamera);

				struct TempMaterialData
				{
					int albedo = -1;
					int normal = -1;
				};
				TempMaterialData mat;

				m_GeometryPass.SetShaderRootConstant<D3D12::ShaderTypes::PixelShader>(cmdList, "MaterialConstants", &mat);
				int samplerIndex = m_anisotropic16Sampler.GetHeapIndex();
				m_GeometryPass.SetShaderRootConstant<D3D12::ShaderTypes::PixelShader>(cmdList, "SamplerSpecs", &samplerIndex);

				m_GeometryPass.DrawInstanced(cmdList, testMesh.GetNumVertices(), 1, 0, 0);

				// Render each entity
				for (const Entity& entity : entitiesToRender)
				{
					// Set per-draw params
					
					//m_GeometryPass.SetVertexBuffer();

					m_GeometryPass.DrawInstanced(cmdList, 0, 0, 0, 0);
				}

				m_directContext.StopRecording();
				m_directQueue->ExecuteContext({ m_directContext });
			}

			void ExecuteLightPass()
			{

			}

			// TODO - Implement culling techniques used here (frustrum, octree etc...)
			void CalculateVisibility(std::vector<Entity>& visibleEntities, const std::vector<Entity>& allEntities)
			{
				visibleEntities = allEntities;
			}

			void Render(const std::vector<Entity>& entitiesToRender)
			{
				ExecuteGeometryPass(entitiesToRender);
				ExecuteLightPass();
			}

		public:
			// TODO - Replace, only temp
			Camera* m_activeCamera = nullptr;
			D3D12::GraphicsPass m_GeometryPass;
			ID3D12Device* m_device = nullptr;
			D3D12::CommandQueue* m_directQueue = nullptr;
			D3D12::CommandContext m_directContext;
			
			D3D12::Resource::Texture2D m_renderTargetResources[3];
			D3D12::Descriptor m_renderTargetDescriptors[3];
			D3D12::Resource::Texture2D m_depthStencil;
			D3D12::Descriptor m_depthStencilDescriptor;

			D3D12_CLEAR_VALUE m_rtClearValue;
			D3D12::ResourceRecycler* m_resourceRecycler = nullptr;
			int m_frameIndex = 0;

			D3D12::DescriptorManager* m_descriptorManager = nullptr;

			D3D12::Descriptor m_anisotropic4Sampler;
			D3D12::Descriptor m_anisotropic8Sampler;
			D3D12::Descriptor m_anisotropic16Sampler;
			D3D12::Descriptor m_linearSampler;
			D3D12::Descriptor m_pointSampler;

			Asset::Mesh testMesh;
			//

			RenderSystem(ComponentManager& componentManager, 
				/*temp...*/ID3D12Device* const device, D3D12::Shader* vShader, D3D12::Shader* pShader,
				D3D12::DescriptorManager* descriptorManager,
				D3D12::CommandQueue* commandQueue, D3D12::ResourceRecycler* resourceRecycler, const DXM::Vector2& renderSurfaceDimensions)
				:System(componentManager), 
				/*temp...*/m_device(device), m_descriptorManager(descriptorManager), m_directQueue(commandQueue), m_resourceRecycler(resourceRecycler)
			{
				// Signature Setup
				m_componentMask.set(m_componentManager.GetComponentBit<Component::Transform>());
				m_componentMask.set(m_componentManager.GetComponentBit<Component::Mesh>());
				m_componentMask.set(m_componentManager.GetComponentBit<Component::Material>());

				// Setup test pass
				D3D12::GraphicsPass::PassDescription desc;
				desc.TopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				desc.BlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
				desc.DepthStencilDesc.Description = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
				desc.DepthStencilDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
				desc.RasterDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.VertexShader = *vShader;
				desc.PixelShader = *pShader;

				// NOTE - Should be generated automatically with the shader compilation/reflection
				InputLayout layout;
				layout.AddElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
				layout.AddElement("UV", DXGI_FORMAT_R32G32_FLOAT);
				layout.AddElement("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
				layout.AddElement("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT);
				desc.InputLayoutDesc.pInputElementDescs = layout.GetDescription();
				desc.InputLayoutDesc.NumElements = layout.GetNumElements();

				m_GeometryPass = std::move(D3D12::GraphicsPass(m_device, desc));

				m_directContext = std::move(D3D12::CommandContext(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT));

				m_rtClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				m_rtClearValue.DepthStencil.Depth = 0;
				m_rtClearValue.DepthStencil.Stencil = 0;
				m_depthStencil = std::move(D3D12::Resource::Texture2D(*m_resourceRecycler, m_device, DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT,
					D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, renderSurfaceDimensions, 1, 1, &m_rtClearValue, D3D12_RESOURCE_STATE_DEPTH_WRITE));
				m_depthStencilDescriptor = m_descriptorManager->GetDsvHeap().GetDescriptor();
				D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
				dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				dsvDesc.Texture2D.MipSlice = 0;
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
				m_device->CreateDepthStencilView(m_depthStencil.GetResource(), &dsvDesc, m_depthStencilDescriptor.GetCPUHandle());

				m_rtClearValue.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
				m_rtClearValue.Color[0] = 0.5;
				m_rtClearValue.Color[1] = 0.5;
				m_rtClearValue.Color[2] = 0.5;
				m_rtClearValue.Color[3] = 0;

				D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
				rtvDesc.Texture2D.MipSlice = 0;
				rtvDesc.Texture2D.PlaneSlice = 0;
				rtvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2D;
				for (int i = 0; i < 3; i++)
				{
					m_renderTargetResources[i] = std::move(D3D12::Resource::Texture2D(*m_resourceRecycler, m_device, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
						D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, renderSurfaceDimensions, 1, 1, &m_rtClearValue));

					m_renderTargetDescriptors[i] = m_descriptorManager->GetRtvHeap().GetDescriptor();
					m_device->CreateRenderTargetView(m_renderTargetResources[i].GetResource(), &rtvDesc, m_renderTargetDescriptors[i].GetCPUHandle());
				}

				// Temp
				HelperFunctions::FBXFileInfo fbxInfo;
				HelperFunctions::LoadFBXFile(fbxInfo, "C:/Programming/aZeroEngine2.0/aZeroEngine/Assets/Meshes/cube");
				testMesh = std::move(Asset::Mesh(device, fbxInfo.Meshes[0].Vertices.size(), sizeof(HelperFunctions::BasicVertex), *resourceRecycler));;

				m_directContext.StartRecording();
				testMesh.Upload(m_directContext.GetCommandList(), fbxInfo.Meshes[0].Vertices.data(), fbxInfo.Meshes[0].Vertices.size(), sizeof(HelperFunctions::BasicVertex), 16, *resourceRecycler);
				m_directContext.StopRecording();
				m_directQueue->ExecuteContext({ m_directContext });
				m_directQueue->FlushCommands();
				// Temp

				CreateSamplers();
			}

			D3D12::Resource::Texture2D& GetFrameRendertarget()
			{
				return m_renderTargetResources[m_frameIndex];
			}

			void BeginFrame(int frameIndex)
			{
				m_frameIndex = frameIndex;

				if (m_frameIndex % 3 == 0)
				{
					m_directContext.FreeCommandBuffer();
				}
			}

			virtual void Update() override
			{
				if (!m_activeCamera)
				{
					std::vector<Entity> entitiesToRender;
					CalculateVisibility(entitiesToRender, m_entities.GetConstReferenceInternal());
					Render(entitiesToRender);
				}
			}

			virtual void OnRegister() override
			{
				printf("Entity registered for RenderSystem\n");
			}

			virtual void OnUnRegister() override
			{
				printf("Entity unregistered for RenderSystem\n");
			}
		};
	}
}