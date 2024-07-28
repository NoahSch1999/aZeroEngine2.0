#pragma once
#include <vector>
#include <list>

template <typename ElementType>
class SparseLookupArray
{
private:
	std::vector<unsigned int> m_IDtoIndexMap; // should be sized equal to highest id it gets. maps id to m_data index
	std::vector<unsigned int> m_IndexToIDMap; // should be sized equal to highest id it gets. maps index to m_data id
	std::vector<ElementType> m_Data; // should be sized equal to how many elements

	std::list<unsigned int> m_IndexFreeList; // free element indices of m_data


public:
	SparseLookupArray() = default;

	SparseLookupArray(unsigned int MaxElements, unsigned int StartElementArraySize)
	{
		//DEBUG_CHECK(StartElementArraySize >= 0)

		m_IDtoIndexMap.resize(MaxElements);
		for (unsigned int Index = 0; Index < m_IDtoIndexMap.size(); Index++)
		{
			m_IDtoIndexMap[Index] = UINT_MAX;
		}

		m_IndexToIDMap.resize(MaxElements);

		m_Data.resize(StartElementArraySize);

		for (unsigned int Index = 0; Index < StartElementArraySize; Index++)
		{
			m_IndexFreeList.push_back(Index);
		}
	}

	void Add(unsigned int ID, ElementType&& Data)
	{
		// get element index from freelist
		unsigned int ElementIndex = m_Data.size();
		if (m_IndexFreeList.empty())
		{
			m_IDtoIndexMap[ID] = ElementIndex;
			m_IndexToIDMap[ElementIndex] = ID; //
			m_Data.emplace_back(std::move(Data));
		}
		else
		{
			ElementIndex = m_IndexFreeList.front();
			m_IDtoIndexMap[ID] = ElementIndex;
			m_IndexToIDMap[ElementIndex] = ID; // 
			m_Data[ElementIndex] = std::move(Data);
			m_IndexFreeList.pop_front();
		}
	}

	void Remove(unsigned int ID)
	{
		unsigned int Index = m_IDtoIndexMap[ID];
		unsigned int LastIndex = m_Data.size() - 1;
		m_IDtoIndexMap[ID] = UINT_MAX;

		m_Data[Index] = std::move(m_Data[LastIndex]);
		m_IndexFreeList.push_back(LastIndex);

		unsigned int IDOfLast = m_IDtoIndexMap[m_IndexToIDMap[LastIndex]];
		m_IDtoIndexMap[m_IndexToIDMap[LastIndex]] = Index;
		m_IndexToIDMap[LastIndex] = IDOfLast;
	}

	ElementType& Get(unsigned int ID)
	{
		return m_Data[m_IDtoIndexMap[ID]];
	}

	const ElementType& Get(unsigned int ID) const
	{
		return m_Data[m_IDtoIndexMap[ID]];
	}
};