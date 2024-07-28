#pragma once
#include <tuple>
#include "ComponentArray.h"

namespace aZero
{
	namespace ECS
	{
		template<typename TargetType, typename... TupleTypes>
		constexpr int GetIndexOfTupleElement(const std::tuple<TupleTypes...>& Tuple)
		{
			bool found = false;
			int count = 0;

			((!found ? (++count, found = std::is_same_v<TargetType, TupleTypes>) : 0), ...);

			return found ? count - 1 : throw;
		}

		template<class... Args>
		class ComponentManager
		{
			std::tuple<ComponentArray<Args>...> m_ComponentArrays;

		public:
			ComponentManager() = default;
			~ComponentManager() = default;

			template<typename ComponentType>
			constexpr int GetComponentBit() const
			{
				return GetIndexOfTupleElement<ComponentArray<ComponentType>>(m_ComponentArrays);
			}

			template<typename ComponentType>
			ComponentArray<ComponentType>& GetComponentArray()
			{
				return std::get<ComponentArray<ComponentType>>(m_ComponentArrays);
			}

			template<typename ComponentType>
			void AddComponent(Entity& Ent, ComponentType&& Component)
			{
				GetComponentArray<ComponentType>().AddComponent(Ent, std::forward<ComponentType>(Component));
				Ent.SetComponentBit(GetComponentBit<ComponentType>(), true);
			}

			template<typename ComponentType>
			void RemoveComponent(Entity& Ent)
			{
				GetComponentArray<ComponentType>().RemoveComponent(Ent);
				Ent.SetComponentBit(GetComponentBit<ComponentType>(), false);
			}

			template<typename ComponentType>
			bool HasComponent(Entity& Ent)
			{
				return Ent.GetComponentMask().test(GetComponentBit<ComponentType>());
			}
		};
	}
}