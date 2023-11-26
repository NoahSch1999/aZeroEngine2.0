#pragma once
#include <memory>

#include "../../Helpers/NoneCopyable.h"
#include "../../ECS/ECS.h"
#include "../D3D12Wrappers/ResourceRecycler.h"
#include "../D3D12Wrappers/Descriptors/DescriptorManager.h"

namespace aZero
{
	namespace Singleton
	{
		template<typename InstanceType>
		class Singleton : public Helpers::NoneCopyable
		{
		private:
			static InstanceType* m_instance;

		public:
			Singleton() = default;

			~Singleton()
			{
				delete m_instance;
			}

			static InstanceType& Get()
			{
				if (m_instance == nullptr)
					m_instance = new InstanceType();

				return *m_instance;
			}

			static void Delete()
			{
				if (m_instance)
				{
					delete m_instance;
					m_instance = nullptr;
				}
			}
		};
	}
}