#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include "SystemManager.h"

namespace aZero
{
	namespace ECS
	{
		class ComponentManager;

		// TODO - Try to remove this base class
		struct ComponentArrayBase
		{
			ComponentArrayBase() = default;
			virtual ~ComponentArrayBase() = default;
			
			/** Pure virtual which is overriden by ComponentArray to allow MappedVector::Remove() without having to specify template parameters.
			@param entity The Entity to remove
			@return void
			*/
			virtual void RemoveComponent(Entity entity) = 0;
		};

		template<typename T>
		class ComponentArray : public ComponentArrayBase
		{
		private:
			DataStructures::PackedLookupArray<int, T> m_components;

		public:
			ComponentArray() = default;
			~ComponentArray() = default;

			// TODO - Implement move and copy constructors / operators
			ComponentArray(const ComponentArray&) = delete;
			ComponentArray(ComponentArray&&) = delete;
			ComponentArray operator=(const ComponentArray&) = delete;
			ComponentArray operator=(ComponentArray&&) = delete;

			void AddComponent(Entity entity, T&& component)
			{
				m_components.Add(entity.m_id, std::move(component));
			}

			/** Returns a const reference to the component of type T for the input Entity.
			* This method returns nullptr if the input Entity doesn't have a component of type T.
			@param entity The Entity to get the component for
			@return const T&
			*/
			const T& GetComponentConstRef(Entity entity) const
			{
				return m_components.GetElementConstRef(entity.m_id);
			}

			/** Returns a pointer to the component of type T for the input Entity.
			* This method returns nullptr if the input Entity doesn't have a component of type T.
			@param entity The Entity to get the component for
			@return T* const
			*/
			T& GetComponentRef(Entity entity)
			{
				return m_components.GetElementRef(entity.m_id);
			}

			std::vector<T>& GetArrayRef() { return m_components.GetReferenceInternal(); }

			const std::vector<T>& GetArrayConstRef() { return m_components.GetConstReferenceInternal(); }

			/** Checks whether or not the input Entity currently has a component of type T.
			@param entity The Entity to check
			@return bool TRUE: The Entity has a component of type T, FALSE: The Entity doesn't have a component of type T
			*/
			bool HasComponent(Entity entity) const 
			{
				return m_components.Exists(entity.m_id);
			}

			/** Removes the component for the input Entity.
			@param entity The Entity to remove
			@return void
			*/
			virtual void RemoveComponent(Entity entity) override
			{
				m_components.Remove(entity.m_id);
			}

			/** Returns number of components within the ComponentArray.
			@return int
			*/
			int NumComponents() const { return m_components.GetNumElements(); }
		};

		/** @brief Used to store, manage, and register components.
		*/
		class ComponentManager
		{
		private:
			std::unordered_map<std::type_index, std::unique_ptr<ComponentArrayBase>> m_componentArrayMap;
			std::unordered_map<std::type_index, int> m_typeToBitflag;
			SystemManager& m_systemManager;

		public:
			ComponentManager(SystemManager& systemManager)
				:m_systemManager(systemManager) { }
			~ComponentManager() = default;

			/** Registers a new component type of type T for the ComponentManager.
			* This should be called once for each component that the ComponentManager should support.
			@return void
			*/
			template<typename T>
			void RegisterComponentType()
			{
				const std::type_index typeIndex = std::type_index(typeid(T));

				m_typeToBitflag.emplace(typeIndex, m_componentArrayMap.size());
				m_componentArrayMap.emplace(typeIndex, std::make_unique<ComponentArray<T>>());
			}

			/** Checks whether or not the input Entity currently has a component of type T.
			@param entity The Entity to check
			@return bool TRUE: The Entity has a component of type T, FALSE: The Entity doesn't have a component of type T
			*/
			template<typename T>
			bool HasComponent(const Entity& entity) const
			{
				return entity.m_componentMask.test(static_cast<size_t>(m_typeToBitflag.at(std::type_index(typeid(T)))));
			}

			/** Adds a component of type T to the input Entity.
			@param entity The Entity to add the component to
			@return void
			*/
			template<typename T>
			void AddComponent(Entity& entity)
			{
				entity.m_componentMask.set(static_cast<size_t>(m_typeToBitflag.at(std::type_index(typeid(T)))), true);

				ComponentArray<T>* const componentArray = static_cast<ComponentArray<T>*>(m_componentArrayMap.at(std::type_index(typeid(T))).get());

				componentArray->AddComponent(entity, std::move(T()));
			}

			/** Adds a component of type T to the input Entity.
			* Initiates the new component with the input data.
			@param entity The Entity to add the component to
			@param data Initial data for the component
			@return void
			*/
			template<typename T>
			void AddComponent(Entity& entity, T&& data)
			{
				entity.m_componentMask.set(static_cast<size_t>(m_typeToBitflag.at(std::type_index(typeid(T)))), true);

				ComponentArray<T>* const componentArray = static_cast<ComponentArray<T>*>(m_componentArrayMap.at(std::type_index(typeid(T))).get());

				componentArray->AddComponent(entity, std::move(data));
			}

			/** Returns a const reference to the component of type T for the input Entity.
			* Consider using ComponentManager::GetComponentArray<T>() in conjunctions with ComponentArray::GetComponentConst() to avoid additional lookup time per component.
			* This will avoid the additional ComponentArray<T> lookup time which this method has.
			@param entity The Entity to get the component for
			@return const T&
			*/
			template<typename T>
			const T& GetComponentConstRef(const Entity& entity) const
			{
				return static_cast<ComponentArray<T>*>(m_componentArrayMap.at(std::type_index(typeid(T))).get())->GetComponentConstRef(entity);
			}

			/** Returns a reference to the component of type T for the input Entity.
			* Consider using ComponentManager::GetComponentArray<T>() in conjunctions with ComponentArray::GetComponentRef() to avoid additional lookup time per component.
			* This will avoid the additional ComponentArray<T> lookup time which this method has.
			@param entity The Entity to get the component for
			@return T&
			*/
			template<typename T>
			T& GetComponentRef(const Entity& entity)
			{
				return static_cast<ComponentArray<T>*>(m_componentArrayMap.at(std::type_index(typeid(T))).get())->GetComponentRef(entity);
			}

			/** Removes the component of type T for the input Entity.
			*
			* NOTE! Contrary to ComponentManager::RemoveComponent() it doesn't remove the Entity from relevant Systems.
			* System::Unregister() has to be called before the System in question tries to acquire the removed component.
			* This method should be used to batch multiple remove component calls without immediately removing them from a System for performance reasons.
			*
			@param entity The Entity to remove the component for
			@return void
			*/
			template<typename T>
			void RemoveComponentWithoutUnRegister(Entity& entity)
			{
				entity.m_componentMask.set(static_cast<size_t>(m_typeToBitflag.at(std::type_index(typeid(T)))), false);

				ComponentArrayBase* const base = m_componentArrayMap.at(std::type_index(typeid(T))).get();
				base->RemoveComponent(entity);
			}

			/** Removes the component of type T for the input Entity.
			@param entity The Entity to remove the component for
			@return void
			*/
			template<typename T>
			void RemoveComponent(Entity& entity)
			{
				entity.m_componentMask.set(static_cast<size_t>(m_typeToBitflag.at(std::type_index(typeid(T)))), false);

				ComponentArrayBase* const base = m_componentArrayMap.at(std::type_index(typeid(T))).get();
				base->RemoveComponent(entity);

				m_systemManager.RemoveEntityFromSystems(entity);
			}

			/** Removes the component of type T matching the input std::type_index for the input Entity.
			@param entity The Entity to remove the component for
			@param typeIndex The std::type_index which matches a registered type T component
			@return void
			*/
			void RemoveComponent(Entity& entity, std::type_index typeIndex)
			{
				entity.m_componentMask.set(static_cast<size_t>(m_typeToBitflag.at(typeIndex)), false);

				ComponentArrayBase* const base = m_componentArrayMap.at(typeIndex).get();
				base->RemoveComponent(entity);

				m_systemManager.RemoveEntityFromSystems(entity);
			}

			/** Returns a reference to a ComponentArray<T>.
			@return ComponentArray<T>&
			*/
			template<typename T>
			ComponentArray<T>& GetComponentArray()
			{
				return *static_cast<ComponentArray<T>*>(m_componentArrayMap.at(std::type_index(typeid(T))).get());
			}

			/** Returns the std::bitset bit index for the component of type T.
			@return int
			*/
			template<typename T>
			int GetComponentBit() const
			{
				return m_typeToBitflag.at(std::type_index(typeid(T)));
			}

			/** Returns a const reference to the map containing the std::type_index and bitflags for all registered components.
			@return std::unordered_map<std::type_index, short>&
			*/
			const std::unordered_map<std::type_index, int>& GetBitFlagMap() const { return m_typeToBitflag; }
		};
	}
}