#pragma once
#include "Entity.h"
#include "../DataStructures/PackedLookupArray.h"

namespace aZero
{
	namespace ECS
	{
		class ComponentManager;

		/** @brief An abstract base class which can be inherited and registered with SystemManager::RegisterSystem().
		* Each inheriting subclass should specify what set of components an Entity needs to have to be bound when System::Bind() is called.
		* This can be done by writing "m_componentMask.set(m_componentManager.GetComponentBit<ComponentClassName>());" inside the subclass constructor.
		* A component have to be registered using ComponentManager::RegisterComponent() prior to a System subclass constructor being called for ComponentManager::GetComponentBit() to work.
		*/
		class System
		{
			friend class SystemManager;

		protected:
			// TODO - Replace macro
			std::bitset<MAXCOMPONENTS> m_componentMask;
			ComponentManager& m_componentManager;
			DataStructures::PackedLookupArray<int, Entity> m_entities;

		public:
			// TODO - Remove need to send in compmanager through subclass
			System(ComponentManager& componentManager)
				:m_componentManager(componentManager) { }

			virtual ~System() = default;

			/** Pure virtual method which should be overridden by the inheriting subclass.
			* Defines custom behavior related to the inheriting System when registering an Entity.
			@return void
			*/
			virtual void OnRegister() = 0;

			/** Pure virtual method which should be overridden by the inheriting subclass.
			* Defines custom behavior related to the inheriting System when unregistering an Entity.
			@return void
			*/
			virtual void OnUnRegister() = 0;
			
			/** Pure virtual method which should be overridden by the inheriting subclass.
			@return void
			*/
			virtual void Update() = 0;

			/** Registers the input Entity if it's m_componentMask matches the m_componentMask set in the System subclass constructor.
			@param entity The Entity which should be bound
			@return bool TRUE: Entity is either already bound or bound during this call of System::Register(). FALSE: Entity::m_componentMask isn't compatible with System::m_componentMask
			*/
			bool Register(const Entity& entity)
			{
				if (!IsRegistered(entity))
				{
					std::bitset<MAXCOMPONENTS> bitwiseResult = m_componentMask & entity.m_componentMask;
					if (bitwiseResult != m_componentMask)
						return false;

					m_entities.AddCopy(entity.m_id, entity);

					OnRegister();

					return true;
				}

				return false;
			}

			/** Unregisters the input Entity.
			@param entity The Entity which should be unbound
			@return void
			*/
			void UnRegister(const Entity& entity)
			{
				if (IsRegistered(entity))
				{
					OnUnRegister();

					m_entities.Remove(entity.m_id);
				}
			}

			bool IsRegistered(const Entity& entity)
			{
				return m_entities.Exists(entity.m_id);
			}

			int NumEntitiesBound() const { return m_entities.GetNumElements(); }
		};
	}
}