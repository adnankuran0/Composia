#ifndef COMPOSIA_COMPONENT_MANAGER_H
#define COMPOSIA_COMPONENT_MANAGER_H

#include "ComponentPool.h"
#include "Core/PoolMap.h"
using Composia::Core::PoolMap;

namespace Composia {

class ComponentManager
{
public:
	template<typename T>
	inline void Add(Entity e, const T& comp) noexcept
	{
		auto* wrapper = GetOrCreatePool<T>();
		wrapper->pool.Add(e, comp);
	}

	template<typename T, typename... Args>
	void Emplace(Entity e, Args&&... args) noexcept
	{
		auto* wrapper = GetOrCreatePool<T>();
		wrapper->pool.Emplace(e, std::forward<Args>(args)...);
	}

	template<typename T>
	void Remove(Entity e) noexcept
	{
		auto existing = m_Pools.Get(typeid(T));
		if (existing)
		{
			static_cast<ComponentPoolWrapper<T>*>(existing)->pool.Remove(e);
		}
	}

	template<typename T>
	T* Get(Entity e) noexcept
	{
		auto existing = m_Pools.Get(typeid(T));
		if (!existing)
		{
			return nullptr;
		}

		return static_cast<ComponentPoolWrapper<T>*>(existing)->pool.Get(e);
	}

	template<typename T>
	bool Has(Entity e) noexcept
	{
		auto existing = m_Pools.Get(typeid(T)); 
		if (!existing)
			return false;

		auto wrapper = static_cast<ComponentPoolWrapper<T>*>(existing);
		return wrapper->Has(e);
	}

	template<typename T>
	ComponentPool<T>* Pool()
	{
		auto existing = m_Pools.Get(typeid(T));
		if (!existing)
		{
			return nullptr;
		}

		return &static_cast<ComponentPoolWrapper<T>*>(existing)->pool;
	}

	void RemoveAllForEntity(Entity entity) noexcept
	{
		for (size_t i = 0; i < m_Pools.GetBuckets().Size(); ++i) 
		{
			auto& slot = m_Pools.GetBuckets().At(i);
			if (slot.occupied && slot.value)
			{
				slot.value->Remove(entity);
			}
		}
	}

private:
	template<typename T>
	ComponentPoolWrapper<T>* GetOrCreatePool()
	{
		auto& idx = typeid(T);

		auto existing = m_Pools.Get(idx);
		if (existing)
		{
			return static_cast<ComponentPoolWrapper<T>*>(existing);
		}

		auto wrapper = std::make_unique<ComponentPoolWrapper<T>>();
		auto ptr = wrapper.get();
		m_Pools.Insert(idx,std::move(wrapper));
		
		return ptr;
	}
private:
	PoolMap m_Pools;
};

} // namespace Composia 

#endif // !COMPOSIA_COMPONENT_MANAGER_H
