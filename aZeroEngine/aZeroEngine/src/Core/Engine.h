#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

// STD
#include <memory>
#include <string>

// Core
#include "../D3D12Core.h"
#include "../Helpers/NoneCopyable.h"
#include "Window.h"
#include "Scene.h"
#include "Camera.h"

// ECS
#include "../ECS/ECS.h"
#include "CustomSystems/RenderSystem.h"
#include "CustomSystems/PhysicsSystem.h"

// Asset Caches
#include "Caches/MeshCache.h"
#include "Caches/TextureCache.h"
#include "Caches/MaterialCache.h"
#include "Caches/AudioCache.h"
#include "Caches/ShaderCache.h"

#include "D3D12Wrappers/Pipeline/Graphics/GraphicsPass.h"

namespace aZero
{
	class Engine : public Helpers::NoneCopyable
	{
	private:
		// D3D12 Core
		Microsoft::WRL::ComPtr<ID3D12Device> m_device = nullptr;
		std::unique_ptr<D3D12::DescriptorManager> m_descriptorManager = nullptr;
		std::unique_ptr<D3D12::CommandQueue> m_directQueue = nullptr;
		D3D12::ResourceRecycler m_resourceRecycler;
		int m_frameIndex = 0;
		int m_frameCount = 0;

		// Window
		std::shared_ptr<Window::Window> m_activeWindow = nullptr;

		// ECS
		std::unique_ptr<ECS::ECS> m_ecs = nullptr;
		std::shared_ptr<ECS::RenderSystem> m_renderSystem = nullptr;
		std::shared_ptr<ECS::PhysicsSystem> m_physicsSystem = nullptr;

		// Caches
		std::unique_ptr<MeshCache> m_meshCache = nullptr;
		std::unique_ptr<TextureCache> m_textureCache = nullptr;
		std::unique_ptr<MaterialCache> m_materialCache = nullptr;
		std::unique_ptr<AudioCache> m_audioCache = nullptr;
		std::unique_ptr<ShaderCache> m_shaderCache = nullptr;

		// Scenes
		std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;

		// Temporary
		std::shared_ptr<Camera> m_activeCamera = nullptr;
		D3D12::CommandContext m_directContext;

	public:
		Engine(HINSTANCE appInstance)
		{
			// Setup everything etc...

			// Create device
			HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(m_device.GetAddressOf()));
			if (FAILED(hr))
			{
				// DEBUG MACRO
				throw;
			}

			m_ecs = std::make_unique<ECS::ECS>();
			m_descriptorManager = std::make_unique<D3D12::DescriptorManager>(m_device.Get());
			m_directQueue = std::make_unique<D3D12::CommandQueue>(m_device.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);

			// ECS Setup
			m_ecs->GetComponentManager().RegisterComponentType<Component::Transform>();
			m_ecs->GetComponentManager().RegisterComponentType<Component::Mesh>();
			m_ecs->GetComponentManager().RegisterComponentType<Component::Material>();
			m_ecs->GetComponentManager().RegisterComponentType<Component::PointLight>();
			m_ecs->GetComponentManager().RegisterComponentType<Component::SpotLight>();
			m_ecs->GetComponentManager().RegisterComponentType<Component::DirectionalLight>();
			m_ecs->GetComponentManager().RegisterComponentType<Component::RigidBody>();

			// Asset Cache Setup
			m_meshCache = std::make_unique<MeshCache>();
			m_textureCache = std::make_unique<TextureCache>();
			m_materialCache = std::make_unique<MaterialCache>();
			m_audioCache = std::make_unique<AudioCache>();
			m_shaderCache = std::make_unique<ShaderCache>();

			// TODO - Load and prep shaders for render test
			m_shaderCache->CompileAndStore("VS_GeometryPass", D3D12::Shader::TYPE::VS);
			m_shaderCache->CompileAndStore("PS_GeometryPass", D3D12::Shader::TYPE::PS);
			D3D12::Shader* vShader = m_shaderCache->GetShader("VS_GeometryPass");
			vShader->AddParameter(D3D12::Shader::RootConstant("World", 0, 16));
			vShader->AddParameter(D3D12::Shader::RootConstant("Camera", 1, 16));

			D3D12::Shader* pShader = m_shaderCache->GetShader("PS_GeometryPass");
			pShader->AddParameter(D3D12::Shader::RootConstant("MaterialConstants", 0, 8));
			pShader->AddParameter(D3D12::Shader::RootConstant("SamplerSpecs", 1, 4));
			pShader->AddRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
			//pShader->AddRenderTarget(DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);

			// Window Setup
			m_activeWindow = std::make_shared<Window::Window>(
				*m_directQueue.get(),
				*m_descriptorManager.get(),
				appInstance,
				WndProc,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXM::Vector2( 800, 600 ),
				false,
				"aZero Engine");

			m_renderSystem = m_ecs->GetSystemManager().RegisterSystem<ECS::RenderSystem>(m_ecs->GetComponentManager(), 
				/*temp...*/m_device.Get(), vShader, pShader, m_descriptorManager.get(),
				m_directQueue.get(), &m_resourceRecycler, m_activeWindow->GetClientDimensions());

			m_physicsSystem = m_ecs->GetSystemManager().RegisterSystem<ECS::PhysicsSystem>(m_ecs->GetComponentManager());

			// Camera Setup
			m_activeCamera = std::make_shared<Camera>();

			// Setup queues
			m_directContext = std::move(D3D12::CommandContext(m_device.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT));
		}
		
		~Engine()
		{
			m_directQueue->FlushCommands();
		}

		void SetActiveCamera(std::shared_ptr<Camera> camera) { m_activeCamera = camera; }

		std::shared_ptr<Camera> GetActiveCamera() { return m_activeCamera; }

		std::shared_ptr<Window::Window> GetActiveWindow() { return m_activeWindow; }

		void BeginFrame()
		{
			if (m_activeWindow)
			{
				m_activeWindow->HandleMessages();
			}

			m_frameIndex = m_frameCount % 3;
			m_renderSystem->BeginFrame(m_frameIndex);
		}

		void Update()
		{
			m_renderSystem->Update();
		}

		void EndFrame()
		{
			m_directContext.StartRecording();

			D3D12::Resource::Texture2D& currentRenderTarget = m_renderSystem->GetFrameRendertarget();
			m_activeWindow->GetSwapChain().ResolveRenderSurface(m_directContext.GetCommandList(), m_frameIndex, currentRenderTarget);

			m_directContext.StopRecording();
			m_directQueue->ExecuteContext({ m_directContext });

			m_activeWindow->GetSwapChain().Present();

			m_directQueue->ForceSignal();

			if (m_frameIndex == 2)
			{
				m_directQueue->FlushCommands();
				m_directContext.FreeCommandBuffer();
			}

			m_frameCount++;
		}

		std::shared_ptr<Scene> CreateScene(const std::string& name)
		{
			if (auto it = m_scenes.find(name); it != m_scenes.end())
				return nullptr;

			std::shared_ptr<Scene> scene(std::make_shared<Scene>());
			m_scenes.emplace(name, scene);

			return scene;
		}

		void RemoveScene(const std::string& name)
		{
			m_scenes.erase(name);
		}

		std::shared_ptr<Scene> GetScene(const std::string& name)
		{
			if (auto it = m_scenes.find(name); it != m_scenes.end())
				return it->second;

			return nullptr;
		}
	};
}