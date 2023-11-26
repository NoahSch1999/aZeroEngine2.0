#pragma once
#include "../Shader.h"

namespace aZero
{
	namespace D3D12
	{
		// TODO - Create fail-safe so that the user can evaluate if the pass is valid. This way it can be used easier in the editor
		// TODO - Add support for HS, DS, and GS stages
		class GraphicsPass
		{
		public:
			struct DepthStencilDesc
			{
				D3D12_DEPTH_STENCIL_DESC Description;
				DXGI_FORMAT Format = DXGI_FORMAT_FORCE_UINT;
			};

			struct PassDescription
			{
				D3D12_BLEND_DESC BlendDesc;
				D3D12_RASTERIZER_DESC RasterDesc;
				D3D12_INPUT_LAYOUT_DESC InputLayoutDesc;
				D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType;
				DXGI_SAMPLE_DESC SampleDesc;
				DepthStencilDesc DepthStencilDesc;
				std::vector<D3D12_STATIC_SAMPLER_DESC> StaticSamplers;
			};

		private:
			Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso = nullptr;
			Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;

			Shader m_vertexShader;
			Shader m_pixelShader;

			PassDescription m_description;

			// TODO - Try to avoid having to go through this hashmap when setting buffers etc
			std::unordered_map<std::string, int> m_vsParameterNameToRootSignatureSlot;
			std::unordered_map<std::string, int> m_psParameterNameToRootSignatureSlot;

			void CreateRootSignatureFromShaders(ID3D12Device* const device)
			{
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
					m_description.StaticSamplers.size(), &m_description.StaticSamplers[0],
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
				D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
				ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

#ifdef _DEBUG
				psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;
#endif // DEBUG

				psoDesc.pRootSignature = m_rootSignature.Get();
				psoDesc.SampleDesc = m_description.SampleDesc;
				psoDesc.RasterizerState = m_description.RasterDesc;
				psoDesc.BlendState = m_description.BlendDesc;
				psoDesc.InputLayout = m_description.InputLayoutDesc;

				psoDesc.NumRenderTargets = m_pixelShader.GetRenderTargetFormats().size();
				for (int i = 0; i < m_pixelShader.GetRenderTargetFormats().size(); i++)
				{
					psoDesc.RTVFormats[i] = m_pixelShader.GetRenderTargetFormats()[i];
				}

				if (m_description.DepthStencilDesc.Description.DepthEnable)
				{
					psoDesc.DepthStencilState = m_description.DepthStencilDesc.Description;
					psoDesc.DSVFormat = m_description.DepthStencilDesc.Format;
				}

				psoDesc.VS = {
					reinterpret_cast<BYTE*>(m_vertexShader.GetShaderBlob()->GetBufferPointer()),
					m_vertexShader.GetShaderBlob()->GetBufferSize()
				};

				psoDesc.PS = {
					reinterpret_cast<BYTE*>(m_pixelShader.GetShaderBlob()->GetBufferPointer()),
					m_pixelShader.GetShaderBlob()->GetBufferSize()
				};

				HRESULT hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_pso.GetAddressOf()));
				if (FAILED(hr))
				{
					// TODO - Add logging etc...
					throw;
				}
			}

		public:
			GraphicsPass() = default;

			GraphicsPass(ID3D12Device* const device, const PassDescription& description, const Shader& vShader, const Shader& pShader)
				:m_vertexShader(vShader), m_pixelShader(pShader), m_description(description)
			{
				Build(device);
			}

			~GraphicsPass() = default;

			void SetVertexShader(const Shader& vertexShader)
			{
				m_vertexShader = vertexShader;
			}

			void SetPixelShader(const Shader& pixelShader)
			{
				m_pixelShader = pixelShader;
			}

			void SetDescripton(const PassDescription& desc)
			{
				m_description = desc;
			}

			void Build(ID3D12Device* const device)
			{
				if (!m_rootSignature)
				{
					CreateRootSignatureFromShaders(device);
					CreatePipelineStateObject(device);
				}
			}

			void SetPass(ID3D12GraphicsCommandList* const commandList)
			{
				commandList->SetGraphicsRootSignature(m_rootSignature.Get());
				commandList->SetPipelineState(m_pso.Get());
			}

			template<typename ShaderType>
			void SetShaderRootConstant(ID3D12GraphicsCommandList* const commandList, const std::string& paramName, void* data)
			{
				if constexpr (std::is_same_v<ShaderType, ShaderTypes::VertexShader>)
				{
					commandList->SetGraphicsRoot32BitConstants(m_vsParameterNameToRootSignatureSlot.at(paramName), 
						m_vertexShader.GetRootConstants().at(paramName).Num32BitValues, 
						data, 0);
				}
				else if constexpr (std::is_same_v<ShaderType, ShaderTypes::PixelShader>)
				{
					commandList->SetGraphicsRoot32BitConstants(m_psParameterNameToRootSignatureSlot.at(paramName),
						m_vertexShader.GetRootConstants().at(paramName).Num32BitValues,
						data, 0);
				}
			}

			template<typename ShaderType, typename DescriptorType>
			void SetShaderRootDescriptor(ID3D12GraphicsCommandList* const commandList, const std::string& paramName, D3D12_GPU_VIRTUAL_ADDRESS address)
			{
				if constexpr (std::is_same_v<ShaderType, ShaderTypes::VertexShader>)
				{
					if constexpr (std::is_same_v<DescriptorType, DescriptorTypes::CBV>)
					{
						commandList->SetGraphicsRootConstantBufferView(m_vsParameterNameToRootSignatureSlot.at(paramName), address);
					}
					else if constexpr (std::is_same_v<DescriptorType, DescriptorTypes::SRV>)
					{
						commandList->SetGraphicsRootShaderResourceView(m_vsParameterNameToRootSignatureSlot.at(paramName), address);
					}
					else if constexpr (std::is_same_v<DescriptorType, DescriptorTypes::UAV>)
					{
						commandList->SetGraphicsRootUnorderedAccessView(m_vsParameterNameToRootSignatureSlot.at(paramName), address);
					}
				}
				else if constexpr (std::is_same_v<ShaderType, ShaderTypes::PixelShader>)
				{
					if constexpr (std::is_same_v<DescriptorType, DescriptorTypes::CBV>)
					{
						commandList->SetGraphicsRootConstantBufferView(m_psParameterNameToRootSignatureSlot.at(paramName), address);
					}
					else if constexpr (std::is_same_v<DescriptorType, DescriptorTypes::SRV>)
					{
						commandList->SetGraphicsRootShaderResourceView(m_psParameterNameToRootSignatureSlot.at(paramName), address);
					}
					else if constexpr (std::is_same_v<DescriptorType, DescriptorTypes::UAV>)
					{
						commandList->SetGraphicsRootUnorderedAccessView(m_psParameterNameToRootSignatureSlot.at(paramName), address);
					}
				}
			}
		};
	}
}