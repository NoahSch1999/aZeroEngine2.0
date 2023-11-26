#pragma once
#include <span>

#include "../../Singleton/Singleton.h"
#include "../../../Macros/DebugMacros.h"

namespace aZero
{
	namespace D3D12
	{
		namespace Resource
		{
			class ResourceBase
			{
			private:
				D3D12_RESOURCE_STATES m_resourceState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;

			protected:
				ResourceRecycler* m_resourceRecycler = nullptr;

				Microsoft::WRL::ComPtr<ID3D12Resource> m_resource = nullptr;

				void Initialize(ResourceRecycler& resourceRecycler, ID3D12Device* const device,
					const D3D12_RESOURCE_DESC& description,
					const D3D12_HEAP_TYPE heapType,
					const D3D12_CLEAR_VALUE* const clearValue = nullptr,
					const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON)
				{
					if (!m_resource)
					{
						D3D12_HEAP_PROPERTIES heapProps = {};
						heapProps.Type = heapType;

						const HRESULT hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE, &description, initialState, clearValue, IID_PPV_ARGS(&m_resource));
						if (FAILED(hr))
							throw;

						m_resourceRecycler = &resourceRecycler;
					}
					else
					{
						// TODO - Fix error with this
						//FATAL_LOG("Tried to initialize an already initialized resource.");
						throw;
					}
				}

				ResourceBase() = default;
			public:

				~ResourceBase()
				{
					if (m_resource)
					{
						m_resourceRecycler->RecycleResource(m_resource);
					}
				}

				ID3D12Resource* const GetResource() const { return m_resource.Get(); }

				D3D12_RESOURCE_STATES GetResourceState() const { return m_resourceState; }

				// TODO - Find better/more automatic way to set the state
				void ForceResourceState(const D3D12_RESOURCE_STATES newState) { m_resourceState = newState; }
			};
		}
	}
}