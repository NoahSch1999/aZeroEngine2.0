#pragma once
#include "Entity.h"
#include "PackedLookupArray.h"

namespace aZero
{
	namespace ECS
	{
		/** @brief A class that stores components in a sparse array which can be accessed at constant time using an EntityID */
		template<typename ComponentType>
		class ComponentArray
		{
		private:
			aZero::DataStructures::PackedLookupArray<EntityID, ComponentType> m_ComponentArray;

		public:
			ComponentArray()
			{

			}
			~ComponentArray() = default;

			/** Returns a referece to a component tied to the input Entity
			@param Entity
			@return ComponentType&
			*/
			ComponentType& GetComponent(const Entity& Ent)
			{
				return m_ComponentArray.GetElementRef(Ent.GetID());
			}


			/** Returns a const referece to a component tied to the input Entity
			@param Entity
			@return const ComponentType&
			*/
			const ComponentType& GetComponent(const Entity& Ent) const
			{
				return m_ComponentArray.GetElementConstRef(Ent.GetID());
			}

			/** Returns a referece to the internal sparse array containing all the components
			@return std::vector<ComponentType>&
			*/
			std::vector<ComponentType>& GetInternalArray()
			{
				return m_ComponentArray.GetReferenceInternal();
			}

			/** Returns a const referece to the internal sparse array containing all the components
			@return const std::vector<ComponentType>&
			*/
			const std::vector<ComponentType>& GetInternalArray() const
			{
				return m_ComponentArray.GetConstReferenceInternal();
			}

			/** Adds the input component to the input Entity.
			* Overwrites with the new component if the input Entity alread has a component of that type.
			@param Entity&
			@param ComponentType&&
			@return void
			*/
			void AddComponent(Entity& Ent, ComponentType&& Component)
			{
				// TODO - Set bitmask value

				m_ComponentArray.Add(Ent.GetID(), std::forward<ComponentType>(Component));
			}

			/** Removes the component of the ComponentArray's type that is tied with the input Entity.
			* This function is safe to use on an Entity that doesn't have a component of the specific type.
			@param Entity&
			@return void
			*/
			void RemoveComponent(Entity& Ent)
			{
				// TODO - Set bitmask value

				m_ComponentArray.Remove(Ent.GetID());
			}

			/** Returns whether or not the input Entity has a component of the specific type.
			@param const Entity&
			@return bool
			*/
			bool HasComponent(const Entity& Ent)
			{
				return m_ComponentArray.Exists(Ent.GetID());
			}
		};
	}
}