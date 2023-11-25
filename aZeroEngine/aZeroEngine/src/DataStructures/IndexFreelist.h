#pragma once
#include <set>

namespace aZero
{
	namespace DataStructures
	{
		class IndexFreelist
		{
		private:
			std::set<int> m_freeIDs;
			int m_currentMax = 0;

		public:
			IndexFreelist(const int beginIndex = 0)
				:m_currentMax(beginIndex)
			{

			}

			~IndexFreelist()
			{

			}

			int GetFreeID()
			{
				if (!m_freeIDs.empty())
				{
					const int ID = *m_freeIDs.begin();
					m_freeIDs.erase(ID);
					return ID;
				}

				const int ID = m_currentMax;
				m_currentMax++;
				return ID;
			}

			void RecycleID(const int ID)
			{
				if (ID < m_currentMax)
				{
					m_freeIDs.insert(ID);
				}
			}

			// TODO - Define sm~mooove operator ;)
			IndexFreelist(IndexFreelist&&) = delete;
			IndexFreelist operator=(IndexFreelist&&) = delete;
		};
	}
}