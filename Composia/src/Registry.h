#ifndef COMPOSIA_REGISTRY_H
#define COMPOSIA_REGISTRY_H

#include "EntityManager.h"
#include "ComponentManager.h"

namespace Composia {

class Registry
{
public:
	inline Entity Create() noexcept
	{
		return m_EntityManager.Create();
	}

	template<typename T>
	inline void Remove(Entity e) noexcept
	{
		m_ComponentManager.Remove<T>(e);
	}

	inline void Destroy(Entity e) noexcept
	{
		m_ComponentManager.RemoveAllForEntity(e);
		m_EntityManager.Destroy(e);
	}

	template<typename T>
	inline void Add(Entity e, const T& comp) noexcept
	{
		m_ComponentManager.Add<T>(e, comp);
	}

	template<typename T>
	[[nodiscard]] inline bool Has(Entity e)
	{
		return m_ComponentManager.Has<T>(e);
	}
 
	template<typename T, typename... Args>
	inline void Emplace(Entity e, Args&&... args) noexcept
	{
		m_ComponentManager.Emplace<T>(e, std::forward<Args>(args)...);
	}

	template<typename T>
	inline T& Get(Entity e) noexcept
	{
		return *m_ComponentManager.Get<T>(e);
	}


private:
	EntityManager m_EntityManager;
	ComponentManager m_ComponentManager;
};

} // namespace Composia 

#endif // !COMPOSIA_REGISTRY_H
