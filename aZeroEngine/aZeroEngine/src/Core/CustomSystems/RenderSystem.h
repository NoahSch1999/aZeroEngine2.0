#pragma once
#include "../../ECS/System.h"
#include "../ComponentTypes/ComponentTypes.h"
#include "../../Helpers/NoneCopyable.h"
#include "../Camera.h"
#include "../D3D12Wrappers/Pipeline/Graphics/GraphicsPass.h"
#include "../D3D12Wrappers/Descriptors/DescriptorHeap.h"
#include "../D3D12Wrappers/Commands/CommandQueue.h"
#include "../D3D12Wrappers/Resources/GPUResource.h"
#include "../AssetTypes/MeshAsset.h"
#include "../Caches/ShaderCache.h"
#include "../Caches/MeshCache.h"
#include "../Caches/TextureCache.h"
#include "../Scene.h"

namespace aZero
{
	namespace ECS
	{
		class RenderSystem : public System, public Helpers::NoneCopyable
		{
		private:
			class GBuffers
			{
			public:
				enum GBUFFERINDEX{ BASECOLOR, WORLDNORMAL, WORLDPOSITION, MAXVALUE };

			private:
				struct GBuffer
				{
					D3D12::GPUResource RenderTexture;
					D3D12_CLEAR_VALUE ClearValue;
				};
				std::vector<std::vector<GBuffer>> m_GBuffers;

				void CreateTextures(ID3D12Device* const device, 
					D3D12::ResourceRecycler& resourceRecycler, 
					const DXM::Vector2& renderSurfaceDimensions, 
					std::vector<GBuffer>& gBuffers)
				{
					// BASE COLOR
					gBuffers[BASECOLOR].ClearValue.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
					gBuffers[BASECOLOR].ClearValue.Color[0] = 0.5f;
					gBuffers[BASECOLOR].ClearValue.Color[1] = 0.5f;
					gBuffers[BASECOLOR].ClearValue.Color[2] = 0.5f;
					gBuffers[BASECOLOR].ClearValue.Color[3] = 0.0f;

					D3D12::TextureResourceDesc GBufferDesc;
					GBufferDesc.ClearValue = &gBuffers[BASECOLOR].ClearValue;
					GBufferDesc.Dimensions = renderSurfaceDimensions;
					GBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					GBufferDesc.UsageFlags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
					GBufferDesc.MSSampleCount = 1;
					GBufferDesc.NumMipLevels = 1;
					GBufferDesc.InitialState = D3D12_RESOURCE_STATE_RENDER_TARGET;

					gBuffers[BASECOLOR].RenderTexture =
						std::move(D3D12::GPUResource(
							device,
							resourceRecycler,
							GBufferDesc));
					//

					// WORLD NORMAL
					gBuffers[WORLDNORMAL].ClearValue.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SNORM;
					gBuffers[WORLDNORMAL].ClearValue.Color[0] = 0.0f;
					gBuffers[WORLDNORMAL].ClearValue.Color[1] = 0.0f;
					gBuffers[WORLDNORMAL].ClearValue.Color[2] = 0.0f;
					gBuffers[WORLDNORMAL].ClearValue.Color[3] = 0.0f;

					GBufferDesc.ClearValue = &gBuffers[WORLDNORMAL].ClearValue;
					GBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_SNORM;
					gBuffers[WORLDNORMAL].RenderTexture =
						std::move(D3D12::GPUResource(
							device,
							resourceRecycler,
							GBufferDesc
						));
					//

					// WORLD POSITION
					gBuffers[WORLDPOSITION].ClearValue.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
					gBuffers[WORLDPOSITION].ClearValue.Color[0] = 0.0f;
					gBuffers[WORLDPOSITION].ClearValue.Color[1] = 0.0f;
					gBuffers[WORLDPOSITION].ClearValue.Color[2] = 0.0f;
					gBuffers[WORLDPOSITION].ClearValue.Color[3] = 0.0f;

					GBufferDesc.ClearValue = &gBuffers[WORLDPOSITION].ClearValue;
					GBufferDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

					gBuffers[WORLDPOSITION].RenderTexture =
						std::move(D3D12::GPUResource(
							device,
							resourceRecycler,
							GBufferDesc));
					//
				}

				void CreateDescriptors(ID3D12Device* const device, 
					D3D12::DescriptorManager& descriptorManager, 
					std::vector<GBuffer>& gBuffers)
				{
					gBuffers[BASECOLOR].RenderTexture.CreateTextureRTV(descriptorManager, 0);
					gBuffers[WORLDNORMAL].RenderTexture.CreateTextureRTV(descriptorManager);
					gBuffers[WORLDPOSITION].RenderTexture.CreateTextureRTV(descriptorManager);

					gBuffers[BASECOLOR].RenderTexture.CreateTextureSRV(descriptorManager);
					gBuffers[WORLDNORMAL].RenderTexture.CreateTextureSRV(descriptorManager);
					gBuffers[WORLDPOSITION].RenderTexture.CreateTextureSRV(descriptorManager);


					/*D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
					rtvDesc.Texture2D.MipSlice = 0;
					rtvDesc.Texture2D.PlaneSlice = 0;
					rtvDesc.ViewDimension = D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2D;

					D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
					srvDesc.Texture2D.MostDetailedMip = 0;
					srvDesc.Texture2D.MipLevels = 1;
					srvDesc.Texture2D.PlaneSlice = 0;
					srvDesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
					srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;*/

					// BASE COLOR
					/*rtvDesc.Format = gBuffers[BASECOLOR].ClearValue.Format;
					gBuffers[BASECOLOR].RtvDescriptor = descriptorManager.GetRtvHeap().GetDescriptor();
					device->CreateRenderTargetView(
						gBuffers[BASECOLOR].RenderTexture.GetResource(),
						&rtvDesc, 
						gBuffers[BASECOLOR].RtvDescriptor.GetCPUHandle());

					srvDesc.Format = gBuffers[BASECOLOR].ClearValue.Format;
					gBuffers[BASECOLOR].SrvDescriptor = descriptorManager.GetResourceHeap().GetDescriptor();
					device->CreateShaderResourceView(
						gBuffers[BASECOLOR].RenderTexture.GetResource(),
						&srvDesc,
						gBuffers[BASECOLOR].SrvDescriptor.GetCPUHandle()
					);*/
					//

					//// WORLD NORMAL
					//rtvDesc.Format = gBuffers[WORLDNORMAL].ClearValue.Format;
					//gBuffers[WORLDNORMAL].RtvDescriptor = descriptorManager.GetRtvHeap().GetDescriptor();
					//device->CreateRenderTargetView(
					//	gBuffers[WORLDNORMAL].RenderTexture.GetResource(),
					//	&rtvDesc,
					//	gBuffers[WORLDNORMAL].RtvDescriptor.GetCPUHandle());

					//srvDesc.Format = gBuffers[WORLDNORMAL].ClearValue.Format;
					//gBuffers[WORLDNORMAL].SrvDescriptor = descriptorManager.GetResourceHeap().GetDescriptor();
					//device->CreateShaderResourceView(
					//	gBuffers[WORLDNORMAL].RenderTexture.GetResource(),
					//	&srvDesc,
					//	gBuffers[WORLDNORMAL].SrvDescriptor.GetCPUHandle()
					//);
					//

					// WORLD POSITION
					/*rtvDesc.Format = gBuffers[WORLDPOSITION].ClearValue.Format;
					gBuffers[WORLDPOSITION].RtvDescriptor = descriptorManager.GetRtvHeap().GetDescriptor();
					device->CreateRenderTargetView(
						gBuffers[WORLDPOSITION].RenderTexture.GetResource(),
						&rtvDesc,
						gBuffers[WORLDPOSITION].RtvDescriptor.GetCPUHandle());

					srvDesc.Format = gBuffers[WORLDPOSITION].ClearValue.Format;
					gBuffers[WORLDPOSITION].SrvDescriptor = descriptorManager.GetResourceHeap().GetDescriptor();
					device->CreateShaderResourceView(
						gBuffers[WORLDPOSITION].RenderTexture.GetResource(),
						&srvDesc,
						gBuffers[WORLDPOSITION].SrvDescriptor.GetCPUHandle()
					);*/
					//

				}

			public:
				GBuffers() = default;

				GBuffers(ID3D12Device* const device, 
					D3D12::ResourceRecycler& resourceRecycler, 
					D3D12::DescriptorManager& descriptorManager,
					const DXM::Vector2& renderSurfaceDimensions, 
					int numFramesBeforeSync)
				{
					m_GBuffers.reserve(numFramesBeforeSync);
					for (int i = 0; i < numFramesBeforeSync; i++)
					{
						std::vector<GBuffer> frameGBuffers;
						
						frameGBuffers.resize(GBUFFERINDEX::MAXVALUE);
						CreateTextures(device, resourceRecycler, renderSurfaceDimensions, frameGBuffers);
						CreateDescriptors(device, descriptorManager, frameGBuffers);

						m_GBuffers.emplace_back(std::move(frameGBuffers));
					}
				}

				GBuffer& GetGBuffer(GBUFFERINDEX gBufferType, int frameIndex)
				{
					return m_GBuffers.at(frameIndex).at(gBufferType);
				}
			};

			// TODO - Replace, only temp
			ID3D12Device* m_device = nullptr;
			D3D12::CommandQueue* m_directQueue = nullptr;
			D3D12::CommandContext m_directContext;
			ShaderCache* m_shaderCache;
			MeshCache* m_MeshCache = nullptr;
			TextureCache* m_TextureCache = nullptr;

			D3D12::ResourceRecycler* m_resourceRecycler = nullptr;
			int m_frameIndex = 0;

			D3D12::DescriptorManager* m_descriptorManager = nullptr;

			D3D12::Descriptor m_anisotropic4Sampler;
			D3D12::Descriptor m_anisotropic8Sampler;
			D3D12::Descriptor m_anisotropic16Sampler;
			D3D12::Descriptor m_linearSampler;
			D3D12::Descriptor m_pointSampler;

			// Deferred Rendering
			GBuffers m_GBuffers;
			D3D12::GPUResource m_geometryPassDSV;
			D3D12::Descriptor m_geometryPassDSVDescriptor;
			D3D12_CLEAR_VALUE m_geometryPassDSVClearValue;

			D3D12::GraphicsPass m_DeferredGeometryPass;
			D3D12::GraphicsPass m_DeferredLightPass;
			D3D12_VERTEX_BUFFER_VIEW m_lightPassVBV;
			D3D12::GPUResource m_lightPassQuadBuffer;
			std::vector<D3D12::GPUResource> m_LightPassRenderTextures;
			std::vector<D3D12::Descriptor> m_LightPassRenderTextureDescriptors;
			//

			DXM::Vector2 m_renderResolution;

		private:

			void CreateSamplers();

			void CreateDeferredGeometryPass();
			void CreateDeferredLightPass();

			void ExecuteDeferredGeometryPass(const Camera& camera, const std::vector<Entity>& entitiesToRender);

			void ExecuteDeferredLightPass(D3D12::GPUResource& RenderTarget, const D3D12::Descriptor& RenderTargetDescriptor);

			// TODO - Implement culling techniques used here (frustrum, octree etc...)
			void CalculateVisibility(const Camera& camera, std::vector<Entity>& visibleEntities, const std::vector<Entity>& allEntities);

			void Render(const Camera& camera, const std::vector<Entity>& entitiesToRender);

			
			//
		public:

			RenderSystem(ComponentManager& componentManager,
				/*temp...*/ ShaderCache* const shaderCache, MeshCache* MeshCache, TextureCache* TextureCache,
				D3D12::DescriptorManager* descriptorManager,
				D3D12::CommandQueue* commandQueue, D3D12::ResourceRecycler* resourceRecycler, const DXM::Vector2& renderResolution);
				

			// TODO - Make it grab the final render output which should (?) be local to the renderer/rendersystem
			D3D12::GPUResource& GetFrameRendertarget()
			{
				return m_LightPassRenderTextures[m_frameIndex];
			}

			void BeginFrame(int frameIndex)
			{
				m_frameIndex = frameIndex;

				if (m_frameIndex % NUM_FRAMEBUFFERS == 0)
				{
					m_directContext.FreeCommandBuffer();
				}
			}

			void RenderFromView(const Camera& camera)
			{
				std::vector<Entity> entitiesToRender;
				CalculateVisibility(camera, entitiesToRender, m_entities.GetConstReferenceInternal());
				Render(camera, entitiesToRender);
			}

			virtual void Update() override
			{
				// ...
				throw;
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