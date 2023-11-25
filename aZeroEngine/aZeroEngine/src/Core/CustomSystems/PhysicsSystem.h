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
			PhysicsSystem()
				:System(Singleton::ECS->Get().GetComponentManager())
			{
				// Signature Setup
				m_componentMask.set(m_componentManager.GetComponentBit<Component::Transform>());
				m_componentMask.set(m_componentManager.GetComponentBit<Component::Mesh>());
				m_componentMask.set(m_componentManager.GetComponentBit<Component::RigidBody>());
			}

			virtual void Update() override
			{

			}
		};
	}
}