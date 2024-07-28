#pragma once
#include "../../D3D12Core.h"

namespace aZero
{
	namespace Asset
	{
		enum MATERIAL_TYPE { TOON, INVALID };

		class MaterialBase
		{
		private:
			MATERIAL_TYPE m_Type = INVALID;
			std::string m_Name = "";

		public:
			MaterialBase() = default;
			MaterialBase(MATERIAL_TYPE Type)
				:m_Type(Type)
			{

			}
		};

		class ToonMaterial : public MaterialBase
		{
		private:
			struct RenderData
			{
				int AlbedoTextureIndex = -1;
				int NormalMapTextIndex = -1;
			};

			RenderData m_RenderData;

		public:
			ToonMaterial()
				:MaterialBase(MATERIAL_TYPE::TOON)
			{

			}

			RenderData& GetRenderData() { return m_RenderData; }
		};

		/*template<typename ShaderInfo>
		class Material
		{
		private:
			ShaderInfo m_ShaderInfo;

		public:
			
			Material()
			{

			}

			ShaderInfo& GetShaderInfo() { return m_ShaderInfo; }
		};

		struct ToonMaterialInfo
		{
			int AlbedoTextureIndex = -1;
			int NormalMapTextIndex = -1;
			int LightStepTextureIndex = -1;
			DXM::Vector3 AlbedoColor;
		};

		using ToonMaterial = Material<ToonMaterialInfo>;


		enum MATERIAL_TYPE { TOON, INVALID };*/
	}
}