// Vertex

#include "Core/Containers/Allocator.h"

#include <cstring>

namespace VCore
{
void* VMemory::Allocate(const TSize Size, const TSize Alignment)
{
	if (Size == 0) return nullptr;

#ifdef _MSC_VER
	return _aligned_malloc(Size, Alignment);
#else
	return std::aligned_alloc(Alignment, Size);
#endif
}

void* VMemory::AllocateZeroed(const TSize Size, const TSize Alignment)
{
	void* Memory = Allocate(Size, Alignment);

	if (Memory)
	{
		std::memset(Memory, 0, Size);
	}

	return Memory;
}

void* VMemory::Reallocate(void* Memory, const TSize OldSize, const TSize NewSize, const TSize Alignment)
{
	if (!Memory) return Allocate(NewSize, Alignment);

	if (NewSize == 0)
	{
		Deallocate(Memory, OldSize, Alignment);
		return nullptr;
	}

#ifdef _MSC_VER
	return _aligned_realloc(Memory, NewSize, Alignment);
#else
	void* NewMemory = Allocate(NewSize, Alignment);

	if (NewMemory)
	{
		std::memcpy(NewMemory, Memory, std::min(OldSize, NewSize));
		Deallocate(Memory, OldSize, Alignment);
	}
	
	return NewMemory;
#endif
}

void VMemory::Deallocate(void* Memory, const TSize Size, const TSize Alignment)
{
	if (!Memory) return;

#ifdef _MSC_VER
	_aligned_free(Memory);
#else
	std::free(Memory);
#endif
}
}