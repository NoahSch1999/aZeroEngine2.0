#pragma once
#include "../../ECS/ECS.h"
#include "../ComponentTypes/ComponentTypes.h"
#include "../../Helpers/NoneCopyable.h"

namespace aZero
{
	namespace ECS
	{
		class PhysicsSystem : public System, public Helpers::NoneCopyable
		{
		private:

		public:
			PhysicsSystem(ComponentManager& componentManager)
				:System(componentManager)
			{
				// Signature Setup
				m_componentMask.set(m_componentManager.GetComponentBit<Component::TransformComponent>());
				m_componentMask.set(m_componentManager.GetComponentBit<Component::MeshComponent>());
				m_componentMask.set(m_componentManager.GetComponentBit<Component::RigidBodyComponent>());
			}

			virtual void Update() override
			{

			}

			virtual void OnRegister() override
			{
				printf("Entity registered for PhysicsSystem\n");
			}

			virtual void OnUnRegister() override
			{
				printf("Entity unregistered for PhysicsSystem\n");
			}
		};
	}
}