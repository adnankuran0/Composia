#ifndef COMPOSIA_SPARSE_SET_H

#include <limits> // std::numeric_limits

#include "DynamicArray.h"

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
			m_Sparse.At(k) != INVALID_INDEX &&
			m_Sparse.At(k) < m_Dense.Size();
	}

	inline void Add(Key k, const T& value) noexcept
	{
		EnsureSparseSize(k);
		if (Has(k))
		{
			m_Dense.At(m_Sparse.At(k)) = value;
			return;
		}

		m_Sparse.At(k) = static_cast<uint32_t>(m_Dense.Size());
		m_Dense.PushBack(value);
		m_Packed.PushBack(k);
	}

	template<typename... Args>
	inline void Emplace(Key k, Args&&... args)
	{
		EnsureSparseSize(k);
		if (Has(k))
		{
			m_Dense.At(m_Sparse.At(k)) = T(std::forward<Args>(args)...);
			return;
		}

		m_Sparse[k] = static_cast<uint32_t>(m_Dense.Size());
		m_Dense.EmplaceBack(std::forward<Args>(args)...);
		m_Packed.PushBack(k);
	}

	inline void Remove(Key k)
	{
		if (!Has(k)) return;

		uint32_t denseRemovedIndex = m_Sparse.At(k);
		uint32_t denseLastIndex = static_cast<uint32_t>(m_Dense.Size() - 1);

		// move last element into removed slot
		m_Dense.At(denseRemovedIndex) = std::move(m_Dense.At(denseLastIndex));
		Key movedKey = m_Packed.At(denseLastIndex);
		m_Packed.At(denseRemovedIndex) = movedKey;
		m_Sparse.At(movedKey) = denseRemovedIndex;

		// pop back
		m_Dense.PopBack();
		m_Packed.PopBack();
		m_Sparse.At(k) = INVALID_INDEX;

	}

	[[nodiscard]] inline T* Get(Key k) noexcept
	{
		if (!Has(k)) return nullptr;
		return &m_Dense.At(m_Sparse.At(k));
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
			while (k <= newCapacity)
			{
				newCapacity *= 2;
			}
			m_Sparse.Resize(newCapacity, INVALID_INDEX);
		}
	}

	DynamicArray<T> m_Dense;
	DynamicArray<uint32_t> m_Sparse;
	DynamicArray<Key> m_Packed;

};

} // namespace Composia::Core 

#endif //!COMPOSIA_SPARSE_SET_H