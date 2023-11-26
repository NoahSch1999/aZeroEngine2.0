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
		std::unique_ptr<D3D12::Shader> m_shaderCache = nullptr;

		// Scenes
		std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;

		// Temporary
		std::shared_ptr<Camera> m_activeCamera = nullptr;

	public:
		Engine()
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

			// ECS Setup
			m_ecs->GetComponentManager().RegisterComponentType<Component::Transform>();
			m_ecs->GetComponentManager().RegisterComponentType<Component::Mesh>();
			m_ecs->GetComponentManager().RegisterComponentType<Component::Material>();
			m_ecs->GetComponentManager().RegisterComponentType<Component::PointLight>();
			m_ecs->GetComponentManager().RegisterComponentType<Component::SpotLight>();
			m_ecs->GetComponentManager().RegisterComponentType<Component::DirectionalLight>();
			m_ecs->GetComponentManager().RegisterComponentType<Component::RigidBody>();

			m_renderSystem = m_ecs->GetSystemManager().RegisterSystem<ECS::RenderSystem>(m_ecs->GetComponentManager());
			m_physicsSystem = m_ecs->GetSystemManager().RegisterSystem<ECS::PhysicsSystem>(m_ecs->GetComponentManager());

			// Asset Cache Setup
			m_meshCache = std::make_unique<MeshCache>();
			m_textureCache = std::make_unique<TextureCache>();
			m_materialCache = std::make_unique<MaterialCache>();
			m_audioCache = std::make_unique<AudioCache>();
			m_shaderCache = std::make_unique<D3D12::Shader>();

			// Window Setup
			m_activeWindow = std::make_shared<Window::Window>();

			// Camera Setup
			m_activeCamera = std::make_shared<Camera>();
		}
		
		~Engine()
		{

		}

		void SetActiveCamera(std::shared_ptr<Camera> camera) { m_activeCamera = camera; }

		std::shared_ptr<Window::Window> GetActiveWindow() { return m_activeWindow; }

		void BeginFrame()
		{
			// Even needed? The input isnt handled by the engine anyways...


		}

		void Update()
		{
			// Update all systems
				// Calculate Physics
				// Render scene
		}

		void EndFrame()
		{
			// Aquire Window and copy the renderers output to the back buffer


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