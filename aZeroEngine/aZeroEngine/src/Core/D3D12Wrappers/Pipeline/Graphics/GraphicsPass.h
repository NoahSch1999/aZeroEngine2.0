#pragma once
#include "../PipelinePass.h"

namespace aZero
{
	namespace D3D12
	{
		// TODO - Create fail-safe so that the user can evaluate if the pass is valid. This way it can be used easier in the editor
		class GraphicsPass : public PipelinePass
		{
		public:
			struct StaticSamplerDesc
			{
				D3D12_STATIC_SAMPLER_DESC Desc;
			};

		private:

			// TODO - samplers and rtv formats should be deduced from the shader compiler
			Shader m_vertexShader;
			Shader m_pixelShader;
			std::vector<DXGI_FORMAT> m_renderTargetFormats;
			std::vector<StaticSamplerDesc> m_staticSamplers;

			std::unordered_map<std::string, int> m_vsParameterNameToRootSignatureSlot;
			std::unordered_map<std::string, int> m_psParameterNameToRootSignatureSlot;

			void CreateRootSignatureFromShaders(ID3D12Device* const device)
			{
				// Store the name to binding slot somehow

				std::vector<D3D12_ROOT_PARAMETER> allParams;

				for (const auto& [name, param] : m_vertexShader.GetRootConstants())
				{
					D3D12_ROOT_PARAMETER rootParam;
					rootParam.Constants.ShaderRegister = param.BindingSlot;
					rootParam.Constants.Num32BitValues = param.Num32BitValues;
					rootParam.Constants.RegisterSpace = 0;
					rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
					rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
					m_vsParameterNameToRootSignatureSlot.emplace(name, allParams.size());
					allParams.push_back(rootParam);
				}

				for (const auto& [name, param] : m_pixelShader.GetRootConstants())
				{
					D3D12_ROOT_PARAMETER rootParam;
					rootParam.Constants.ShaderRegister = param.BindingSlot;
					rootParam.Constants.Num32BitValues = param.Num32BitValues;
					rootParam.Constants.RegisterSpace = 0;
					rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
					rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
					m_psParameterNameToRootSignatureSlot.emplace(name, allParams.size());
					allParams.push_back(rootParam);
				}

				for (const auto& [name, param] : m_vertexShader.GetRootDescriptors())
				{
					D3D12_ROOT_PARAMETER rootParam;
					rootParam.Descriptor.ShaderRegister = param.BindingSlot;
					rootParam.Descriptor.RegisterSpace = 0;
					rootParam.ParameterType = param.ParameterType;
					rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
					m_vsParameterNameToRootSignatureSlot.emplace(name, allParams.size());
					allParams.push_back(rootParam);
				}

				for (const auto& [name, param] : m_pixelShader.GetRootDescriptors())
				{
					D3D12_ROOT_PARAMETER rootParam;
					rootParam.Descriptor.ShaderRegister = param.BindingSlot;
					rootParam.Descriptor.RegisterSpace = 0;
					rootParam.ParameterType = param.ParameterType;
					rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
					m_psParameterNameToRootSignatureSlot.emplace(name, allParams.size());
					allParams.push_back(rootParam);
				}

				const D3D12_ROOT_SIGNATURE_DESC desc{ 
					(UINT)allParams.size(),
					allParams.data(),
					m_staticSamplers.size(), &m_staticSamplers[0].Desc, 
					D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
					| D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED
					| D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED };

				Microsoft::WRL::ComPtr<ID3DBlob> serializeBlob;
				Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

				if (FAILED(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, serializeBlob.GetAddressOf(), errorBlob.GetAddressOf())))
				{
					// TODO - Add debug error
					throw;
				}

				if (FAILED(device->CreateRootSignature(0, serializeBlob->GetBufferPointer(), serializeBlob->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.GetAddressOf()))))
				{
					// TODO - Add debug error
					throw;
				}
			}

			void CreatePipelineStateObject(ID3D12Device* const device)
			{

			}

		public:
			GraphicsPass()
			{

			}

			void SetVertexShader(const Shader& vertexShader)
			{
				m_vertexShader = vertexShader;
			}

			void SetPixelShader(const Shader& pixelShader)
			{
				m_pixelShader = pixelShader;
			}

			void AddRenderTarget(DXGI_FORMAT format)
			{
				m_renderTargetFormats.push_back(format);
			}

			void AddStaticSampler(const GraphicsPass::StaticSamplerDesc& sampler)
			{
				m_staticSamplers.push_back(sampler);
			}

			virtual void Build(ID3D12Device* const device)
			{
				if (!m_rootSignature)
				{
					CreateRootSignatureFromShaders(device);
					CreatePipelineStateObject(device);
				}
			}

			~GraphicsPass()
			{

			}

			GraphicsPass(const GraphicsPass&) = delete;
			GraphicsPass(GraphicsPass&&) = delete;
			GraphicsPass operator=(const GraphicsPass&) = delete;
			GraphicsPass operator=(GraphicsPass&&) = delete;
		};
	}
}