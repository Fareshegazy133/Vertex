// Vertex

#pragma once

#include <cstddef>
#include <utility>

#include "Core/Types/Numeric.h"

namespace VCore
{
class VMemory
{
public:
	static void* Allocate(const TSize Size, const TSize Alignment = alignof(std::max_align_t));
	static void* AllocateZeroed(const TSize Size, const TSize Alignment = alignof(std::max_align_t));
	
	static void* Reallocate(void* Memory, const TSize OldSize, const TSize NewSize, const TSize Alignment = alignof(std::max_align_t));
	static void Deallocate(void* Memory, const TSize Size = 0, const TSize Alignment = alignof(std::max_align_t));
};


template<typename ElementType>
class VAllocator
{
public:
	using ValueType = ElementType;
	
public:
	template<typename... TArgs>
	static void Construct(ElementType* Address, TArgs&&... Arguments);
	
	static void Construct(ElementType* Address);
	static void Destroy(ElementType* Address);
	
	static ElementType* Allocate(const int32 Count);
	static void Deallocate(ElementType* Memory, const int32 Count);

	static void DestroyRange(ElementType* Address, const int32 Count);
};

template <typename ElementType>
template <typename ... TArgs>
void VAllocator<ElementType>::Construct(ElementType* Address, TArgs&&... Arguments)
{
	::new (static_cast<void*>(Address)) ElementType(std::forward<TArgs>(Arguments)...);
}

template <typename ElementType>
void VAllocator<ElementType>::Construct(ElementType* Address)
{
	::new (static_cast<void*>(Address)) ElementType();
}

template <typename ElementType>
void VAllocator<ElementType>::Destroy(ElementType* Address)
{
	if constexpr (!std::is_trivially_destructible_v<ElementType>)
	{
		Address->~ElementType();
	}
}

template <typename ElementType>
ElementType* VAllocator<ElementType>::Allocate(const int32 Count)
{
	if (Count <= 0) return nullptr;
	return static_cast<ElementType*>(VMemory::Allocate(sizeof(ElementType) * static_cast<size_t>(Count),alignof(ElementType)));
}

template <typename ElementType>
void VAllocator<ElementType>::Deallocate(ElementType* Memory, const int32 Count)
{
	if (!Memory) return;
	VMemory::Deallocate(Memory, sizeof(ElementType) * static_cast<size_t>(Count), alignof(ElementType));
}

template <typename ElementType>
void VAllocator<ElementType>::DestroyRange(ElementType* Address, const int32 Count)
{
	if constexpr (!std::is_trivially_destructible_v<ElementType>)
	{
		for (int32 Index = Count - 1; Index >= 0; Index--)
		{
			Address[Index].~ElementType();
		}
	}
}
}