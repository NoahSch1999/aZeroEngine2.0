#pragma once
#include "Entity.h"
#include "PackedLookupArray.h"

namespace aZero
{
	namespace ECS
	{
		template<typename ComponentType>
		class ComponentArray
		{
		private:
			aZero::DataStructures::PackedLookupArray<EntityID, ComponentType> m_ComponentArray;

		public:
			ComponentArray() = default;
			~ComponentArray() = default;

			ComponentType& GetComponent(const Entity& Ent)
			{
				DEBUG_CHECK(HasComponent(Ent))
				return m_ComponentArray.GetElementRef(Ent.GetID());
			}

			const ComponentType& GetComponent(const Entity& Ent) const
			{
				DEBUG_CHECK(HasComponent(Ent))
				return m_ComponentArray.GetElementConstRef(Ent.GetID());
			}

			std::vector<ComponentType>& GetInternalArray()
			{
				return m_ComponentArray.GetReferenceInternal();
			}

			const std::vector<ComponentType>& GetInternalArray() const
			{
				return m_ComponentArray.GetConstReferenceInternal();
			}

			void AddComponent(Entity& Ent, ComponentType&& InitialData)
			{
				DEBUG_CHECK(!HasComponent(Ent))
				//Ent.SetComponentBit(bit, true);
				m_ComponentArray.Add(Ent.GetID(), std::forward<ComponentType>(InitialData));
			}

			void RemoveComponent(Entity& Ent)
			{
				DEBUG_CHECK(HasComponent(Ent))
				//Ent.SetComponentBit(bit, false);
				m_ComponentArray.Remove(Ent.GetID());
			}

			bool HasComponent(const Entity& Ent)
			{
				return m_ComponentArray.Exists(Ent.GetID());
			}
		};
	}
}