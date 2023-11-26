#pragma once
#include <fstream>

#include "Singleton/Singleton.h"

namespace aZero
{
	class Scene
	{
	private:
		DataStructures::PackedLookupArray<std::string, ECS::Entity> m_entities;

	public:
		Scene()
		{

		}

		~Scene()
		{

		}

		// TODO - Implement move and copy constructors / operators
		Scene(const Scene&) = delete;
		Scene(Scene&&) = delete;
		Scene operator=(const Scene&) = delete;
		Scene operator=(Scene&&) = delete;

		void Load(std::ifstream& inFile)
		{
			if (inFile.is_open())
			{
				// Load data from the stream...


			}
		}

		void Save(std::ofstream& outFile)
		{
			if (outFile.is_open())
			{
				// Save scene to the file...


			}
		}

		ECS::Entity& GetEntity(const std::string& name) { return m_entities.GetElementRef(name); }
	};
}