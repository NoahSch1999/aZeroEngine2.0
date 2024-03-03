#pragma once
#include <Windows.h>
#include <bitset>
#include "GlobalMacrosECS.h"

#include <tuple>
#include <iostream>

namespace aZero
{
	namespace ECS
	{


		typedef unsigned int EntityID;

		class Entity
		{

		private:
			EntityID m_ID = UINT_MAX;
			std::bitset<MAXCOMPONENTS> m_ComponentMask;

		public:
			Entity(EntityID ID)
				:m_ID(ID)
			{
			}

			void SetComponentBit(int ComponentBit, bool Value)
			{
				DEBUG_CHECK(ComponentBit < MAXCOMPONENTS)
				m_ComponentMask.set(ComponentBit, Value);
			}

			EntityID GetID()const { return m_ID; }

			const std::bitset<MAXCOMPONENTS>& GetComponentMask() const { return m_ComponentMask; }
		};
	}
}