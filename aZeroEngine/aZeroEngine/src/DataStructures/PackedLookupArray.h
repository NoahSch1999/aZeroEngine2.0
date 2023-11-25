#pragma once
#include <unordered_map>
#include <utility>

namespace aZero
{
	namespace DataStructures
	{
		/** @brief An unordered map which stores the elements in a contiguous packed array.
		*/
		template<typename KeyType, typename ElementType>
		class PackedLookupArray
		{
		private:
			std::vector<ElementType> m_elements;
			std::unordered_map<KeyType, int> m_keyToIndex;
			std::unordered_map<int, KeyType> m_indexToKey;
			int m_numElements = 0;
			int m_perIncrementSize = 1;

		public:
			/**Initiates the PackedLookupArray instance.
			@param startSize Number of elements to pre-allocate for.
			@param perIncrementSize How many empty elements that should be allocated for whenever the internal std::vector is full.
			*/
			PackedLookupArray(std::size_t startSize = 0, std::size_t perIncrementSize = 1)
				:m_perIncrementSize(perIncrementSize)
			{
				m_elements.reserve(startSize);
			}

			~PackedLookupArray() = default;

			PackedLookupArray(const PackedLookupArray& other)
			{
				m_elements.assign(other.m_elements.begin(), other.m_elements.end());

				for (const auto& [key, index] : other.m_keyToIndex)
				{
					m_keyToIndex.emplace(std::make_pair(key, index));
				}

				for (const auto& [index, key] : other.m_indexToKey)
				{
					m_indexToKey.emplace(std::make_pair(index, key));
				}

				m_numElements = other.m_numElements;
				m_perIncrementSize = other.m_perIncrementSize;
			}

			PackedLookupArray(PackedLookupArray&& other)
			{
				m_elements = std::move(other.m_elements);
				m_keyToIndex = std::move(other.m_keyToIndex);
				m_indexToKey = std::move(other.m_indexToKey);
				m_numElements = other.m_numElements;
				m_perIncrementSize = other.m_perIncrementSize;
			}

			PackedLookupArray operator=(const PackedLookupArray& other)
			{
				if (this != &other)
				{
					m_elements.assign(other.m_elements.begin(), other.m_elements.end());

					for (const auto& [key, index] : other.m_keyToIndex)
					{
						m_keyToIndex.emplace(std::make_pair(key, index));
					}

					for (const auto& [index, key] : other.m_indexToKey)
					{
						m_indexToKey.emplace(std::make_pair(index, key));
					}

					m_numElements = other.m_numElements;
					m_perIncrementSize = other.m_perIncrementSize;
				}

				return *this;
			}

			PackedLookupArray operator=(PackedLookupArray&& other)
			{
				if (this != &other)
				{
					m_elements = std::move(other.m_elements);
					m_keyToIndex = std::move(other.m_keyToIndex);
					m_indexToKey = std::move(other.m_indexToKey);
					m_numElements = other.m_numElements;
					m_perIncrementSize = other.m_perIncrementSize;
				}

				return *this;
			}

			/**Adds an element to the map with the specified key.
			* Time complexity is O(1) on average except when the internal std::vector is full.
			* The input element won't be added if an element is already associated with the input key.
			@param key Key to associate the element with.
			@param data The element data.
			@return void
			*/
			void Add(const KeyType& key, ElementType&& data)
			{
				if (auto foundPairIter = m_keyToIndex.find(key); foundPairIter == m_keyToIndex.end())
				{
					if (m_numElements == m_elements.capacity())
					{
						m_elements.reserve(m_numElements + m_perIncrementSize);
					}

					m_keyToIndex.emplace(key, m_numElements);
					m_indexToKey.emplace(m_numElements, key);
					m_elements.emplace_back(std::forward<ElementType>(data));
					m_numElements++;
				}
			}

			/**Removes an element associated with the input key value.
			* Time complexity is O(1) on average.
			@param key Key associated with the element to remove.
			@return void
			*/
			void Remove(const KeyType& key)
			{
				if (auto foundPairIter = m_keyToIndex.find(key); foundPairIter != m_keyToIndex.end())
				{
					const int indexToOverwrite = foundPairIter->second;
					const int lastIndex = m_numElements - 1;

					if (lastIndex != indexToOverwrite)
					{
						const KeyType idOfLast = m_indexToKey.at(lastIndex);
						m_elements.at(indexToOverwrite) = std::move(m_elements.at(lastIndex));
						m_keyToIndex.at(idOfLast) = indexToOverwrite;
						m_indexToKey.at(indexToOverwrite) = idOfLast;
					}

					m_keyToIndex.erase(key);
					m_indexToKey.erase(lastIndex);

					m_numElements--;
				}
			}

			bool Exists(const KeyType& key) const
			{
				if (const auto foundPairIter = m_keyToIndex.find(key); foundPairIter != m_keyToIndex.end())
				{
					return true;
				}
				return false;
			}

			/**Changes the amount of empty elements allocated whenever the internal std::vector is full.
			@param newPerIncrementSize The new amount that should be allocated (in elements).
			@return void
			*/
			void ChangeResizeSize(std::size_t newPerIncrementSize)
			{
				m_perIncrementSize = newPerIncrementSize;
			}

			/**Shrinks the internal std::vector to fit the held elements associated with keys.
			@return void
			*/
			void ShrinkToFit()
			{
				m_elements.resize(m_numElements);
			}

			/**Allocates the internal std::vector capacity.
			* Useful when you want to manually control when the internal std::vector should allocate more memory.
			@param numElements The number of elements to allocate space for.
			@return void
			*/
			void Extend(std::size_t numElements)
			{
				m_elements.resize(m_elements.capacity() + numElements);
			}

			/**Returns a copy of the element matching the key.
			@param key Key associated with an element.
			@return ElementType
			*/
			ElementType GetElementCopy(const KeyType& key) const { return m_elements[m_keyToIndex.at(key)]; }

			/**Returns a reference to the element matching the key.
			@param key Key associated with an element.
			@return ElementType&
			*/
			ElementType& GetElementRef(const KeyType& key) { return m_elements[m_keyToIndex.at(key)]; }

			/**Returns a const reference to the element matching the key.
			@param key Key associated with an element.
			@return ElementType&
			*/
			const ElementType& GetElementConstRef(const KeyType& key) const { return m_elements[m_keyToIndex.at(key)]; }

			/**Returns a copy of the internal std::vector.
			* The std::vector is contiguous and can be iterated through safely.
			@return std::vector<ElementType>
			*/
			std::vector<ElementType> GetCopyInternal() const { return m_elements; }

			/**Returns a referencce to the internal std::vector.
			* The std::vector is contiguous and can be iterated through safely.
			@return std::vector<ElementType>&
			*/
			std::vector<ElementType>& GetReferenceInternal() { return m_elements; }

			/**Returns a const referencce to the internal std::vector.
			* The std::vector is contiguous and can be iterated through safely.
			@return std::vector<ElementType>&
			*/
			const std::vector<ElementType>& GetConstReferenceInternal() const { return m_elements; }

			/**Returns the number of elements within the internal std::vector that are currently associated with keys.
			@return std::size_t
			*/
			std::size_t GetNumElements() const { return m_numElements; }
		};

	}
}