// Vertex

#pragma once

#include <cassert>
#include <utility>

namespace VCore
{
template <typename ElementType>
VArray<ElementType>::~VArray()
{
	Empty();
}

template <typename ElementType>
VArray<ElementType>::VArray(const VArray& OtherArray)
{
	if (OtherArray.Indices == 0) return;

	Data = static_cast<ElementType*>(::operator new(sizeof(ElementType) * OtherArray.Indices));
	Capacity = OtherArray.Indices;

	for (int32 Index = 0; Index < OtherArray.Indices; ++Index)
	{
		new (&Data[Index]) ElementType(OtherArray.Data[Index]);
	}

	Indices = OtherArray.Indices;
}

template <typename ElementType>
VArray<ElementType>::VArray(VArray&& OtherArray) noexcept
	: Data(OtherArray.Data), Indices(OtherArray.Indices), Capacity(OtherArray.Capacity)
{
	OtherArray.Data = nullptr;
	OtherArray.Indices = 0;
	OtherArray.Capacity = 0;
}

template <typename ElementType>
void VArray<ElementType>::Init(const ElementType& Element, const int32 NewCapacity)
{
	Empty();
	Reserve(NewCapacity);
	
	for (int32 ArrayIndex = 0; ArrayIndex < NewCapacity; ArrayIndex++)
	{
		Add(Element);
	}
}

template <typename ElementType>
void VArray<ElementType>::Reserve(const int32 NewCapacity)
{
	if (NewCapacity <= Capacity) return;
	ElementType* NewData = static_cast<ElementType*>(::operator new(sizeof(ElementType) * NewCapacity));

	for (int32 Index = 0; Index < Indices; ++Index)
	{
		new (&NewData[Index]) ElementType(std::move(Data[Index]));
		Data[Index].~ElementType();
	}

	::operator delete(Data);
	Data = NewData;
	Capacity = NewCapacity;
}

template <typename ElementType>
void VArray<ElementType>::Add(const ElementType& Element)
{
	if (Indices >= Capacity)
	{
		Reserve(Capacity == 0 ? 1 : Capacity * 2);
	}

	new (&Data[Indices]) ElementType(Element);
	Indices++;
}

template <typename ElementType>
void VArray<ElementType>::Add(ElementType&& Element)
{
	if (Indices >= Capacity)
	{
		Reserve(Capacity == 0 ? 1 : Capacity * 2);
	}

	new (&Data[Indices]) ElementType(std::move(Element));
	Indices++;
}

template <typename ElementType>
void VArray<ElementType>::RemoveAt(int32 ArrayIndex)
{
	if (ArrayIndex < 0 || ArrayIndex >= Indices) return;
	Data[ArrayIndex].~ElementType();

	for (int32 CurrentIndex = ArrayIndex; CurrentIndex < Indices - 1; ++CurrentIndex)
	{
		new (&Data[CurrentIndex]) ElementType(std::move(Data[CurrentIndex + 1]));
		Data[CurrentIndex + 1].~ElementType();
	}

	Indices--;
}

template <typename ElementType>
void VArray<ElementType>::Empty()
{
	for (int32 Index = 0; Index < Indices; ++Index)
	{
		Data[Index].~ElementType();
	}

	::operator delete(Data);
	Data = nullptr;
	Indices = 0;
	Capacity = 0;
}

template <typename ElementType>
template <typename TFunction>
void VArray<ElementType>::ForEach(TFunction&& Function)
{
	for (ElementType* Iterator = Begin(); Iterator != End(); ++Iterator)
	{
		Function(*Iterator);
	}
}

template <typename ElementType>
ElementType* VArray<ElementType>::Begin()
{
	return Data;
}

template <typename ElementType>
ElementType* VArray<ElementType>::begin()
{
	return Data;
}

template <typename ElementType>
const ElementType* VArray<ElementType>::Begin() const
{
	return Data;
}

template <typename ElementType>
const ElementType* VArray<ElementType>::begin() const
{
	return Data;
}

template <typename ElementType>
ElementType* VArray<ElementType>::End()
{
	return Data + Indices;
}

template <typename ElementType>
ElementType* VArray<ElementType>::end()
{
	return Data + Indices;
}

template <typename ElementType>
const ElementType* VArray<ElementType>::End() const
{
	return Data + Indices;
}

template <typename ElementType>
const ElementType* VArray<ElementType>::end() const
{
	return Data + Indices;
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
bool VArray<ElementType>::IsEmpty() const
{
	return Indices == 0;
}

template <typename ElementType>
bool VArray<ElementType>::IsValidIndex(const int32 ArrayIndex) const
{
	return ArrayIndex >= 0 && ArrayIndex < Indices;
}

template <typename ElementType>
VArray<ElementType>& VArray<ElementType>::operator=(const VArray& OtherArray)
{
	if (this == &OtherArray) return *this;
	
	Empty();
	if (OtherArray.Indices == 0) return *this;

	Data = static_cast<ElementType*>(::operator new(sizeof(ElementType) * OtherArray.Indices));
	Capacity = OtherArray.Indices;

	for (int32 Index = 0; Index < OtherArray.Indices; ++Index)
	{
		new (&Data[Index]) ElementType(OtherArray.Data[Index]);
	}

	Indices = OtherArray.Indices;
	return *this;
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
ElementType& VArray<ElementType>::operator[](int32 ArrayIndex)
{
	assert(ArrayIndex >= 0 && ArrayIndex < Indices);
	return Data[ArrayIndex];
}

template <typename ElementType>
const ElementType& VArray<ElementType>::operator[](int32 ArrayIndex) const
{
	assert(ArrayIndex >= 0 && ArrayIndex < Indices);
	return Data[ArrayIndex];
}
}
