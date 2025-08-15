#ifndef COMPOSIA_COMPONENT_POOL_H
#define COMPOISA_COMPONENT_POOL_H

#include <limits> // std::numeric_limits

#include "Entity.h"
#include "Core/SparseSet.h"
using Composia::Core::SparseSet;



namespace Composia {

template<typename T>
class ComponentPool
{
	ComponentPool() = default;

	inline bool Has(Entity e) const noexcept
	{
		return m_Set.Has(e);
	}

	inline void Add(Entity e, const T& value) noexcept
	{
		m_Set.Add(e);
	}

	inline void Remove(Entity e)
	{
		m_Set.Remove(e);
	}

	[[nodiscard]] inline T* Get(Entity e) noexcept
	{
		if (!Has(e)) return nullptr;
		return m_Set.Get(e);
	}

	[[nodiscard]] inline const DynamicArray<T>& RawDense() const noexcept
	{
		return m_Set.RawDense();
	}

	[[nodiscard]] inline const DynamicArray<Entity>& RawEntities() const noexcept
	{
		return m_Set.RawPacked();
	}

	[[nodiscard]] inline size_t Size() const noexcept
	{
		return m_Set.Size();
	}

private:
	SparseSet<T> m_Set;
};

// Type-erased component pool interface
struct IComponentPool
{
	virtual ~IComponentPool() = default;
	virtual void Remove(Entity e) noexcept = 0;
	virtual bool Has(Entity e) noexcept = 0;
	virtual size_t Size() noexcept = 0;
};

template<typename T>
struct ComponentPoolWrapper : IComponentPool
{
	ComponentPool<T> pool;
	void Remove(Entity e) noexcept override
	{
		pool.Remove(e);
	}

	bool Has(Entity e) noexcept override
	{
		return pool.Has(e);
	}

	size_t Size() noexcept override
	{
		return pool.Size();
	}

};

} // namespace Composia 

#endif // !COMPOSIA_COMPONENT_POOL_H
