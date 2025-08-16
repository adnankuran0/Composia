#ifndef COMPOSIA_H
#define COMPOSIA_H

#include <new>       // operator new / delete
#include <utility>   // std::move, std::forward
#include <cassert> // assert
#include <type_traits> // std::is_trivially_destructible_v

namespace Composia::Core {

	template<typename T>
	class DynamicArray
	{
	public:
		DynamicArray(size_t initialCapacity = 4)
			: m_Capacity(initialCapacity), m_Size(0), m_GrowMultiplier(2), m_Data(nullptr)
		{
			m_Data = static_cast<T*>(operator new(m_Capacity * sizeof(T)));
		}

		~DynamicArray()
		{
			for (size_t i = 0; i < m_Size; ++i)
				m_Data[i].~T();
			operator delete(m_Data);
		}

		inline void PushBack(const T& value) noexcept
		{
			if (m_Size >= m_Capacity) Grow();

			if constexpr (std::is_trivially_copyable_v<T>)
			{
				m_Data[m_Size] = value;
			}
			else
			{
				new (&m_Data[m_Size]) T(value);
			}
			m_Size++;
		}

		inline void PushBack(T&& value) noexcept
		{
			if (m_Size >= m_Capacity) Grow();


			if constexpr (std::is_trivially_copyable_v<T>)
			{
				m_Data[m_Size] = std::move(value);
			}
			else
			{
				new (&m_Data[m_Size]) T(std::move(value));
			}
			m_Size++;
		}

		template<typename... Args>
		inline T& EmplaceBack(Args&&... args)
		{
			if (m_Size >= m_Capacity) Grow();

			new (&m_Data[m_Size]) T(std::forward<Args>(args)...);
			return m_Data[m_Size++];
		}

		inline void PopBack() noexcept
		{
			if (m_Size > 0)
			{
				m_Data[m_Size - 1].~T();
				m_Size--;
			}
		}

		inline T& At(size_t index)
		{
			assert(index < m_Size && "Index out of bounds");
			return m_Data[index];
		}

		inline const T& At(size_t index) const noexcept
		{
			assert(index < m_Size && "Index out of bounds");
			return m_Data[index];
		}

		inline size_t Size() const noexcept
		{
			return m_Size;
		}

		inline size_t Capacity() const noexcept
		{
			return m_Capacity;
		}

		inline T* Data() const noexcept
		{
			return m_Data;
		}

		void Reserve(size_t newCapacity)
		{
			if (newCapacity <= m_Capacity) return;
			T* newPtr = static_cast<T*>(operator new(newCapacity * sizeof(T)));

			if constexpr (std::is_trivially_copyable_v<T>)
			{
				memcpy(newPtr, m_Data, m_Size * sizeof(T));
			}
			else
			{
				for (size_t i = 0; i < m_Size; ++i)
				{
					new (&newPtr[i]) T(std::move(m_Data[i]));
					if constexpr (!std::is_trivially_destructible_v<T>)
						m_Data[i].~T();
				}
			}

			operator delete(m_Data);
			m_Data = newPtr;
			m_Capacity = newCapacity;
		}

		void Resize(size_t newSize)
		{
			if (newSize > m_Capacity) Reserve(newSize);

			if (newSize > m_Size)
			{
				if constexpr (std::is_trivially_default_constructible_v<T>)
				{
					for (size_t i = m_Size; i < newSize; ++i)
						m_Data[i] = T{};
				}
				else
				{
					for (size_t i = m_Size; i < newSize; ++i)
						new (&m_Data[i]) T();
				}
			}
			else
			{
				for (size_t i = newSize; i < m_Size; ++i)
					if constexpr (!std::is_trivially_destructible_v<T>)
						m_Data[i].~T();
			}

			m_Size = newSize;
		}

		void Resize(size_t newSize, const T& value)
		{
			if (newSize > m_Capacity) Reserve(newSize);

			if (newSize > m_Size)
			{
				if constexpr (std::is_trivially_copyable_v<T>)
				{
					for (size_t i = m_Size; i < newSize; ++i)
						m_Data[i] = value;
				}
				else
				{
					for (size_t i = m_Size; i < newSize; ++i)
						new (&m_Data[i]) T(value);
				}
			}
			else
			{
				if constexpr (!std::is_trivially_destructible_v<T>)
				{
					for (size_t i = newSize; i < m_Size; ++i)
						m_Data[i].~T();
				}
			}

			m_Size = newSize;
		}

		inline void Clear()
		{
			for (size_t i = 0; i < m_Size; ++i)
			{
				if constexpr (!std::is_trivially_destructible_v<T>)
				{
					m_Data[i].~T();
				}
			}

			m_Size = 0;
		}

		inline T& Front() noexcept
		{
			assert(!Empty());
			return m_Data[0];
		}

		inline T& Back() noexcept
		{
			assert(!Empty());
			return m_Data[m_Size - 1];
		}

		inline bool Empty() const noexcept
		{
			return m_Size == 0;
		}

		inline T& operator[](size_t index) noexcept
		{
			return m_Data[index];
		}

		inline const T& operator[](size_t index) const noexcept
		{
			return m_Data[index];
		}

		inline int GrowMultiplier() const noexcept
		{
			return m_GrowMultiplier;
		}

		inline void GrowMultiplier(uint8_t newMultipier) noexcept
		{
			if (newMultiplyer <= 1) return;
			m_GrowMultiplier = newMultiplyer;
		}

		T* begin() noexcept { return m_Data; }
		T* end() noexcept { return m_Data + m_Size; }

		const T* begin() const noexcept { return m_Data; }
		const T* end() const noexcept { return m_Data + m_Size; }

	private:
		inline void Grow() noexcept
		{
			size_t newCapacity = m_Capacity != 0 ? m_Capacity * m_GrowMultiplier : 1;
			Reserve(newCapacity);
		}

		size_t m_Capacity;
		size_t m_Size;
		uint8_t m_GrowMultiplier;
		T* m_Data;
	};

} // namespace Composia::Core 

#include <limits> // std::numeric_limits

using Composia::Core::DynamicArray;
using Key = uint32_t;

namespace Composia::Core {

	template<typename T>
	class SparseSet
	{
	public:
		SparseSet(size_t reserveSize = 1024)
		{
			m_Dense.Reserve(reserveSize);
			m_Sparse.Resize(reserveSize, INVALID_INDEX);
			m_Packed.Reserve(reserveSize);
		}

		inline bool Has(Key k) const noexcept
		{
			return k < m_Sparse.Size() &&
				m_Sparse[k] != INVALID_INDEX &&
				m_Sparse[k] < m_Dense.Size();
		}

		inline void Add(Key k, const T& value) noexcept
		{
			EnsureSparseSize(k);
			if (Has(k))
			{
				m_Dense[m_Sparse[k]] = value;
				return;
			}

			m_Sparse[k] = static_cast<uint32_t>(m_Dense.Size());
			m_Dense.PushBack(value);
			m_Packed.PushBack(k);
		}

		template<typename... Args>
		inline void Emplace(Key k, Args&&... args)
		{
			EnsureSparseSize(k);
			if (Has(k))
			{
				m_Dense[m_Sparse[k]] = T(std::forward<Args>(args)...);
				return;
			}

			m_Sparse[k] = static_cast<uint32_t>(m_Dense.Size());
			m_Dense.EmplaceBack(std::forward<Args>(args)...);
			m_Packed.PushBack(k);
		}

		inline void Remove(Key k)
		{
			if (!Has(k)) return;

			uint32_t denseRemovedIndex = m_Sparse[k];
			uint32_t denseLastIndex = static_cast<uint32_t>(m_Dense.Size() - 1);

			// move last element into removed slot
			m_Dense[denseRemovedIndex] = std::move(m_Dense[denseLastIndex]);
			Key movedKey = m_Packed[denseLastIndex];
			m_Packed[denseRemovedIndex] = movedKey;
			m_Sparse[movedKey] = denseRemovedIndex;

			// pop back
			m_Dense.PopBack();
			m_Packed.PopBack();
			m_Sparse[k] = INVALID_INDEX;

		}

		[[nodiscard]] inline T* Get(Key k) noexcept
		{
			if (!Has(k)) return nullptr;
			return &m_Dense[m_Sparse[k]];
		}

		[[nodiscard]] const DynamicArray<T>& RawDense() const noexcept
		{
			return m_Dense;
		}

		[[nodiscard]] inline const DynamicArray<Key>& RawPacked() const noexcept
		{
			return m_Packed;
		}

		[[nodiscard]] inline size_t Size() const noexcept
		{
			return m_Dense.Size();
		}

	private:
		static constexpr Key INVALID_INDEX = std::numeric_limits<Key>::max();

		inline void EnsureSparseSize(Key k) noexcept
		{
			if (k >= m_Sparse.Size())
			{
				size_t newCapacity = m_Sparse.Size() == 0 ? 64 : m_Sparse.Size();
				while (k >= newCapacity)
					newCapacity *= 2;

				m_Sparse.Resize(newCapacity, INVALID_INDEX);
			}
		}

		DynamicArray<T> m_Dense;
		DynamicArray<uint32_t> m_Sparse;
		DynamicArray<Key> m_Packed;

	};

} // namespace Composia::Core 

#include <cstdint> // uint32_t

namespace Composia {

	using Entity = uint32_t;
	static constexpr Entity INVALID_ENTITY = std::numeric_limits<Entity>::max();

} // namespace Composia

#include <vector>
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

			m_Alive[e] = false;
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

using Composia::Core::SparseSet;

namespace Composia {

	template<typename T>
	class ComponentPool
	{
	public:
		ComponentPool() = default;

		inline bool Has(Entity e) const noexcept
		{
			return m_Set.Has(e);
		}

		inline void Add(Entity e, const T& value) noexcept
		{
			m_Set.Add(e, value);
		}

		template<typename... Args>
		void Emplace(Entity e, Args&&... args) noexcept
		{
			m_Set.Emplace(e, std::forward<Args>(args)...);
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
		virtual bool Has(Entity e) const noexcept = 0;
		virtual size_t Size() const noexcept = 0;
	};

	template<typename T>
	struct ComponentPoolWrapper : IComponentPool
	{
		ComponentPool<T> pool;
		void Remove(Entity e) noexcept override
		{
			pool.Remove(e);
		}

		bool Has(Entity e) const noexcept override
		{
			return pool.Has(e);
		}

		size_t Size() const noexcept override
		{
			return pool.Size();
		}

	};

} // namespace Composia 

#include <typeindex> // std::type_index
#include <memory> // std::unique_ptr

using Composia::Core::DynamicArray;

class IComponentPool; // forward declaration

namespace Composia::Core {

	// Map structure for storing storing ComponentPools by type_index using robin hood hashing.
	class PoolMap
	{
	private:
		struct Entry
		{
			std::type_index key;
			std::unique_ptr<IComponentPool> value;
			size_t probeDistance = 0;
			bool occupied = false;

			Entry() noexcept : key(typeid(void)), value(nullptr), probeDistance(0), occupied(false) {}
		};

		DynamicArray<Entry> m_Buckets;
		size_t m_Size = 0;
		float m_LoadFactor = 0.7f;

	private:
		void Rehash(size_t newCapacity) noexcept
		{
			DynamicArray<Entry> oldBuckets = std::move(m_Buckets);
			m_Buckets.Clear();
			m_Buckets.Resize(newCapacity);
			m_Size = 0;

			for (auto& e : oldBuckets)
			{
				if (e.occupied)
					Insert(e.key, std::move(e.value));
			}
		}
	public:
		PoolMap(size_t capacity = 16)
		{
			m_Buckets.Resize(capacity);
		}

		void Insert(std::type_index key, std::unique_ptr<IComponentPool> value) noexcept
		{
			if ((float)(m_Size + 1) / m_Buckets.Size() > m_LoadFactor)
			{
				Rehash(m_Buckets.Size() * 2);
			}

			size_t hash = key.hash_code();
			size_t index = hash % m_Buckets.Size();
			size_t dist = 0;

			while (true)
			{
				Entry& e = m_Buckets[index];
				if (!e.occupied)
				{
					e.key = key;
					e.value = std::move(value);
					e.probeDistance = dist;
					e.occupied = true;
					++m_Size;
					return;
				}

				if (e.key == key) // overwrite
				{
					e.value = std::move(value);
					return;
				}

				if (e.probeDistance < dist)
				{
					std::swap(key, e.key);
					std::swap(value, e.value);
					std::swap(dist, e.probeDistance);
				}

				index = (index + 1) % m_Buckets.Size();
				++dist;
			}
		}

		[[nodiscard]] IComponentPool* Get(std::type_index key) noexcept
		{
			size_t hash = key.hash_code();
			size_t index = hash % m_Buckets.Size();
			size_t dist = 0;

			while (true)
			{
				Entry& e = m_Buckets[index];
				if (!e.occupied || dist > m_Buckets.Size())
					return nullptr;

				if (e.key == key)
					return e.value.get();

				++dist;
				index = (index + 1) % m_Buckets.Size();
			}
		}

		[[nodiscard]] const IComponentPool* Get(std::type_index key) const noexcept
		{
			size_t hash = key.hash_code();
			size_t index = hash % m_Buckets.Size();
			size_t dist = 0;

			while (true)
			{
				const auto& e = m_Buckets.At(index);
				if (!e.occupied || dist > m_Buckets.Size())
					return nullptr;

				if (e.key == key)
					return e.value.get();

				++dist;
				index = (index + 1) % m_Buckets.Size();
			}
		}

		[[nodiscard]] const DynamicArray<Entry>& GetBuckets() const noexcept
		{
			return m_Buckets;
		}

	};

} // namespace Composia::Core 

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
		inline void Emplace(Entity e, Args&&... args) noexcept
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
			static auto& idx = typeid(T);

			auto existing = m_Pools.Get(idx);
			if (existing)
			{
				return static_cast<ComponentPoolWrapper<T>*>(existing);
			}

			auto wrapper = std::make_unique<ComponentPoolWrapper<T>>();
			auto ptr = wrapper.get();
			m_Pools.Insert(idx, std::move(wrapper));

			return ptr;
		}
	private:
		PoolMap m_Pools;
	};

} // namespace Composia 

#include <tuple>

namespace Composia {

	template<typename... Components>
	class View
	{
	public:
		using PoolsTuple = std::tuple<ComponentPool<Components>*...>;

		View(ComponentManager& manager)
		{
			pools = std::make_tuple(manager.Pool<Components>()...);
			smallestPoolIndex = FindSmallestPoolIndex();
		}

		struct Iterator
		{
			using EntityVec = const Core::DynamicArray<Entity>&;

			Iterator(PoolsTuple* pools, size_t idx, size_t smallestPoolIndex)
				: pools(pools), index(idx), smallestPoolIndex(smallestPoolIndex)
			{
				AdvanceToValid();
			}

			Iterator& operator++() noexcept
			{
				++index;
				AdvanceToValid();
				return *this;
			}

			Entity operator*() const noexcept
			{
				Entity result{};
				get_by_index(*pools, smallestPoolIndex, [&](auto* pool) {
					result = pool->RawEntities()[index];
					});
				return result;
			}

			bool operator!=(const Iterator& other) const noexcept
			{
				return index != other.index;
			}

		private:
			void AdvanceToValid() noexcept
			{
				bool valid = false;
				while (!valid) {
					bool out_of_range = false;
					get_by_index(*pools, smallestPoolIndex, [&](auto* pool) {
						if (index >= pool->Size()) {
							out_of_range = true;
							return;
						}
						});
					if (out_of_range) break;

					Entity e{};
					get_by_index(*pools, smallestPoolIndex, [&](auto* pool) {
						e = pool->RawEntities()[index];
						});
					if (HasAllComponents(e))
						valid = true;
					else
						++index;
				}
			}

			inline bool HasAllComponents(Entity e) const noexcept
			{
				bool result = true;
				ApplyAll([&](auto* pool) {
					if (!pool->Has(e)) result = false;
					});
				return result;
			}

			template<typename Func>
			inline void ApplyAll(Func&& func) const noexcept
			{
				std::apply([&](auto*... poolPtrs) {
					(func(poolPtrs), ...);
					}, *pools);
			}

			PoolsTuple* pools;
			size_t index;
			size_t smallestPoolIndex;
		};

		inline Iterator begin() noexcept
		{
			return Iterator(&pools, 0, smallestPoolIndex);
		}

		inline Iterator end() noexcept
		{
			size_t size = 0;
			get_by_index(pools, smallestPoolIndex, [&](auto* pool)
				{
					size = pool->Size();
				});
			return Iterator(&pools, size, smallestPoolIndex);
		}

		template<typename Func>
		void each(Func&& func) noexcept
		{
			size_t size = 0;
			get_by_index(pools, smallestPoolIndex, [&](auto* pool)
				{
					size = pool->Size();
				});
			auto& smallestPool = std::get<0>(pools);

			for (size_t i = 0; i < size; ++i)
			{
				Entity e{};
				get_by_index(pools, smallestPoolIndex, [&](auto* pool)
					{
						e = pool->RawEntities()[i];
					});
				if (!HasAllComponents(e))
					continue;

				invokeFunc(e, std::forward<Func>(func));
			}
		}

	private:
		inline bool HasAllComponents(Entity e) const noexcept
		{
			bool result = true;
			std::apply([&](auto*... poolPtrs) {
				((result = result && poolPtrs->Has(e)), ...);
				}, pools);
			return result;
		}

		template<typename Func, size_t... Is>
		inline void invokeFuncImpl(Entity e, Func&& func, std::index_sequence<Is...>) noexcept
		{
			func(*std::get<Is>(pools)->Get(e)...);
		}

		template<typename Func>
		inline void invokeFunc(Entity e, Func&& func) noexcept
		{
			invokeFuncImpl(e, std::forward<Func>(func), std::index_sequence_for<Components...>{});
		}

		size_t FindSmallestPoolIndex() const noexcept
		{
			size_t smallest = 0;
			size_t smallestSize = std::numeric_limits<size_t>::max();

			for (size_t i = 0; i < sizeof...(Components); ++i)
			{
				bool sizeFetched = false;
				size_t poolSize = 0;

				get_by_index(pools, i, [&](auto* pool)
					{
						poolSize = pool->Size();
						sizeFetched = true;
					});

				if (sizeFetched && poolSize < smallestSize)
				{
					smallestSize = poolSize;
					smallest = i;
				}
			}
			return smallest;
		}

		// Helper to get tuple element by runtime index
		template<size_t I = 0, typename FuncT, typename TupleT>
		inline static typename std::enable_if<I == std::tuple_size<TupleT>::value, void>::type
			get_by_index(TupleT&, size_t, FuncT)
		{
			// out of range, do nothing
		}

		template<size_t I = 0, typename FuncT, typename TupleT>
		inline static typename std::enable_if < I < std::tuple_size<TupleT>::value, void>::type
			get_by_index(TupleT& t, size_t index, FuncT f)
		{
			if (I == index)
				f(std::get<I>(t));
			else
				get_by_index<I + 1, FuncT, TupleT>(t, index, f);
		}

		PoolsTuple pools;
		size_t smallestPoolIndex = 0;
	};

} // namespace Composia

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

		template<typename... Components>
		inline Composia::View<Components...> View() noexcept
		{
			return Composia::View<Components...>(this->m_ComponentManager);
		}

	private:
		EntityManager m_EntityManager;
		ComponentManager m_ComponentManager;
	};

} // namespace Composia 

#endif // !COMPOSIA_H
