#include "Singleton.h"

aZero::ECS::ECSWrapper* aZero::Singleton::Singleton<aZero::ECS::ECSWrapper>::m_instance = nullptr;

aZero::D3D12::ResourceRecycler* aZero::Singleton::Singleton<aZero::D3D12::ResourceRecycler>::m_instance = nullptr;

aZero::D3D12::DescriptorManager* aZero::Singleton::Singleton<aZero::D3D12::DescriptorManager>::m_instance = nullptr;