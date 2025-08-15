#ifndef COMPOSIA_DYNAMIC_ARRAY_H
#define COMPOSIA_DYNAMIC_ARRAY_H

#include <new>       // operator new / delete
#include <utility>   // std::move, std::forward
#include <cassert> // assert
#include <type_traits> // std::is_trivially_destructible_v

namespace Composia::Core {

template<typename T>
class DynamicArray
{
public:
	DynamicArray(size_t initialCapacity = 1) 
		: m_Capacity(initialCapacity), m_Size(0), m_GrowMultiplier(3), m_Data(nullptr)
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

	inline void PushBack(T&& value) noexcept
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

	inline T& At(size_t index)
	{
		assert(index < m_Size && "Index out of bounds");
		return m_Data[index];
	}

	inline const T& At(size_t index) const
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
		if (std::is_trivially_copyable_v<T>)
		{
			memcpy(newPtr, m_Data, m_Size * sizeof(T));
		}
		else
		{
			for (size_t i = 0; i < m_Size; i++)
			{
				new (&newPtr[i]) T(std::move(m_Data[i]));
				if constexpr (!std::is_trivially_destructible_v<T>)
				{
					m_Data[i].~T();
				}
			}
		}
		
		operator delete(m_Data);
		m_Data = newPtr;
		m_Capacity = newCapacity;
	}

	inline void Resize(size_t newSize) noexcept
	{
		if (newSize > m_Capacity) Reserve(newSize);

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
				if constexpr (!std::is_trivially_destructible_v<T>)
				{
					m_Data[i].~T();
				}
			}
		}

		m_Size = newSize;
	}

	inline void Resize(size_t newSize, const T& value) 
	{
		if (newSize > m_Capacity) Reserve(newSize);

		if (newSize > m_Size) 
		{
			for (size_t i = m_Size; i < newSize; ++i)
				new (&m_Data[i]) T(value); 
		}
		else 
		{
			for (size_t i = newSize; i < m_Size; ++i)
			{
				if constexpr (!std::is_trivially_destructible_v<T>)
				{
					m_Data[i].~T();
				}
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

	inline void GrowMultiplier(uint8_t newMultiplyer) noexcept
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

#endif // !COMPOSIA_DYNAMIC_ARRAY_H
