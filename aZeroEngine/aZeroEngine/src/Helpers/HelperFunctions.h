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
    }
}