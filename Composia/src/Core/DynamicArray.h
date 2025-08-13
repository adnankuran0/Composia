#ifndef COMPOSIA_DYNAMIC_ARRAY_H
#define COMPOSIA_DYNAMIC_ARRAY_H

#include <algorithm> // std::move
#include <cassert>

namespace Composia::Core {


template<typename T>
class DynamicArray
{
public:
	DynamicArray(size_t initialCapacity = 1) 
		: m_Capacity(initialCapacity), m_Size(0), m_Data(nullptr) 
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

		new (&m_Data[m_Size]) T(value);
		m_Size++;
	}

	inline void PushBack(const T&& value) noexcept
	{
		if (m_Size >= m_Capacity) Grow();

		new (&m_Data[m_Size]) T(std::move(value));
		m_Size++;
	}

	inline void PopBack() noexcept
	{
		if (m_Size > 0)
		{
			m_Data[m_Size - 1].~T();
			m_Size--;
		}
	}

	inline const T& At(size_t index) noexcept
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

	inline void Reserve(size_t newCapacity)
	{
		T* newPtr = static_cast<T*>(operator new(newCapacity * sizeof(T)));
		for (size_t i = 0; i < m_Size; i++) 
		{
			new (&newPtr[i]) T(std::move(m_Data[i]));
			m_Data[i].~T();
		}
		operator delete(m_Data);
		m_Data = newPtr;
		m_Capacity = newCapacity;
	}

	inline void Resize(size_t newSize) noexcept
	{
		if (newSize > m_Capacity) Grow();

		if (newSize > m_Size)
		{
			for (size_t i = m_Size; i < newSize; i++)
			{
				new (&m_Data[i]) T();
			}
		}
		else
		{
			for (size_t i = newSize; i < m_Size; i++)
			{
				m_Data[i].~T();
			}
		}

		m_Size = newSize;
	}

	inline void Resize(size_t newSize, const T& value) 
	{
		if (newSize > m_Capacity) Grow();
		if (newSize > m_Size) {
			for (size_t i = m_Size; i < newSize; ++i)
				new (&m_Data[i]) T(value); 
		}
		else {
			for (size_t i = newSize; i < m_Size; ++i)
				m_Data[i].~T();
		}
		m_Size = newSize;
	}

private:
	inline void Grow() noexcept
	{
		size_t newCapacity = m_Capacity != 0 ? m_Capacity * 2 : 1;
		Reserve(newCapacity);
	}

	size_t m_Capacity;
	size_t m_Size;
	T* m_Data;
};

} // namespace Composia::Core 

#endif // !COMPOSIA_DYNAMIC_ARRAY_H
