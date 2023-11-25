#pragma once
#include "D3D12Wrappers/SwapChain.h"

namespace aZero
{
	namespace Window
	{
		class Window
		{
		private:
			std::unique_ptr<D3D12::SwapChain> m_swapChain = nullptr;

		public:
			Window() = default;
		};
	}
}
