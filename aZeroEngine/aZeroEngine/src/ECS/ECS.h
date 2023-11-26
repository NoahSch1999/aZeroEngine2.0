#pragma once
#include "EntityManager.h"

namespace aZero
{
	namespace ECS
	{
		class ECS
		{
		private:
			std::unique_ptr<EntityManager> m_entityManager = nullptr;
			std::unique_ptr<ComponentManager> m_componentManager = nullptr;
			std::unique_ptr<SystemManager> m_systemManager = nullptr;

		public:
			ECS()
			{
				m_systemManager = std::make_unique<SystemManager>();
				m_componentManager = std::make_unique<ComponentManager>(*m_systemManager.get());
				m_entityManager = std::make_unique<EntityManager>(*m_componentManager.get(), *m_systemManager.get());
			}

			~ECS() = default;

			// TODO - Implement move and copy constructors / operators
			ECS(const ECS&) = delete;
			ECS(ECS&&) = delete;
			ECS operator=(const ECS&) = delete;
			ECS operator=(ECS&&) = delete;

			EntityManager& GetEntityManager() { return *m_entityManager.get(); }
			ComponentManager& GetComponentManager() { return *m_componentManager.get(); }
			SystemManager& GetSystemManager() { return *m_systemManager.get(); }
		};
	}
}