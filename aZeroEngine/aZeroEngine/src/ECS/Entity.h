#pragma once
#include <bitset>

namespace aZero
{
	namespace ECS
	{
		#define MAXCOMPONENTS 32

		/** @brief Contains a unique ID and a bitmask containing which components the Entity currently has.
		*/
		struct Entity
		{
			Entity(int id)
				:m_id(id) { }

			int m_id = -1;
			std::bitset<MAXCOMPONENTS> m_componentMask;
		};
	}
}