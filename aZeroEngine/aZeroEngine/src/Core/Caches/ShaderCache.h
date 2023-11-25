#pragma once
#include <string>
#include <unordered_map>

#include "../D3D12Wrappers/Pipeline/Shader.h"

#define SHADER_OBJECT_DIRECTORY ""

namespace aZero
{
	namespace D3D12
	{
		class ShaderCache
		{
		private:
			std::unordered_map<std::string, Shader> m_shaders;

		public:
			ShaderCache()
			{

			}

			~ShaderCache()
			{

			}

			void CompileAndStore(const std::string& shaderSourceFileName)
			{
				// Compile shader with path
				const std::string absolutePath(SHADER_OBJECT_DIRECTORY + shaderSourceFileName + ".hlsl");

				// Store shader in m_shaders

			}

			void Remove(const std::string& shaderSourceFileName)
			{
				m_shaders.erase(shaderSourceFileName);
			}

			// TODO - Implement move and copy constructors / operators
			ShaderCache(const ShaderCache&) = delete;
			ShaderCache(ShaderCache&&) = delete;
			ShaderCache operator=(const ShaderCache&) = delete;
			ShaderCache operator=(ShaderCache&&) = delete;
		};
	}
}