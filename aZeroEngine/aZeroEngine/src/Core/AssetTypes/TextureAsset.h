#pragma once
#include "../D3D12Wrappers/Resources/Texture2D.h"
#include "../D3D12Wrappers/Descriptors/Descriptor.h"

namespace aZero
{
	namespace Asset
	{
		class Texture
		{
		private:
			std::shared_ptr<D3D12::Resource::Texture2D> m_texture = nullptr;
			D3D12::Descriptor m_srvDescriptor;

		public:
			Texture()
			{

			}
		};
	}
}