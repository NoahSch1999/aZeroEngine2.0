#pragma once
#include <queue>
#include "ComponentManager.h"

namespace aZero
{
	namespace ECS
	{
		/** @brief Creates and recycles Entity objects.
		*/
		template<typename ComponentManagerSpecialization>
		class EntityManager
		{
		private:
			int m_currentMax = 0;
			std::queue<int> m_freeEntityIDs;
			ComponentManagerSpecialization& m_componentManager;

		public:
			EntityManager(ComponentManagerSpecialization& componentManager)
				:m_componentManager(componentManager) { }
			~EntityManager() = default;

			// TODO - Implement move and copy constructors / operators
			EntityManager(const EntityManager&) = delete;
			EntityManager(EntityManager&&) = delete;
			EntityManager operator=(const EntityManager&) = delete;
			EntityManager operator=(EntityManager&&) = delete;

			/** Creates a new Entity object with a unique ID.
			@return Entity
			*/
			Entity CreateEntity()
			{
				if (!m_freeEntityIDs.empty())
				{
					const int id = m_freeEntityIDs.front();
					m_freeEntityIDs.pop();
					return Entity(id);
				}

				const int id = m_currentMax;
				m_currentMax++;
				return Entity(id);
			}

			/** Recycles the input Entity so that it's unique ID can be reused when calling EntityManager::CreateEntity().
			* Removes all components for the Entity and unbinds it from all registered System subclasses.
			* Changes the input Entity object ID to -1 to indicate that it is recycled.
			* Doesn't do anything if the input Entity ID is -1.
			@param The Entity which should be recycled
			@return void
			*/
			void RemoveEntity(Entity& entity)
			{
				if (entity.GetID() == -1)
					return;

				/*for (const auto& [index, bitFlag] : m_componentManager.GetBitFlagMap())
				{
					if (entity.m_componentMask.test(bitFlag))
					{
						m_componentManager.RemoveComponent(entity, index);
					}
				}*/

				// TODO - Check if this is unnecessary
				//m_systemManager.RemoveEntityFromSystems(entity);

				m_freeEntityIDs.push(entity.GetID());
			}
		};
	}
}