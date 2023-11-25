#include <fstream>

#include "HelperFunctions.h"

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