#pragma once
#include "../Helpers/CommandProcessor.h"
#include "../Core/D3D12Wrappers/Commands/CommandQueue.h"
#include "../Core/D3D12Wrappers/Descriptors/DescriptorManager.h"
#include "../Core/Caches/ShaderCache.h"
#include "../Core/D3D12Wrappers/Pipeline/Graphics/GraphicsPass.h"
#include "../Core/D3D12Wrappers/Pipeline/Compute/ComputePass.h"
#include "../Core/D3D12Wrappers/Resources/StructredBuffer.h"
#include "../Core/D3D12Wrappers/Resources/GPUTexture.h"
#include "../DataStructures/IndexFreelist.h"

namespace aZero
{
	namespace Rendering
	{
		struct PrimitiveRenderData
		{
			int VertexBufferOffset;
			int IndexBufferOffset;
			float MeshBounds;
			int MeshLOD;

			int MaterialIndex;

			DXM::Matrix LocalToWorld;

			int PrimitiveID;
		};

		template<typename ElementType>
		class FrameResourceArray
		{
		private:
			D3D12::DefaultHeapStructuredBuffer<ElementType> m_GPUBuffer;
			std::vector<D3D12::UploadHeapStructuredBuffer<ElementType>> m_StagingBuffers;
			DataStructures::IndexFreelist IndexFreeList;

		public:
			FrameResourceArray(int NumElements, int BufferCount, DXGI_FORMAT Format, std::optional<D3D12::ResourceRecycler> OptResourceRecycler)
			{
				IndexFreeList = std::move(DataStructures::IndexFreelist(NumElements));
				m_GPUBuffer = D3D12::DefaultHeapStructuredBuffer<ElementType>(NumElements, Format, OptResourceRecycler);
				for (int i = 0; i < NumElements; i++)
				{
					m_StagingBuffers.emplace_back(D3D12::UploadHeapStructuredBuffer<ElementType>(NumElements, Format, OptResourceRecycler));
				}
			}

			void WriteElement(int StagingBufferIndex, int ElementIndex, const ElementType& Data)
			{
				m_StagingBuffers[StagingBufferIndex].Write(ElementIndex, ElementIndex + 1, (void*)&Data);
			}

			void UploadToGPU(int StagingBufferIndex, ID3D12GraphicsCommandList* CmdList)
			{
				m_GPUBuffer.Write(0, m_GPUBuffer.GetNumElements(), m_StagingBuffers[StagingBufferIndex].GetInternalBuffer(), 0, CmdList);
			}
		};

		class Renderer : public Helpers::NoneCopyable
		{
		private:
			struct GBuffers
			{
				D3D12::GPUTexture m_Color;
				D3D12::Descriptor m_ColorsRTV;
				D3D12::Descriptor m_ColorsUAV;

				D3D12::GPUTexture m_Positions;
				D3D12::Descriptor m_PositionsRTV;
				D3D12::Descriptor m_PositionsUAV;

				D3D12::GPUTexture m_Normals;
				D3D12::Descriptor m_NormalsRTV;
				D3D12::Descriptor m_NormalsUAV;

				GBuffers() = default;

				GBuffers(const DXM::Vector2& RenderResolution, D3D12::DescriptorManager& DescriptorManager)
				{
					this->Init(RenderResolution, DescriptorManager);
				}

				void Init(const DXM::Vector2& RenderResolution, D3D12::DescriptorManager& DescriptorManager)
				{

					m_Color = std::move(D3D12::GPUTexture(
						DXM::Vector3(RenderResolution.x, RenderResolution.y, 0),
						DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
						D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
						1,
						D3D12_RESOURCE_STATE_COMMON,
						{}, {}));

					m_Positions = std::move(D3D12::GPUTexture(
						DXM::Vector3(RenderResolution.x, RenderResolution.y, 0),
						DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
						D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
						1,
						D3D12_RESOURCE_STATE_COMMON,
						{}, {}));

					m_Normals = std::move(D3D12::GPUTexture(
						DXM::Vector3(RenderResolution.x, RenderResolution.y, 0),
						DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
						D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
						1,
						D3D12_RESOURCE_STATE_COMMON,
						{}, {}));

					m_ColorsRTV = DescriptorManager.GetRtvHeap().GetDescriptor();
					m_PositionsRTV = DescriptorManager.GetRtvHeap().GetDescriptor();
					m_NormalsRTV = DescriptorManager.GetRtvHeap().GetDescriptor();

					m_ColorsUAV = DescriptorManager.GetResourceHeap().GetDescriptor();
					m_PositionsUAV = DescriptorManager.GetResourceHeap().GetDescriptor();
					m_NormalsUAV = DescriptorManager.GetResourceHeap().GetDescriptor();

					D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc;
					SrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					SrvDesc.Texture2D.MipLevels = 1;
					SrvDesc.Texture2D.MostDetailedMip = 0;
					SrvDesc.Texture2D.ResourceMinLODClamp = 0;
					SrvDesc.Texture2D.PlaneSlice = 0;
					SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					//SrvDesc.Shader4ComponentMapping = xxx;

					D3D12_RENDER_TARGET_VIEW_DESC RtvDesc;
					RtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					RtvDesc.Texture2D.MipSlice = 0;
					RtvDesc.Texture2D.PlaneSlice = 0;
					RtvDesc.ViewDimension = D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2D;

					gDevice->CreateShaderResourceView(m_Color.GetResource(), &SrvDesc, m_ColorsUAV.GetCPUHandle());
					gDevice->CreateRenderTargetView(m_Color.GetResource(), &RtvDesc, m_ColorsRTV.GetCPUHandle());

					SrvDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					RtvDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					gDevice->CreateShaderResourceView(m_Positions.GetResource(), &SrvDesc, m_PositionsUAV.GetCPUHandle());
					gDevice->CreateRenderTargetView(m_Positions.GetResource(), &RtvDesc, m_PositionsRTV.GetCPUHandle());

					gDevice->CreateShaderResourceView(m_Normals.GetResource(), &SrvDesc, m_NormalsUAV.GetCPUHandle());
					gDevice->CreateRenderTargetView(m_Normals.GetResource(), &RtvDesc, m_NormalsRTV.GetCPUHandle());
				}
			};

		private:
			GBuffers m_GBuffers;

			D3D12::GPUTexture m_FinalRenderSurface;
			D3D12::Descriptor m_FinalRenderSurfaceUAV;

			D3D12::GPUTexture m_GeometryDepthTexture;
			D3D12::Descriptor m_GeometryDepthTextureDSV;

			class Occtree; // static primitives
			class Occtree; // static lights
			class SceneHierarchy; // info of all primitives and parenting etc

			// Main Rendering Classes
			D3D12::CommandQueue m_GraphicsQueue;
			D3D12::DescriptorManager m_DescriptorManager;
				// Command lists????
			D3D12::CommandContext m_GraphicsCommandContext;

			// Pipeline Caching
			ShaderCache m_ShaderCache;
			std::unordered_map<std::string, D3D12::GraphicsPass> m_GraphicsPassCache;
			std::unordered_map<std::string, D3D12::ComputePass> m_ComputePassCache;

			// Connection to swapchain
			CommandProcessor m_CommandProcessor;

			std::atomic<int> m_RTFrameIndex = 2;
			bool m_RenderNextFrame = false;

			DXM::Vector2 RenderResolution;

			// make swapchain owned by the render thread and the window owned by the game thread
			std::atomic<D3D12::SwapChain*> m_CurrentSwapchain = nullptr;

			UINT64 m_LastPresentSignal;

		private:
			void CopyRenderSurfaceToBackBuffer()
			{
				ID3D12GraphicsCommandList* const CmdList = m_GraphicsCommandContext.GetCommandList();
				CmdList->CopyResource(m_CurrentSwapchain.load()->GetBackBufferResource(m_RTFrameIndex), m_FinalRenderSurface.GetResource());
			}

			void Present()
			{
				m_CurrentSwapchain.load()->Present();
				m_LastPresentSignal = m_GraphicsQueue.ForceSignal();
			}

			void BuildRenderPasses()
			{
				m_GeometryDepthTexture = std::move(
					D3D12::GPUTexture(
						DXM::Vector3(RenderResolution.x, RenderResolution.y, 0),
						DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT,
						D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
						1,
						D3D12_RESOURCE_STATE_DEPTH_WRITE,
						std::nullopt,
						std::nullopt)
				);

				m_GeometryDepthTextureDSV = m_DescriptorManager.GetDsvHeap().GetDescriptor();

				D3D12_DEPTH_STENCIL_VIEW_DESC DsvDesc;
				DsvDesc.Flags = D3D12_DSV_FLAG_NONE;
				DsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				DsvDesc.Texture2D.MipSlice = 0;
				DsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

				gDevice->CreateDepthStencilView(m_GeometryDepthTexture.GetResource(), &DsvDesc, m_GeometryDepthTextureDSV.GetCPUHandle());

				m_FinalRenderSurface = std::move(
					D3D12::GPUTexture(
						DXM::Vector3(RenderResolution.x, RenderResolution.y, 0),
						DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
						D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
						1,
						D3D12_RESOURCE_STATE_COMMON,
						std::nullopt,
						std::nullopt)
				);

				D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc;
				UavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				UavDesc.Texture2D.MipSlice = 0;
				UavDesc.Texture2D.PlaneSlice = 0;
				UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

				m_FinalRenderSurfaceUAV = m_DescriptorManager.GetResourceHeap().GetDescriptor();

				gDevice->CreateUnorderedAccessView(m_FinalRenderSurface.GetResource(), nullptr, &UavDesc, m_FinalRenderSurfaceUAV.GetCPUHandle());

				{
					D3D12::GraphicsPass::PassDescription BasePassDesc;

					m_ShaderCache.CompileAndStore("BasePassVS");
					m_ShaderCache.CompileAndStore("BasePassPS");
					D3D12::Shader* BasePassVS = m_ShaderCache.GetShader("BasePassVS");
					BasePassVS->AddParameter<D3D12::Shader::RootConstant>(D3D12::Shader::RootConstant("VertexShaderConstants", 0, 32));
					BasePassVS->AddParameter<D3D12::Shader::RootConstant>(D3D12::Shader::RootConstant("PrimitiveData", 0, 1));

					D3D12::Shader* BasePassPS = m_ShaderCache.GetShader("BasePassPS");
					BasePassPS->AddParameter<D3D12::Shader::RootConstant>(D3D12::Shader::RootConstant("PixelShaderConstants", 0, 1));
					
					BasePassDesc.VertexShader = *BasePassVS;
					BasePassDesc.PixelShader = *BasePassPS;
					BasePassDesc.TopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
					BasePassDesc.BlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
					BasePassDesc.DepthStencilDesc.Description = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
					BasePassDesc.DepthStencilDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
					BasePassDesc.RasterDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
					BasePassDesc.RasterDesc.CullMode = D3D12_CULL_MODE_FRONT;
					BasePassDesc.SampleDesc.Count = 1;
					BasePassDesc.SampleDesc.Quality = 0;


					D3D12::GraphicsPass BasePass(gDevice.Get(), BasePassDesc);
					m_GraphicsPassCache.emplace("BasePass", std::move(BasePass));

				}

				//D3D12::ComputePass::PassDescription LightPassDesc;
				//m_ShaderCache.CompileAndStore("LightPassCS");
				//D3D12::Shader* LightPassCS = m_ShaderCache.GetShader("LightPassCS");
				//LightPassCS->AddParameter(D3D12::Shader::RootConstant("Data", 0, 4));
				//D3D12::ComputePass LightPass(LightPassDesc);
				//m_ComputePassCache.emplace("LightPass", std::move(LightPass));
			}

			void RunBasePass()
			{
				D3D12::GraphicsPass& BasePass = m_GraphicsPassCache.at("BasePass");
				ID3D12GraphicsCommandList* const CmdList = m_GraphicsCommandContext.GetCommandList();
				BasePass.BeginPass(CmdList, m_DescriptorManager.GetResourceHeap().GetDescriptorHeap(), m_DescriptorManager.GetSamplerHeap().GetDescriptorHeap());
				std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> GBufferHandles = {
					m_GBuffers.m_ColorsRTV.GetCPUHandle(),
					m_GBuffers.m_PositionsRTV.GetCPUHandle(),
					m_GBuffers.m_NormalsRTV.GetCPUHandle() };

				//BasePass.SetOutputTargets(CmdList, GBufferHandles, &m_GeometryDepthTextureDSV.GetCPUHandle());
				
				{
					// temp draw to test

					//BasePass.SetIndexBuffer(CmdList, xxx, 0, 1);
					//BasePass.SetVertexBuffer(CmdList, xxx, 0, 1);
					//BasePass.DrawIndexedInstanced(CmdList, xxx, 1, 0, 0, 0);

				}
			}

			void RunLightPass()
			{
				D3D12::ComputePass& LightPass = m_ComputePassCache.at("LightPass");
				ID3D12GraphicsCommandList* const CmdList = m_GraphicsCommandContext.GetCommandList();
				LightPass.BeginPass(CmdList);

				{
					// temp draw to test
					struct LightPassCSData
					{
						int OutputTargetUAVIndex;
						int FragmentColorGBufferSRVIndex;
						int WorldPositionGBufferSRVIndex;
						int NormalGBufferSRVIndex;
					};
					LightPassCSData RootConstantData;
					LightPass.SetShaderRootConstant(CmdList, "Data", &RootConstantData);
					LightPass.Dispatch(CmdList, RenderResolution.x, RenderResolution.y, 0);
				}
			}

			void ProcessGameThreadCommands()
			{
				while (!m_RenderNextFrame)
				{
					m_CommandProcessor.ExecuteNext();
				}

				m_RTFrameIndex.fetch_add(1);
			}

			void GatherVisiblePrimitives()
			{
				GatherPrimtivesFromOctree();
				GatherPrimitivesFromView();
			}

			void GatherPrimtivesFromOctree()
			{

			}

			void GatherPrimitivesFromView()
			{

			}

			void RenderPrimitives()
			{
				RunBasePass();
				RunLightPass();
			}

		public:
			Renderer()
			{
				Init();
			}

			~Renderer()
			{

			}

			void Init()
			{
				RenderResolution = DXM::Vector2(800, 600);

				m_DescriptorManager.Initialize(gDevice.Get());

				m_GBuffers.Init(RenderResolution, m_DescriptorManager);

				BuildRenderPasses();
			}

			void BeginFrame()
			{
				m_GraphicsCommandContext.StartRecording();
			}

			void Render()
			{
				this->ProcessGameThreadCommands();
				this->GatherVisiblePrimitives();
				this->RenderPrimitives();
				this->CopyRenderSurfaceToBackBuffer();

				m_GraphicsCommandContext.StopRecording();
				m_GraphicsQueue.ExecuteContext({ m_GraphicsCommandContext });
				this->Present(); // NOTE - Add input of swapchain texture and perform pass output -> backbuffer resolve
			}

			void EndFrame()
			{
				m_RenderNextFrame = false;
			}

			void EnqueueRenderCommand(CommandTask&& task)
			{
				m_CommandProcessor.Enqueue(std::forward<CommandTask>(task));
			}

			int GetFrameIndex()
			{
				return m_RTFrameIndex.load();
			}

			void SignalRenderNextFrame()
			{
				m_CommandProcessor.Enqueue([this]
					{
						m_RenderNextFrame = true;
					});
			}

			void SetActiveSwapchain(D3D12::SwapChain* SwapChain)
			{
				m_CurrentSwapchain.store(SwapChain);

				// Do everything inc. stalling gpu, recreating surfaces etc...
			}
			
		};
	}
}