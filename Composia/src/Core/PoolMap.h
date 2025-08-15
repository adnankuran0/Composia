#ifndef COMPOSIA_POOL_MAP_H
#define COMPOSIA_POOL_MAP_H

#include <typeindex> // std::type_index
#include <memory> // std::unique_ptr
#include "DynamicArray.h"

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

#endif // !COMPOSIA_POOL_MAP_H
