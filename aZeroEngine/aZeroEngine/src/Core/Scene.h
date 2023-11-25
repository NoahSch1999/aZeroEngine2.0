#pragma once
#include <fstream>

#include "Singleton/Singleton.h"

namespace aZero
{
	class Scene
	{
	private:
		DataStructures::PackedLookupArray<std::string, ECS::Entity> m_entities;

	public:
		Scene()
		{

		}

		~Scene()
		{

		}

		// TODO - Implement move and copy constructors / operators
		Scene(const Scene&) = delete;
		Scene(Scene&&) = delete;
		Scene operator=(const Scene&) = delete;
		Scene operator=(Scene&&) = delete;

		void Load(std::ifstream& inFile)
		{
			if (inFile.is_open())
			{
				// Load data from the stream...


			}
		}

		void Save(std::ofstream& outFile)
		{
			if (outFile.is_open())
			{
				// Save scene to the file...


			}
		}

		ECS::Entity& GetEntity(const std::string& name) { return m_entities.GetElementRef(name); }

		template<typename T>
		void AddComponent(ECS::Entity& entity)
		{
			Singleton::ECS->Get().GetComponentManager().AddComponent<T>(entity);
		}

		template<typename T>
		void AddComponent(ECS::Entity& entity, T&& component)
		{
			if (Singleton::ECS->Get().GetComponentManager().HasComponent<T>(entity))
			{
				Singleton::ECS->Get().GetComponentManager().GetComponentRef<T>(entity) = component;
			}
			else
			{
				Singleton::ECS->Get().GetComponentManager().AddComponent<T>(entity, std::forward<T>(component));
			}
		}

		template<typename T>
		void HasComponent(ECS::Entity& entity) const
		{
			return Singleton::ECS->Get().GetComponentManager().HasComponent<T>(entity);
		}

		template<typename T>
		void RemoveComponent(ECS::Entity& entity)
		{
			return Singleton::ECS->Get().GetComponentManager().RemoveComponent<T>(entity);
		}

		template<typename T>
		const T& GetComponentConstRef(const ECS::Entity& entity) const
		{
			return Singleton::ECS->Get().GetComponentManager().GetComponentConstRef(entity);
		}

		template<typename T>
		T& GetComponentRef(const ECS::Entity& entity)
		{
			return Singleton::ECS->Get().GetComponentManager().GetComponentRef(entity);
		}

	};
}