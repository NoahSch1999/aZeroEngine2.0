#pragma once
#include "../PipelinePass.h"

namespace aZero
{
	namespace D3D12
	{
		class ComputePass : public PipelinePass
		{
		private:

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