#pragma once
#include "../D3D12Core.h"

namespace aZero
{
    namespace HelperFunctions
    {
        template<typename numerical>
        numerical RoundUpNumerical(numerical numToRound, numerical multiple)
        {
            return ((numToRound + multiple - 1) / multiple) * multiple;
        }

        Microsoft::WRL::ComPtr<ID3DBlob> LoadBlobFromFile(const std::string& filePath);

		struct BasicVertex
		{
			DXM::Vector3 position;
			DXM::Vector2 uv;
			DXM::Vector3 normal;
			DXM::Vector3 tangent;
		};

		struct FBXFileInfo
		{
			struct MeshInfo
			{
				std::string Name = "";
				std::vector<BasicVertex> Vertices;
				std::vector<int> Indices;
			};

			std::vector<MeshInfo> Meshes;
		};

		void LoadFBXFile(FBXFileInfo& OutInfo, const std::string& filePath);

    }
}