#pragma once
#include "System.h"

namespace aZero
{
	namespace ECS
	{
		/** @brief Registers System subclasses for aZeroECS.
		*/
		class SystemManager
		{
		private:
			std::unordered_map<std::type_index, std::shared_ptr<System>> m_registeredSystems;

		public:
			SystemManager() = default;

			/** Registers the input System subclass and returns a std::shared_ptr to the shared memory block.
			@param componentManager The ComponentManager which the System subclass can use to access components
			@param args An arbitrary number of arguments which will be passed to the constructor of System subclass after componentManager
			@return std::shared_ptr<T>
			*/
			template<typename T, typename...Args>
			std::shared_ptr<T> RegisterSystem(Args&&...args)
			{
				std::shared_ptr<T> newSystem = std::make_shared<T>(args...);

				m_registeredSystems.emplace(std::type_index(typeid(T)), newSystem);

				return newSystem;
			}

			void AddEntityToSystems(const Entity& entity)
			{
				for (auto& system : m_registeredSystems)
				{
					system.second->Register(entity);
				}
			}

			void RemoveEntityFromSystems(const Entity& entity)
			{
				for (auto& system : m_registeredSystems)
				{
					system.second->UnRegister(entity);
				}
			}
		};
	}
}