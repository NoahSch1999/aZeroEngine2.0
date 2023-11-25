#pragma once
#include <unordered_map>
#include <string>
#include "../../D3D12Core.h"

namespace aZero
{
	namespace D3D12
	{
		class Shader
		{
		public:
			enum class TYPE { VS, GS, PS, MAX_INVALID };

			struct ShaderParameter
			{
				std::string Name = "INVALID";
				int BindingSlot = -1;
			};

			struct RootConstant : public ShaderParameter
			{
				int Num32BitValues;
			};

			struct RootDescriptor : public ShaderParameter
			{
				D3D12_ROOT_PARAMETER_TYPE ParameterType;
			};

		private:

			TYPE m_shaderType = TYPE::MAX_INVALID;
			std::unordered_map<std::string, RootConstant> m_rootConstants;
			std::unordered_map<std::string, RootDescriptor> m_rootDescriptor;

			Microsoft::WRL::ComPtr<ID3DBlob> m_compiledShader = nullptr;

			void CompileFromFile(const std::string& srcFilePath)
			{
				// TODO - Impl

				// Fill param maps with data
			}

		public:

			Shader() = default;

			// TODO - Remove type input and replace with loaded shaders type automatically
			Shader(Shader::TYPE type, const std::string& srcFilePath)
			{
				Initialize(type, srcFilePath);
			}

			void Initialize(Shader::TYPE type, const std::string& srcFilePath);

			template<typename Parameter>
			void AddParemeter(Parameter&& param)
			{
				if constexpr (std::is_same_v<Parameter, RootConstant>)
				{
					m_rootConstants.emplace(param.Name, param);
				}
				else if constexpr (std::is_same_v<Parameter, RootDescriptor>)
				{
					m_rootDescriptor.emplace(param.Name, param);
				}
				else
				{
					// TODO - Add fatal error debug
					throw;
				}
			}

			const std::unordered_map<std::string, RootConstant>& GetRootConstants() const
			{
				return m_rootConstants;
			}

			const std::unordered_map<std::string, RootDescriptor>& GetRootDescriptors() const
			{
				return m_rootDescriptor;
			}
		};
	}
}