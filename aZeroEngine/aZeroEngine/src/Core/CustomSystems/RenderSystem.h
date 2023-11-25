#pragma once
#include "../../ECS/System.h"
#include "../ComponentTypes/ComponentTypes.h"
#include "../../Helpers/NoneCopyable.h"

namespace aZero
{
	namespace ECS
	{
		class RenderSystem : public System, public Helpers::NoneCopyable
		{
		private:
			void CalculateVisibility()
			{

			}

			void Render()
			{

			}

		public:
			RenderSystem()
				:System(Singleton::ECS->Get().GetComponentManager())
			{
				// Signature Setup
				m_componentMask.set(m_componentManager.GetComponentBit<Component::Transform>());
				m_componentMask.set(m_componentManager.GetComponentBit<Component::Mesh>());
				m_componentMask.set(m_componentManager.GetComponentBit<Component::Material>());
			}

			virtual void Update() override
			{
				
			}

			void Register(const Entity& entity)
			{

			}

			void UnRegister(const Entity& entity)
			{

			}
		};
	}
}