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
			RenderSystem(ComponentManager& componentManager)
				:System(componentManager)
			{
				// Signature Setup
				m_componentMask.set(m_componentManager.GetComponentBit<Component::Transform>());
				m_componentMask.set(m_componentManager.GetComponentBit<Component::Mesh>());
				m_componentMask.set(m_componentManager.GetComponentBit<Component::Material>());
			}

			virtual void Update() override
			{
				
			}

			virtual void OnRegister() override
			{
				printf("Entity registered for RenderSystem\n");
			}

			virtual void OnUnRegister() override
			{
				printf("Entity unregistered for RenderSystem\n");
			}
		};
	}
}