// Vertex

#pragma once

#include <algorithm>
#include <cassert>
#include <utility>
#include "Core/Containers/Allocator.h"

namespace VCore
{
template <typename ElementType>
VArray<ElementType>::VArray()
	: Data(nullptr)
{}

template <typename ElementType>
VArray<ElementType>::VArray(VArray&& OtherArray) noexcept
	: Data(OtherArray.Data), Indices(OtherArray.Indices), Capacity(OtherArray.Capacity)
{
	OtherArray.Data = nullptr;
	OtherArray.Indices = 0;
	OtherArray.Capacity = 0;
}

template <typename ElementType>
VArray<ElementType>::VArray(const VArray& OtherArray)
	: Data(nullptr)
{
	Reserve(OtherArray.Indices);

	for (int32 Index = 0; Index < OtherArray.Indices; Index++)
	{
		VAllocator<ElementType>::Construct(Data + Index, OtherArray.Data[Index]);
	}

	Indices = OtherArray.Indices;
}

template <typename ElementType>
VArray<ElementType>::VArray(const int32 InitialCapacity)
	: Data(nullptr)
{
	Reserve(InitialCapacity);
}

template <typename ElementType>
VArray<ElementType>::~VArray()
{
	Empty();
}

template <typename ElementType>
template <typename TPredicate>
void VArray<ElementType>::Sort(TPredicate&& Predicate)
{
	std::sort(Begin(), End(), std::forward<TPredicate>(Predicate));
}

template <typename ElementType>
template <typename TPredicate>
void VArray<ElementType>::StableSort(TPredicate&& Predicate)
{
	std::stable_sort(Begin(), End(), std::forward<TPredicate>(Predicate));
}

template <typename ElementType>
template <typename TFunction>
void VArray<ElementType>::ForEach(TFunction&& Function)
{
	for (auto& Element : *this)
	{
		Function(Element);
	}
}

template <typename ElementType>
template <typename TFunction>
void VArray<ElementType>::ForEach(TFunction&& Function) const
{
	for (const auto& Element : *this)
	{
		Function(Element);
	}
}

template <typename ElementType>
template <typename... TArgs>
ElementType& VArray<ElementType>::Emplace(TArgs&&... Arguments)
{
	GrowFor(Indices + 1);
	VAllocator<ElementType>::Construct(Data + Indices, std::forward<TArgs>(Arguments)...);
	return Data[Indices++];
}

template <typename ElementType>
template <typename... TArgs>
ElementType& VArray<ElementType>::EmplaceAt(const int32 ArrayIndex, TArgs&&... Arguments)
{
	assert(ArrayIndex >= 0 && ArrayIndex <= Indices);
	GrowFor(Indices + 1);

	if (ArrayIndex < Indices)
	{
		VAllocator<ElementType>::Construct(Data + Indices, std::move_if_noexcept(Data[Indices - 1]));

		for (int32 Current = Indices - 1; Current > ArrayIndex; Current--)
		{
			Data[Current] = std::move(Data[Current - 1]);
		}

		VAllocator<ElementType>::Destroy(Data + ArrayIndex);

		VAllocator<ElementType>::Construct(Data + ArrayIndex, std::forward<TArgs>(Arguments)...);
	}
	else
	{
		VAllocator<ElementType>::Construct(Data + ArrayIndex, std::forward<TArgs>(Arguments)...);
	}

	Indices++;
	return Data[ArrayIndex];
}

template <typename ElementType>
template <typename TPredicate>
int32 VArray<ElementType>::RemoveAll(TPredicate&& Predicate)
{
	int32 Removed = 0;
	int32 WriteIndex = 0;

	for (int32 ReadIndex = 0; ReadIndex < Indices; ReadIndex++)
	{
		if (Predicate(Data[ReadIndex]))
		{
			Removed++;
			VAllocator<ElementType>::Destroy(Data + ReadIndex);
			continue;
		}

		if (WriteIndex != ReadIndex)
		{
			VAllocator<ElementType>::Construct(Data + WriteIndex, std::move(Data[ReadIndex]));
			VAllocator<ElementType>::Destroy(Data + ReadIndex);
		}

		WriteIndex++;
	}

	Indices = WriteIndex;
	return Removed;
}

template <typename ElementType>
template <typename TPredicate>
ElementType* VArray<ElementType>::FindByPredicate(TPredicate&& Predicate)
{
	for (int32 Index = 0; Index < Indices; Index++)
	{
		if (Predicate(Data[Index])) return &Data[Index];
	}

	return nullptr;
}

template <typename ElementType>
template <typename TPredicate>
const ElementType* VArray<ElementType>::FindByPredicate(TPredicate&& Predicate) const
{
	for (int32 Index = 0; Index < Indices; Index++)
	{
		if (Predicate(Data[Index])) return &Data[Index];
	}

	return nullptr;
}

template <typename ElementType>
template <typename TPredicate>
int32 VArray<ElementType>::IndexOfByPredicate(TPredicate&& Predicate) const
{
	for (int32 Index = 0; Index < Indices; Index++)
	{
		if (Predicate(Data[Index])) return Index;
	}

	return INDEX_NONE;
}

template <typename ElementType>
void VArray<ElementType>::Init(const ElementType& Element, const int32 NewCapacity)
{
	assert(NewCapacity >= 0);
	Reserve(NewCapacity);

	for (int32 Index = 0; Index < NewCapacity; Index++)
	{
		Emplace(Element);
	}
}

template <typename ElementType>
void VArray<ElementType>::Reset()
{
	VAllocator<ElementType>::DestroyRange(Data, Indices);
	Indices = 0;
}

template <typename ElementType>
void VArray<ElementType>::Reserve(const int32 NewCapacity)
{
	if (NewCapacity <= Capacity) return;
	Relocate(NewCapacity);
}

template <typename ElementType>
void VArray<ElementType>::Shrink()
{
	if (Indices == Capacity) return;

	if (Indices == 0)
	{
		Empty();
		return;
	}

	Relocate(Indices);
}

template <typename ElementType>
void VArray<ElementType>::Empty()
{
	Reset();
	VAllocator<ElementType>::Deallocate(Data, Capacity);

	Data = nullptr;
	Capacity = 0;
}

template <typename ElementType>
void VArray<ElementType>::Swap(VArray& OtherArray)
{
	std::swap(Data, OtherArray.Data);
	std::swap(Indices, OtherArray.Indices);
	std::swap(Capacity, OtherArray.Capacity);
}

template <typename ElementType>
void VArray<ElementType>::Add(ElementType&& Element)
{
	Emplace(std::move(Element));
}

template <typename ElementType>
void VArray<ElementType>::Add(const ElementType& Element)
{
	Emplace(Element);
}

template <typename ElementType>
void VArray<ElementType>::AddDefaulted(const int32 Count)
{
	if (Count <= 0) return;
	GrowFor(Indices + Count);

	for (int32 Index = 0; Index < Count; Index++)
	{
		VAllocator<ElementType>::Construct(Data + Indices++);
	}
}

template <typename ElementType>
ElementType& VArray<ElementType>::AddDefaulted()
{
	return Emplace();
}

template <typename ElementType>
void VArray<ElementType>::Append(VArray&& OtherArray)
{
	if (this == &OtherArray) return;
	GrowFor(Indices + OtherArray.Indices);

	for (int32 Index = 0; Index < OtherArray.Indices; Index++)
	{
		VAllocator<ElementType>::Construct(Data + Indices++, std::move(OtherArray.Data[Index]));
	}

	OtherArray.Reset();
}

template <typename ElementType>
void VArray<ElementType>::Append(const VArray& OtherArray)
{
	if (this == &OtherArray)
	{
		const VArray Copy(OtherArray);
		Append(Copy);
		return;
	}

	GrowFor(Indices + OtherArray.Indices);

	for (int32 Index = 0; Index < OtherArray.Indices; Index++)
	{
		VAllocator<ElementType>::Construct(Data + Indices++, OtherArray.Data[Index]);
	}
}

template <typename ElementType>
void VArray<ElementType>::Append(const ElementType* Elements, const int32 Count)
{
	if (!Elements || Count <= 0) return;
	GrowFor(Indices + Count);

	for (int32 Index = 0; Index < Count; Index++)
	{
		VAllocator<ElementType>::Construct(Data + Indices++, Elements[Index]);
	}
}

template <typename ElementType>
void VArray<ElementType>::Append(std::initializer_list<ElementType> Elements)
{
	Append(Elements.begin(), static_cast<int32>(Elements.size()));
}

template <typename ElementType>
void VArray<ElementType>::Insert(ElementType&& Element, const int32 ArrayIndex)
{
	EmplaceAt(ArrayIndex, std::move(Element));
}

template <typename ElementType>
void VArray<ElementType>::Insert(const ElementType& Element, const int32 ArrayIndex)
{
	EmplaceAt(ArrayIndex, Element);
}

template <typename ElementType>
bool VArray<ElementType>::Remove(const ElementType& Element)
{
	const int32 Index = Find(Element);
	if (Index == -1) return false;

	RemoveAt(Index);
	return true;
}

template <typename ElementType>
bool VArray<ElementType>::RemoveSingle(const ElementType& Element)
{
	return Remove(Element);
}

template <typename ElementType>
void VArray<ElementType>::RemoveAt(const int32 ArrayIndex)
{
	assert(IsValidIndex(ArrayIndex));

	for (int32 CurrentIndex = ArrayIndex; CurrentIndex < Indices - 1; CurrentIndex++)
	{
		Data[CurrentIndex] = std::move(Data[CurrentIndex + 1]);
	}

	VAllocator<ElementType>::Destroy(Data + Indices - 1);
	Indices--;
}

template <typename ElementType>
void VArray<ElementType>::RemoveAtSwap(const int32 ArrayIndex)
{
	assert(IsValidIndex(ArrayIndex));
	const int32 LastIndex = Indices - 1;

	if (ArrayIndex != LastIndex)
	{
		Data[ArrayIndex] = std::move(Data[LastIndex]);
	}

	VAllocator<ElementType>::Destroy(Data + LastIndex);
	Indices--;
}

template <typename ElementType>
VArray<ElementType>::Iterator VArray<ElementType>::Begin()
{
	return Data;
}

template <typename ElementType>
VArray<ElementType>::Iterator VArray<ElementType>::begin()
{
	return Data;
}

template <typename ElementType>
VArray<ElementType>::ConstIterator VArray<ElementType>::Begin() const
{
	return Data;
}

template <typename ElementType>
VArray<ElementType>::ConstIterator VArray<ElementType>::begin() const
{
	return Data;
}

template <typename ElementType>
VArray<ElementType>::Iterator VArray<ElementType>::End()
{
	return Data + Indices;
}

template <typename ElementType>
VArray<ElementType>::Iterator VArray<ElementType>::end()
{
	return Data + Indices;
}

template <typename ElementType>
VArray<ElementType>::ConstIterator VArray<ElementType>::End() const
{
	return Data + Indices;
}

template <typename ElementType>
VArray<ElementType>::ConstIterator VArray<ElementType>::end() const
{
	return Data + Indices;
}

template <typename ElementType>
ElementType* VArray<ElementType>::GetData()
{
	return Data;
}

template <typename ElementType>
const ElementType* VArray<ElementType>::GetData() const
{
	return Data;
}

template <typename ElementType>
int32 VArray<ElementType>::Num() const
{
	return Indices;
}

template <typename ElementType>
int32 VArray<ElementType>::Max() const
{
	return Capacity;
}

template <typename ElementType>
int32 VArray<ElementType>::Find(const ElementType& Element) const
{
	return IndexOfByPredicate([&](const ElementType& Current)
		{
			return Current == Element;
		});
}

template <typename ElementType>
int32 VArray<ElementType>::FindLast(const ElementType& Element) const
{
	for (int32 Index = Indices - 1; Index >= 0; Index--)
	{
		if (Data[Index] == Element) return Index;
	}

	return INDEX_NONE;
}

template <typename ElementType>
bool VArray<ElementType>::IsEmpty() const
{
	return Indices == 0;
}

template <typename ElementType>
bool VArray<ElementType>::Contains(const ElementType& Element) const
{
	return Find(Element) != INDEX_NONE;
}

template <typename ElementType>
bool VArray<ElementType>::IsValidIndex(const int32 ArrayIndex) const
{
	return ArrayIndex >= 0 && ArrayIndex < Indices;
}

template <typename ElementType>
VArray<ElementType>& VArray<ElementType>::operator=(VArray&& OtherArray) noexcept
{
	if (this == &OtherArray) return *this;

	Empty();
	Data = OtherArray.Data;
	Indices = OtherArray.Indices;
	Capacity = OtherArray.Capacity;

	OtherArray.Data = nullptr;
	OtherArray.Indices = 0;
	OtherArray.Capacity = 0;
	return *this;
}
	
template <typename ElementType>
VArray<ElementType>& VArray<ElementType>::operator=(const VArray& OtherArray)
{
	if (this == &OtherArray) return *this;

	Empty();
	Reserve(OtherArray.Indices);

	for (int32 Index = 0; Index < OtherArray.Indices; Index++)
	{
		VAllocator<ElementType>::Construct(Data + Index, OtherArray.Data[Index]);
	}

	Indices = OtherArray.Indices;
	return *this;
}
	
template <typename ElementType>
bool VArray<ElementType>::operator==(const VArray& OtherArray) const
{
	if (Indices != OtherArray.Indices) return false;

	for (int32 Index = 0; Index < Indices; Index++)
	{
		if (!(Data[Index] == OtherArray.Data[Index])) return false;
	}

	return true;
}

template <typename ElementType>
bool VArray<ElementType>::operator!=(const VArray& OtherArray) const
{
	return !(*this == OtherArray);
}

template <typename ElementType>
ElementType& VArray<ElementType>::operator[](const int32 ArrayIndex)
{
	assert(IsValidIndex(ArrayIndex));
	return Data[ArrayIndex];
}

template <typename ElementType>
const ElementType& VArray<ElementType>::operator[](const int32 ArrayIndex) const
{
	assert(IsValidIndex(ArrayIndex));
	return Data[ArrayIndex];
}

template <typename ElementType>
void VArray<ElementType>::GrowFor(const int32 RequiredNumber)
{
	if (RequiredNumber <= Capacity) return;
	int32 NewCapacity = Capacity > 0 ? Capacity * 2 : 4;

	while (NewCapacity < RequiredNumber)
	{
		NewCapacity *= 2;
	}

	Relocate(NewCapacity);
}

template <typename ElementType>
void VArray<ElementType>::Relocate(const int32 NewCapacity)
{
	ElementType* NewData = VAllocator<ElementType>::Allocate(NewCapacity);

	for (int32 Index = 0; Index < Indices; Index++)
	{
		VAllocator<ElementType>::Construct(NewData + Index, std::move_if_noexcept(Data[Index]));
	}

	VAllocator<ElementType>::DestroyRange(Data, Indices);
	VAllocator<ElementType>::Deallocate(Data, Capacity);

	Data = NewData;
	Capacity = NewCapacity;
}
}