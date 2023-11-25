#pragma once
#include "RootSignature.h"
#include "Shader.h"

namespace aZero
{
	namespace D3D12
	{
		class PipelinePass
		{
		protected:
			Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso = nullptr;
			Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;

		public:
			// Takes in x-type shaders and generate a rootsignature + pso for them and check so that they work correctly
			PipelinePass()
			{

			}

			virtual void Build(ID3D12Device* const device) = 0;

			~PipelinePass()
			{

			}

			PipelinePass(const PipelinePass&) = delete;
			PipelinePass(PipelinePass&&) = delete;
			PipelinePass operator=(const PipelinePass&) = delete;
			PipelinePass operator=(PipelinePass&&) = delete;
		};
	}
}