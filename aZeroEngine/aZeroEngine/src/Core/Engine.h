#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

// STD
#include <memory>
#include <string>

#include "Window.h"
#include "../D3D12Core.h"
#include "../Helpers/NoneCopyable.h"
#include "../Renderer/Renderer.h"

namespace aZero
{
	class Engine : public Helpers::NoneCopyable
	{
	private:

		const int m_BufferCount = 3;

		std::atomic<bool> m_ShutDownEngine = false;

		std::thread m_RenderThreadHandle;

		std::atomic<int> m_GTFrameIndex = 0;

		// Window
		std::shared_ptr<Window> m_mainWindow = nullptr;

		Rendering::Renderer m_Renderer;

		void InitVitalD3D12()
		{
			HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(gDevice.GetAddressOf()));
			if (FAILED(hr))
			{
				// DEBUG MACRO
				throw;
			}
		}

		void RenderThread_Main()
		{
			while (!m_ShutDownEngine.load()) // why no exit...
			{
				m_Renderer.BeginFrame();

				m_Renderer.Render();
				
				m_Renderer.EndFrame();
			}
			std::cout << "EXIT";
		}

		void StartRenderThread()
		{
			m_RenderThreadHandle = std::thread([this] {
				RenderThread_Main();
				});
		}

	public:
		Engine(HINSTANCE appInstance, const DXM::Vector2 windowResolution)
		{
			m_Renderer.Init();

			StartRenderThread();

			// Window Setup
			m_mainWindow = std::make_shared<Window>(
				appInstance,
				WndProc,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				windowResolution,
				false,
				"aZero Engine");
		}
		
		~Engine()
		{
			m_ShutDownEngine.store(true);
			m_RenderThreadHandle.join();
		}

		std::shared_ptr<Window> GetMainWindow() { return m_mainWindow; }

		void BeginFrame()
		{
			while (m_Renderer.GetFrameIndex() < m_GTFrameIndex.load() - 2);
			if (m_mainWindow->IsOpen())
			{
				m_mainWindow->HandleMessages();
			}
		}

		void Update()
		{

		}

		void EndFrame()
		{
			m_GTFrameIndex.fetch_add(1);
			m_Renderer.SignalRenderNextFrame();
		}
	};
}