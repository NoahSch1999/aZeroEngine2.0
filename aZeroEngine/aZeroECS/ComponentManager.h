#pragma once
#include <tuple>
#include "ComponentArray.h"

namespace aZero
{
	namespace ECS
	{
		template <class T, class Tuple>
		struct Index;

		template <class T, class... Types>
		struct Index<T, std::tuple<T, Types...>> 
		{
			static const std::size_t value = 0;
		};

		template <class T, class U, class... Types>
		struct Index<T, std::tuple<U, Types...>> 
		{
			static const std::size_t value = 1 + Index<T, std::tuple<Types...>>::value;
		};

		template<class... Args>
		class ComponentManager
		{
			std::tuple<ComponentArray<Args>...> m_ComponentArrays;

		public:
			ComponentManager() = default;
			~ComponentManager() = default;

			template<typename ComponentType>
			int GetComponentBit()
			{
				return Index<ComponentType, std::tuple<ComponentType, ComponentArray<Args>...>>::value;
			}

			template<typename ComponentType>
			ComponentArray<ComponentType>& GetComponentArray()
			{
				return std::get<ComponentArray<ComponentType>>(m_ComponentArrays);
			}
		};
	}
}