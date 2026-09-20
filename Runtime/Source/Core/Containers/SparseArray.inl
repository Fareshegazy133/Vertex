// Vertex

#pragma once

#include <cassert>

namespace VCore
{
template <typename ElementType>
VSparseArray<ElementType>::Iterator::Iterator(VSparseArray* InOwner, const int32 InIndex)
	: Owner(InOwner), Index(InIndex)
{
	AdvanceToValid();
}

template <typename ElementType>
int32 VSparseArray<ElementType>::Iterator::GetIndex() const
{
	return Index;
}

template <typename ElementType>
ElementType& VSparseArray<ElementType>::Iterator::operator*() const
{
	return Owner->Slots[Index].Element;
}

template <typename ElementType>
ElementType* VSparseArray<ElementType>::Iterator::operator->() const
{
	return &Owner->Slots[Index].Element;
}

template <typename ElementType>
typename VSparseArray<ElementType>::Iterator& VSparseArray<ElementType>::Iterator::operator++()
{
	Index++;
	AdvanceToValid();
	return *this;
}

template <typename ElementType>
typename VSparseArray<ElementType>::Iterator& VSparseArray<ElementType>::Iterator::operator--()
{
	Index--;
	RetreatToValid();
	return *this;
}

template <typename ElementType>
bool VSparseArray<ElementType>::Iterator::operator==(const Iterator& OtherIterator) const
{
	return Owner == OtherIterator.Owner && Index == OtherIterator.Index;
}

template <typename ElementType>
bool VSparseArray<ElementType>::Iterator::operator!=(const Iterator& OtherIterator) const
{
	return !(*this == OtherIterator);
}

template <typename ElementType>
void VSparseArray<ElementType>::Iterator::AdvanceToValid()
{
	while (Owner && Index < Owner->Capacity && !Owner->Slots[Index].bIsAllocated)
	{
		Index++;
	}
}

template <typename ElementType>
void VSparseArray<ElementType>::Iterator::RetreatToValid()
{
	while (Index >= 0 && !Owner->IsValidIndex(Index))
	{
		Index--;
	}
}

template <typename ElementType>
VSparseArray<ElementType>::ConstIterator::ConstIterator(const VSparseArray* InOwner, const int32 InIndex)
	: Owner(InOwner), Index(InIndex)
{
	AdvanceToValid();
}

template <typename ElementType>
int32 VSparseArray<ElementType>::ConstIterator::GetIndex() const
{
	return Index;
}

template <typename ElementType>
const ElementType& VSparseArray<ElementType>::ConstIterator::operator*() const
{
	return Owner->Slots[Index].Element;
}

template <typename ElementType>
const ElementType* VSparseArray<ElementType>::ConstIterator::operator->() const
{
	return &Owner->Slots[Index].Element;
}

template <typename ElementType>
typename VSparseArray<ElementType>::ConstIterator& VSparseArray<ElementType>::ConstIterator::operator++()
{
	Index++;
	AdvanceToValid();
	return *this;
}

template <typename ElementType>
typename VSparseArray<ElementType>::ConstIterator& VSparseArray<ElementType>::ConstIterator::operator--()
{
	Index--;
	RetreatToValid();
	return *this;
}

template <typename ElementType>
bool VSparseArray<ElementType>::ConstIterator::operator==(const ConstIterator& OtherConstIterator) const
{
	return Owner == OtherConstIterator.Owner && Index == OtherConstIterator.Index;
}

template <typename ElementType>
bool VSparseArray<ElementType>::ConstIterator::operator!=(const ConstIterator& OtherConstIterator) const
{
	return !(*this == OtherConstIterator);
}

template <typename ElementType>
void VSparseArray<ElementType>::ConstIterator::AdvanceToValid()
{
	while (Owner && Index < Owner->Capacity && !Owner->Slots[Index].bAllocated)
	{
		Index++;
	}
}

template <typename ElementType>
void VSparseArray<ElementType>::ConstIterator::RetreatToValid()
{
	while (Index >= 0 && !Owner->IsValidIndex(Index))
	{
		Index--;
	}
}

template <typename ElementType>
VSparseArray<ElementType>::VSparseArray(VSparseArray&& OtherSparseArray) noexcept
	: Slots(OtherSparseArray.Slots), Indices(OtherSparseArray.Indices), Capacity(OtherSparseArray.Capacity), FirstFreeIndex(OtherSparseArray.FirstFreeIndex)
{
	OtherSparseArray.Slots = nullptr;
	OtherSparseArray.Indices = 0;
	OtherSparseArray.Capacity = 0;
	OtherSparseArray.FirstFreeIndex = INDEX_NONE;
}

template <typename ElementType>
VSparseArray<ElementType>::VSparseArray(const VSparseArray& OtherSparseArray)
{
	Reserve(OtherSparseArray.Capacity);

	for (int32 Index = 0; Index < OtherSparseArray.Capacity; Index++)
	{
		if (OtherSparseArray.Slots[Index].bIsAllocated)
		{
			Emplace(OtherSparseArray.Slots[Index].Element);
		}
	}
}

template <typename ElementType>
VSparseArray<ElementType>::VSparseArray(const int32 InitialCapacity)
{
	Reserve(InitialCapacity);
}

template <typename ElementType>
VSparseArray<ElementType>::~VSparseArray()
{
	Empty();
}

template <typename ElementType>
template <typename TFunction>
void VSparseArray<ElementType>::ForEach(TFunction&& Function)
{
	for (int32 Index = 0; Index < Capacity; Index++)
	{
		if (Slots[Index].bIsAllocated)
		{
			Function(Slots[Index].Element);
		}
	}
}

template <typename ElementType>
template <typename TFunction>
void VSparseArray<ElementType>::ForEach(TFunction&& Function) const
{
	for (int32 Index = 0; Index < Capacity; Index++)
	{
		if (Slots[Index].bIsAllocated)
		{
			Function(Slots[Index].Element);
		}
	}
}

template <typename ElementType>
template <typename ... TArgs>
int32 VSparseArray<ElementType>::Emplace(TArgs&&... Arguments)
{
	int32 Index;

	if (FirstFreeIndex != INDEX_NONE)
	{
		Index = FirstFreeIndex;
		VSlot& Slot = Slots[Index];
		FirstFreeIndex = Slot.NextFreeIndex;
		
		VAllocator<ElementType>::Construct(&Slot.Element, std::forward<TArgs>(Arguments)...);
		Slot.bIsAllocated = true;
	}
	else
	{
		if (Indices == Capacity) Grow();

		Index = Indices;
		VSlot& Slot = Slots[Index];

		VAllocator<ElementType>::Construct(&Slot.Element, std::forward<TArgs>(Arguments)...);
		Slot.bIsAllocated = true;
	}

	Indices++;
	return Index;
}

template <typename ElementType>
template <typename TPredicate>
int32 VSparseArray<ElementType>::IndexOfByPredicate(TPredicate&& Predicate) const
{
	for (int32 Index = 0; Index < Capacity; Index++)
	{
		if (!Slots[Index].bIsAllocated) continue;
		if (Predicate(Slots[Index].Element)) return Index;
	}

	return INDEX_NONE;
}

template <typename ElementType>
void VSparseArray<ElementType>::Reserve(const int32 NewCapacity)
{
	if (NewCapacity <= Capacity) return;
	VSlot* NewSlots = static_cast<VSlot*>(VMemory::Allocate(sizeof(VSlot) * static_cast<TSize>(NewCapacity), alignof(VSlot)));

	for (int32 Index = 0; Index < NewCapacity; Index++)
	{
		::new (static_cast<void*>(NewSlots + Index)) VSlot();
	}

	for (int32 Index = 0; Index < Capacity; Index++)
	{
		NewSlots[Index].bIsAllocated = Slots[Index].bIsAllocated;

		if (Slots[Index].bIsAllocated)
		{
			VAllocator<ElementType>::Construct(&NewSlots[Index].Element, std::move_if_noexcept(Slots[Index].Element));
			VAllocator<ElementType>::Destroy(&Slots[Index].Element);
		}
		else
		{
			NewSlots[Index].NextFreeIndex = Slots[Index].NextFreeIndex;
		}
	}

	for (int32 Index = 0; Index < Capacity; Index++)
	{
		Slots[Index].~VSlot();
	}

	VMemory::Deallocate(Slots, sizeof(VSlot) * static_cast<TSize>(Capacity), alignof(VSlot));
	Slots = NewSlots;
	Capacity = NewCapacity;
}

template <typename ElementType>
void VSparseArray<ElementType>::Reset()
{
	DestroyAll();

	for (int32 Index = 0; Index < Capacity; Index++)
	{
		::new (static_cast<void*>(Slots + Index)) VSlot();
	}

	Indices = 0;
	FirstFreeIndex = Indices;
}

template <typename ElementType>
void VSparseArray<ElementType>::Empty()
{
	if (!Slots) return;

	DestroyAll();
	VMemory::Deallocate(Slots, sizeof(VSlot) * static_cast<size_t>(Capacity), alignof(VSlot));

	Slots = nullptr;
	Indices = 0;
	Capacity = 0;
	FirstFreeIndex = INDEX_NONE;
}

template <typename ElementType>
int32 VSparseArray<ElementType>::Add(ElementType&& Element)
{
	return Emplace(std::move(Element));
}

template <typename ElementType>
int32 VSparseArray<ElementType>::Add(const ElementType& Element)
{
	return Emplace(Element);
}

template <typename ElementType>
void VSparseArray<ElementType>::RemoveAt(const int32 ArrayIndex)
{
	assert(IsValidIndex(ArrayIndex));

	VSlot& Slot = Slots[ArrayIndex];
	VAllocator<ElementType>::Destroy(&Slot.Element);

	Slot.bIsAllocated = false;
	Slot.NextFreeIndex = FirstFreeIndex;
	FirstFreeIndex = ArrayIndex;
	Indices--;
}

template <typename ElementType>
VSparseArray<ElementType>::Iterator VSparseArray<ElementType>::Begin()
{
	return Iterator(this, 0);
}

template <typename ElementType>
VSparseArray<ElementType>::Iterator VSparseArray<ElementType>::begin()
{
	return Iterator(this, 0);
}

template <typename ElementType>
VSparseArray<ElementType>::ConstIterator VSparseArray<ElementType>::Begin() const
{
	return Iterator(this, 0);
}

template <typename ElementType>
VSparseArray<ElementType>::ConstIterator VSparseArray<ElementType>::begin() const
{
	return Iterator(this, 0);
}

template <typename ElementType>
VSparseArray<ElementType>::Iterator VSparseArray<ElementType>::End()
{
	return Iterator(this, Capacity);
}

template <typename ElementType>
VSparseArray<ElementType>::Iterator VSparseArray<ElementType>::end()
{
	return Iterator(this, Capacity);
}

template <typename ElementType>
VSparseArray<ElementType>::ConstIterator VSparseArray<ElementType>::End() const
{
	return Iterator(this, Capacity);
}

template <typename ElementType>
VSparseArray<ElementType>::ConstIterator VSparseArray<ElementType>::end() const
{
	return Iterator(this, Capacity);
}

template <typename ElementType>
int32 VSparseArray<ElementType>::Num() const
{
	return Indices;
}

template <typename ElementType>
int32 VSparseArray<ElementType>::Max() const
{
	return Capacity;
}

template <typename ElementType>
bool VSparseArray<ElementType>::IsEmpty() const
{
	return Indices == 0;
}

template <typename ElementType>
bool VSparseArray<ElementType>::IsValidIndex(const int32 ArrayIndex) const
{
	return ArrayIndex >= 0 && ArrayIndex < Capacity && Slots[ArrayIndex].bIsAllocated;
}

template <typename ElementType>
VSparseArray<ElementType>& VSparseArray<ElementType>::operator=(const VSparseArray& OtherSparseArray)
{
	if (this == &OtherSparseArray) return *this;

	Empty();
	Reserve(OtherSparseArray.Capacity);

	for (int32 Index = 0; Index < OtherSparseArray.Capacity; Index++)
	{
		if (OtherSparseArray.Slots[Index].bIsAllocated)
		{
			Emplace(OtherSparseArray.Slots[Index].Element);
		}
	}

	return *this;
}

template <typename ElementType>
VSparseArray<ElementType>& VSparseArray<ElementType>::operator=(VSparseArray&& OtherSparseArray) noexcept
{
	if (this == &OtherSparseArray) return *this;

	Empty();
	Slots = OtherSparseArray.Slots;
	Indices = OtherSparseArray.Indices;
	Capacity = OtherSparseArray.Capacity;
	FirstFreeIndex = OtherSparseArray.FirstFreeIndex;

	OtherSparseArray.Slots = nullptr;
	OtherSparseArray.Indices = 0;
	OtherSparseArray.Capacity = 0;
	OtherSparseArray.FirstFreeIndex = INDEX_NONE;
	return *this;
}

template <typename ElementType>
ElementType& VSparseArray<ElementType>::operator[](const int32 ArrayIndex)
{
	assert(IsValidIndex(ArrayIndex));
	return Slots[ArrayIndex].Element;
}

template <typename ElementType>
const ElementType& VSparseArray<ElementType>::operator[](const int32 ArrayIndex) const
{
	assert(IsValidIndex(ArrayIndex));
	return Slots[ArrayIndex].Element;
}

template <typename ElementType>
VSparseArray<ElementType>::VSlot::VSlot()
	: NextFreeIndex(INDEX_NONE)
{}

template <typename ElementType>
VSparseArray<ElementType>::VSlot::~VSlot()
{
	if (bIsAllocated)
	{
		Element.~ElementType();
	}
}

template <typename ElementType>
void VSparseArray<ElementType>::Grow()
{
	const int32 NewCapacity = Capacity == 0 ? 8 : Capacity * 2;
	Reserve(NewCapacity);
}

template <typename ElementType>
void VSparseArray<ElementType>::DestroyAll()
{
	for (int32 Index = 0; Index < Capacity; Index++)
	{
		if (Slots[Index].bIsAllocated)
		{
			VAllocator<ElementType>::Destroy(&Slots[Index].Element);
		}

		Slots[Index].~VSlot();
	}
}
}