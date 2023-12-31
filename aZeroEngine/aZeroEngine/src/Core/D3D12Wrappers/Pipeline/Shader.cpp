#include "Shader.h"
#include "../../Helpers/HelperFunctions.h"

void aZero::D3D12::Shader::Initialize(Shader::TYPE type, const std::string& srcFilePath)
{
	if (!m_compiledShader)
	{
		// TODO - Call CompileFromFile and store all the necessary data in the maps etc...
		// CompileFromFile(srcFilePath);
		m_shaderType = type;
		m_compiledShader = HelperFunctions::LoadBlobFromFile(srcFilePath);
	}
}
