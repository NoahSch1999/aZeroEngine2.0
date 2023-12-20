#include <fstream>

#include "HelperFunctions.h"
#include "../ThirdParty/assimp/include/assimp/Importer.hpp"
#include "../ThirdParty/assimp/include/assimp/scene.h"
#include "../ThirdParty/assimp/include/assimp/postprocess.h"

Microsoft::WRL::ComPtr<ID3DBlob> aZero::HelperFunctions::LoadBlobFromFile(const std::string& filePath)
{
	std::ifstream fin(filePath, std::ios::binary);
	if (fin.is_open())
	{
		fin.seekg(0, std::ios_base::end);
		std::streampos size = fin.tellg();
		fin.seekg(0, std::ios_base::beg);

		Microsoft::WRL::ComPtr<ID3DBlob> blob;
		HRESULT hr = D3DCreateBlob(size, blob.GetAddressOf());
		if (FAILED(hr))
			throw;

		fin.read((char*)blob->GetBufferPointer(), size);
		fin.close();
		return blob;
	}

	return nullptr;
}

void aZero::HelperFunctions::LoadFBXFile(FBXFileInfo& OutInfo, const std::string& filePath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath + ".fbx", aiProcess_SortByPType | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	for (int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
	{
		OutInfo.Meshes.push_back(FBXFileInfo::MeshInfo());
		FBXFileInfo::MeshInfo& newMesh = OutInfo.Meshes[OutInfo.Meshes.size() - 1];
		newMesh.Name.assign(scene->mMeshes[meshIndex]->mName.C_Str());
		
		newMesh.Vertices.reserve(scene->mMeshes[meshIndex]->mNumVertices);
		for (unsigned int i = 0; i < scene->mMeshes[meshIndex]->mNumVertices; i++)
		{
			BasicVertex vertex;
			aiVector3D tempData;
			tempData = scene->mMeshes[meshIndex]->mVertices[i];
			vertex.position = { tempData.x, tempData.y, tempData.z };

			tempData = scene->mMeshes[meshIndex]->mTextureCoords[0][i];
			vertex.uv = { tempData.x, tempData.y };

			tempData = scene->mMeshes[meshIndex]->mNormals[i];
			tempData.Normalize();
			vertex.normal = { tempData.x, tempData.y, tempData.z };

			tempData = scene->mMeshes[meshIndex]->mTangents[i];
			tempData.Normalize();
			vertex.tangent = { tempData.x, tempData.y, tempData.z };

			newMesh.Vertices.emplace_back(std::move(vertex));
		}

		for (unsigned int i = 0; i < scene->mMeshes[meshIndex]->mNumFaces; i++)
		{
			newMesh.Indices.reserve(newMesh.Indices.capacity() + scene->mMeshes[meshIndex]->mFaces->mNumIndices);
			for (int h = 0; h < scene->mMeshes[meshIndex]->mFaces[i].mNumIndices; h++)
			{
				newMesh.Indices.emplace_back(scene->mMeshes[meshIndex]->mFaces[i].mIndices[h]);
			}
		}
	}

	

}