#pragma once
#include <Windows.h>
#include <string>

#include "D3D12Wrappers/SwapChain.h"
#include "D3D12Wrappers/Commands/CommandQueue.h"
#include "D3D12Wrappers/ResourceRecycler.h"
#include "D3D12Wrappers/Resources/GPUResource.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace aZero
{
	class Window
	{
	private:
		std::unique_ptr<D3D12::SwapChain> m_swapChain = nullptr;
		HWND m_windowHandle;
		std::wstring m_windowName;
		HINSTANCE m_appInstance;
		DXM::Vector2 m_lastWindowedDimensions = { 0, 0 };

	public:
		Window(
			const D3D12::CommandQueue& graphicsQueue,
			D3D12::DescriptorManager& descriptorManager,
			HINSTANCE appInstance,
			WNDPROC winProcedure,
			DXGI_FORMAT backBufferFormat,
			const DXM::Vector2& dimensions, bool fullscreen, const std::string& windowName);

		~Window();

		bool IsOpen();
		void Resize(const DXM::Vector2& dimensions, const DXM::Vector2& position = { 0, 0 });
		void SetFullscreen(bool enabled);
		DXM::Vector2 GetClientDimensions() const;
		DXM::Vector2 GetFullDimensions() const;

		D3D12::SwapChain& GetSwapChain() { return *m_swapChain.get(); }

		void HandleMessages();

		Window(const Window&) = delete;
		Window(Window&&) = delete;
		Window operator=(const Window&) = delete;
		Window operator=(Window&&) = delete;
	};
}

