#pragma once

#include <vector>
#include <atomic>

#include "CTMRenderer/CTMShape.hpp"
#include "Core/CoreMacros.hpp"

namespace CTMRenderer
{
#pragma region CTMAllocator
//	template <typename DefaultTy>
//	class CTMAllocator
//	{
//	public:
//		CTMAllocator() = default;
//		~CTMAllocator() = default;
//	public:
//		template <typename Ty = DefaultTy>
//		Ty* Allocate(size_t n) const noexcept;
//
//		template <typename Ty = DefaultTy>
//		void Deallocate(Ty*& pMemoryRef, size_t n) const noexcept;
//
//		template <typename Ty = DefaultTy>
//		void Construct(Ty* pMemory, size_t n) const noexcept;
//
//		template <typename Ty = DefaultTy>
//		void Destroy(Ty* pMemory, size_t n) const noexcept;
//	};
//
#pragma region Implementations
//	template <typename DefaultTy>
//	template <typename Ty>
//	Ty* CTMAllocator<DefaultTy>::Allocate(size_t n) const noexcept
//	{
//		size_t bytes = n * sizeof(Ty);
//		RUNTIME_ASSERT(bytes > 0, "Cannot allocate a negative number of bytes. (Did it wrap around?) : " << bytes << '\n');
//
//		void* pMemory = ::operator new(bytes);
//
//		return static_cast<Ty*>(pMemory);
//	}
//
//	template <typename DefaultTy>
//	template <typename Ty>
//	void CTMAllocator<DefaultTy>::Deallocate(Ty*& pMemoryRef, size_t n) const noexcept
//	{
//		RUNTIME_ASSERT(n > 0, "The number of elements to deallocate must be greater than 0 : " << n << '\n');
//
//		::operator delete[](pMemoryRef, n);
//		pMemoryRef = nullptr;
//	}
//
//	template <typename DefaultTy>
//	template <typename Ty>
//	void CTMAllocator<DefaultTy>::Construct(Ty* pMemory, size_t n) const noexcept
//	{
//		if constexpr (std::is_trivially_constructible_v<Ty> || !std::is_default_constructible_v<Ty>)
//			return;
//		
//		new (pMemory) Ty[n];
//	}
//
//	template <typename DefaultTy>
//	template <typename Ty>
//	void CTMAllocator<DefaultTy>::Destroy(Ty* pMemory, size_t n) const noexcept
//	{
//		RUNTIME_ASSERT(n > 0, "The number of elements to destroy must be greater than 0 : " << n << '\n');
//
//		if constexpr (std::is_trivially_destructible_v<Ty>)
//			return;
//
//		for (size_t i = 0; i < n; ++i)
//			pMemory[i].~Ty();
//	}
#pragma endregion
//
#pragma endregion

#pragma region CTMArena
//	template <typename Ty>
//	class CTMArena
//	{
//	public:
//		class Iterator 
//		{
//		public:
//			Iterator(Ty* pDest) noexcept;
//			~Iterator() = default;
//		public:
//			bool operator!=(const Iterator& other) noexcept;
//			Iterator& operator++() noexcept;
//			Ty* operator->() noexcept;
//			Ty& operator*() noexcept;
//		public:
//			inline [[nodiscard]] Ty* Get() noexcept { return mP_Dest; }
//		private:
//			Ty* mP_Dest;
//		};
//	public:
//		CTMArena(size_t size) noexcept;
//		~CTMArena() noexcept;
//	public:
//		[[nodiscard]] Iterator Begin() noexcept;
//		[[nodiscard]] Iterator End() noexcept;
//
//		void ConstructAt(Iterator it) noexcept;
//		
//		[[nodiscard]] Ty& At(size_t i) noexcept;
//	private:
//		CTMAllocator<Ty> m_Allocator;
//		size_t m_Size;
//		Ty* mP_Data = nullptr;
//	};
//
#pragma region Implementations
//
#pragma region Iterator
//	template <typename Ty>
//	inline CTMArena<Ty>::Iterator::Iterator(Ty* pDest) noexcept
//		: mP_Dest(pDest)
//	{
//	}
//
//	template <typename Ty>
//	inline bool CTMArena<Ty>::Iterator::operator!=(const Iterator& other) noexcept
//	{
//		return mP_Dest != other.mP_Dest;
//	}
//
//	template <typename Ty>
//	inline CTMArena<Ty>::Iterator& CTMArena<Ty>::Iterator::operator++() noexcept
//	{
//		mP_Dest++;
//		return *this;
//	}
//
//	template <typename Ty>
//	inline Ty* CTMArena<Ty>::Iterator::operator->() noexcept
//	{
//		return mP_Dest;
//	}
//
//	template <typename Ty>
//	inline Ty& CTMArena<Ty>::Iterator::operator*() noexcept
//	{
//		return *mP_Dest;
//	}
#pragma endregion
//
#pragma region Arena
//	template <typename Ty>
//	CTMArena<Ty>::CTMArena(size_t size) noexcept
//	: m_Size(size)
//	{
//		mP_Data = m_Allocator.Allocate(size);
//		m_Allocator.Construct(mP_Data, size);
//	}
//
//	template <typename Ty>
//	CTMArena<Ty>::~CTMArena() noexcept
//	{
//		m_Allocator.Destroy(mP_Data, m_Size);
//		m_Allocator.Deallocate(mP_Data, m_Size);
//	}
//
//	template <typename Ty>
//	[[nodiscard]] CTMArena<Ty>::Iterator CTMArena<Ty>::Begin() noexcept
//	{
//		return Iterator(mP_Data);
//	}
//
//	template <typename Ty>
//	[[nodiscard]] CTMArena<Ty>::Iterator CTMArena<Ty>::End() noexcept
//	{
//		return Iterator(mP_Data + m_Size);
//	}
//
//
//	template <typename Ty>
//	void CTMArena<Ty>::ConstructAt(Iterator it) noexcept
//	{
//		m_Allocator.Construct(it.Get(), 1);
//	}
//
//	template <typename Ty>
//	[[nodiscard]] Ty& CTMArena<Ty>::At(size_t i) noexcept
//	{
//		RUNTIME_ASSERT(i < m_Size, "Index is out of range of [0 - " << m_Size - 1 << "].\n");
//		return *(mP_Data + i);
//	}
#pragma endregion
//
#pragma endregion
//
#pragma endregion

#pragma region CTMArenaList
//	template <typename Ty>
//	class CTMArenaList
//	{
//	public:
//		CTMArenaList(size_t arenaSize, size_t initialArenas) noexcept;
//	public:
//		inline [[nodiscard]] std::vector<CTMArena<Ty>>& List() noexcept { return m_List; }
//	private:
//		const size_t m_ArenaSize;
//		std::vector<CTMArena<Ty>> m_List;
//	};
//
#pragma region Implementations
//	template <typename Ty>
//	CTMArenaList<Ty>::CTMArenaList(size_t arenaSize, size_t initialArenas) noexcept
//		: m_ArenaSize(arenaSize), m_List(initialArenas) // Default construct the number of arenas.
//	{
//	}
#pragma endregion
//
#pragma endregion

//	template <typename Ty>
//	class CTMSharedControl 
//	{
//	public:
//		template <typename... Args>
//		inline CTMSharedControl(Args&&... args) noexcept
//			: RefCount(0), Obj(std::forward<Args>(args)...)
//		{
//
//		}
//	public:
//		std::atomic<size_t> RefCount = 0;
//		Ty Obj;
//	};
//
	template <typename Ty>
	class CTMPoolUnit
	{
	public:
		CTMPoolUnit() = default;
		~CTMPoolUnit() = default;

		template <typename... Args>
		Ty& GetPooledUnit(Args&&... args) noexcept;
	private:
		void SortPool() noexcept;
	private:
		std::vector<Ty> m_ActivePool;
		std::vector<Ty> m_InactivePool;
	};

	template <typename Ty>
	template <typename... Args>
	Ty& CTMPoolUnit<Ty>::GetPooledUnit(Args&&... args) noexcept
	{
		SortPool();

		if (m_InactivePool.empty())
			m_InactivePool.emplace_back(std::make_shared<Shapes::CTMRect>(std::forward<Args>(args)...));
		else
			m_InactivePool.back()->Set(std::forward<Args>(args)...);

		m_ActivePool.emplace_back(std::move(m_InactivePool.back()));
		m_InactivePool.pop_back();
		return m_ActivePool.back();
	}

	template <typename Ty>
	void CTMPoolUnit<Ty>::SortPool() noexcept
	{
		for (size_t i = 0; i < m_ActivePool.size(); ++i)
		{
			// (Has more references than just itself)
			if (m_ActivePool[i].use_count() != 1)
				continue;

			std::shared_ptr<Shapes::CTMRect> at = std::move(m_ActivePool[i]);

			// Move the element straight into m_InactiveRectPool.
			if (m_ActivePool.size() == 1)
				m_InactivePool.emplace_back(std::move(at));

			// Swap current element with last element and then move the current element into m_InactiveRectPool.
			else
			{
				std::shared_ptr<Shapes::CTMRect> back = std::move(m_ActivePool.back());

				m_ActivePool[i] = std::move(back);

				m_InactivePool.emplace_back(std::move(at));
			}

			m_ActivePool.pop_back();
		}
	}


	class CTMShapePool
	{
	public:
		CTMShapePool() = default;
		~CTMShapePool() = default;
	public:
		template <typename... Args>
		std::shared_ptr<Shapes::CTMRect> GetPooledRect(Args&&... args) noexcept;
	private:
		CTMPoolUnit<std::shared_ptr<Shapes::CTMRect>> m_RectPool;
	};

	template <typename... Args>
	std::shared_ptr<Shapes::CTMRect> CTMShapePool::GetPooledRect(Args&&... args) noexcept
	{
		return m_RectPool.GetPooledUnit(std::forward<Args>(args)...);
	}
}