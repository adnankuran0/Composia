#ifndef COMPOSIA_ENTITY_MANAGER_H
#define COMPOSIA_ENTITY_MANAGER_H

#include <vector>
#include "Entity.h"
#include "Core/DynamicArray.h"
using Composia::Core::DynamicArray;

namespace Composia {

class EntityManager
{
public:
	EntityManager(size_t initialCapacity = 4096)
	{
		m_Generations.Reserve(initialCapacity);
		m_Alive.reserve(initialCapacity);
		m_FreeList.Reserve(initialCapacity);
	}

	inline Entity Create() noexcept
	{
		if (!m_FreeList.Empty())
		{
			Entity removedEntity = m_FreeList.Back();
			m_FreeList.PopBack();

			m_Generations[removedEntity]++;
			m_Alive[removedEntity] = true;
			return removedEntity;
		}

		Entity id = static_cast<Entity>(m_Generations.Size());
		m_Generations.PushBack(0);
		m_Alive.push_back(true);
		return id;
	}

	inline bool IsAlive(Entity e) const noexcept
	{
		return e < m_Generations.Size() && m_Alive[e];
	}

	inline void Destroy(Entity e) noexcept 
	{
		if (!IsAlive(e)) return;

		m_Alive[e]= false;
		m_FreeList.PushBack(e);
	}

	inline uint32_t Generation(Entity e) const noexcept
	{
		return e < m_Generations.Size() ? m_Generations.At(e) : 0;
	}

private:
	DynamicArray<uint32_t> m_Generations;
	std::vector<bool> m_Alive;
	DynamicArray<Entity> m_FreeList;
};

} // namespace Composia 

#endif //!COMPOSIA_ENTITY_MANAGER_H
