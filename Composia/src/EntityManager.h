#ifndef COMPOSIA_ENTITY_MANAGER_H
#define COMPOSIA_ENTITY_MANAGER_H

#include "Entity.h"
#include "Core/DynamicArray.h"
using Composia::Core::DynamicArray;

namespace Composia {

class EntityManager
{
public:
	inline Entity Create() noexcept
	{
		if (!m_FreeList.Empty())
		{
			Entity removedEntity = m_FreeList.Back();
			m_FreeList.PopBack();

			m_Generations.At(removedEntity)++;
			m_Alive.At(removedEntity) = true;
			return removedEntity;
		}

		Entity id = static_cast<Entity>(m_Generations.Size());
		m_Generations.PushBack(0);
		m_Alive.PushBack(true);
		return id;
	}

	inline bool IsAlive(Entity e) const noexcept
	{
		return e < m_Generations.Size() && m_Alive.At(e);
	}

	inline void Destroy(Entity e) noexcept 
	{
		if (!IsAlive(e)) return;

		m_Alive.At(e) = false;
		m_FreeList.PushBack(e);
	}

	inline uint32_t Generation(Entity e) const noexcept
	{
		return e < m_Generations.Size() ? m_Generations.At(e) : 0;
	}

private:
	DynamicArray<uint32_t> m_Generations;
	DynamicArray<bool> m_Alive;
	DynamicArray<Entity> m_FreeList;
};

} // namespace Composia 

#endif //!COMPOSIA_ENTITY_MANAGER_H
