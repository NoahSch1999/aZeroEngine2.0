#pragma once
#include "../D3D12Wrappers/Resources/GPUBuffer.h"

namespace aZero
{
	namespace Asset
	{
		class Mesh
		{
		private:
			D3D12::Resource::GPUBuffer* m_vertexBuffer;
			D3D12_GPU_VIRTUAL_ADDRESS m_virtualAddress;
			int m_numVertices = -1;

		public:
			Mesh()
			{

			}
		};
	}
}