#ifndef VIEW_H
#define VIEW_H

#include <tuple>
#include <utility>
#include <limits>
#include "Core/DynamicArray.h"
#include "ComponentManager.h"

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

#endif // VIEW_H