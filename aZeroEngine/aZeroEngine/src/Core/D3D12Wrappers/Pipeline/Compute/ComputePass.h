#pragma once
#include "../Shader.h"

namespace aZero
{
	namespace D3D12
	{
		class ComputePass
		{
		private:
			Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso = nullptr;
			Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;

		public:
			ComputePass()
			{

			}

			~ComputePass()
			{

			}

			ComputePass(const ComputePass&) = delete;
			ComputePass(ComputePass&&) = delete;
			ComputePass operator=(const ComputePass&) = delete;
			ComputePass operator=(ComputePass&&) = delete;
		};
	}
}