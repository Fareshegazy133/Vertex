// Vertex

#pragma once

namespace VCore
{
template <typename ElementType>
VSet<ElementType>::VSet(VSet&& OtherSet) noexcept
	: Elements(std::move(OtherSet.Elements)), NextLinks(std::move(OtherSet.NextLinks)), HashTable(OtherSet.HashTable)
{}

template <typename ElementType>
VSet<ElementType>::VSet(const VSet& OtherSet)
{
	Reserve(OtherSet.Num());

	for (const auto& Element : OtherSet.Elements)
	{
		Add(Element.Value);
	}
}

template <typename ElementType>
VSet<ElementType>::VSet(const int32 InitialCapacity)
	: Elements(InitialCapacity), HashTable(InitialCapacity * 2)
{
	NextLinks.Reserve(InitialCapacity);
}

template <typename ElementType>
template <typename TFunction>
void VSet<ElementType>::ForEach(TFunction&& Function)
{
	for (auto& Element : Elements)
	{
		Function(Element.Value);
	}
}

template <typename ElementType>
template <typename TFunction>
void VSet<ElementType>::ForEach(TFunction&& Function) const
{
	for (const auto& Element : Elements)
	{
		Function(Element.Value);
	}
}

template <typename ElementType>
template <typename ... TArgs>
ElementType& VSet<ElementType>::Emplace(TArgs&&... Arguments)
{
	ElementType Temporary(std::forward<TArgs>(Arguments)...);
	const int32 Existing = FindIndex(Temporary);

	if (Existing != INDEX_NONE) return Elements[Existing].Value;
	VElement NewElement(std::move(Temporary));

	const int32 Index = AddInternal(std::move(NewElement));
	return Elements[Index].Value;
}

template <typename ElementType>
template <typename TPredicate>
int32 VSet<ElementType>::RemoveAll(TPredicate&& Predicate)
{
	VArray<ElementType> ElementsToRemove;

	for (const auto& Element : Elements)
	{
		if (Predicate(Element.Value))
		{
			ElementsToRemove.Add(Element.Value);
		}
	}

	int32 Removed = 0;

	for (const auto& Element : ElementsToRemove)
	{
		if (Remove(Element))
		{
			Removed++;
		}
	}

	return Removed;
}

template <typename ElementType>
template <typename TPredicate>
ElementType* VSet<ElementType>::FindByPredicate(TPredicate&& Predicate)
{
	for (auto& Element : Elements)
	{
		if (Predicate(Element.Value)) return &Element.Value;
	}

	return nullptr;
}

template <typename ElementType>
template <typename TPredicate>
const ElementType* VSet<ElementType>::FindByPredicate(TPredicate&& Predicate) const
{
	for (const auto& Element : Elements)
	{
		if (Predicate(Element.Value)) return &Element.Value;
	}

	return nullptr;
}

template <typename ElementType>
void VSet<ElementType>::Reset()
{
	Elements.Reset();
	NextLinks.Reset();
	HashTable.Reset();
}

template <typename ElementType>
void VSet<ElementType>::Reserve(const int32 NewCapacity)
{
	if (NewCapacity <= 0) return;
	Elements.Reserve(NewCapacity);
	NextLinks.Reserve(NewCapacity);
	HashTable.Reserve(NewCapacity);
}

template <typename ElementType>
void VSet<ElementType>::Empty()
{
	Elements.Empty();
	NextLinks.Empty();
	HashTable.Empty();
}

template <typename ElementType>
bool VSet<ElementType>::Add(const ElementType& Element)
{
	if (Contains(Element)) return false;
	VElement NewElement(Element);

	AddInternal(std::move(NewElement));
	return true;
}

template <typename ElementType>
bool VSet<ElementType>::Add(ElementType&& Element)
{
	if (Contains(Element)) return false;
	VElement NewElement(std::move(Element));

	AddInternal(std::move(NewElement));
	return true;
}

template <typename ElementType>
bool VSet<ElementType>::Remove(const ElementType& Element)
{
	const int32 Index = FindIndex(Element);
	if (Index == INDEX_NONE) return false;

	HashTable.Remove(Elements[Index].Hash, Index, NextLinks);
	Elements.RemoveAt(Index);
	NextLinks[Index] = INDEX_NONE;
	return true;
}

template <typename ElementType>
VSet<ElementType>::Iterator VSet<ElementType>::Begin()
{
	return Iterator(this, 0);
}

template <typename ElementType>
VSet<ElementType>::Iterator VSet<ElementType>::begin()
{
	return Iterator(this, 0);
}

template <typename ElementType>
VSet<ElementType>::ConstIterator VSet<ElementType>::Begin() const
{
	return ConstIterator(this, 0);
}

template <typename ElementType>
VSet<ElementType>::ConstIterator VSet<ElementType>::begin() const
{
	return ConstIterator(this, 0);
}

template <typename ElementType>
VSet<ElementType>::Iterator VSet<ElementType>::End()
{
	return Iterator(this, Elements.Max());
}

template <typename ElementType>
VSet<ElementType>::Iterator VSet<ElementType>::end()
{
	return Iterator(this, Elements.Max());
}

template <typename ElementType>
VSet<ElementType>::ConstIterator VSet<ElementType>::End() const
{
	return ConstIterator(this, Elements.Max());
}

template <typename ElementType>
VSet<ElementType>::ConstIterator VSet<ElementType>::end() const
{
	return ConstIterator(this, Elements.Max());
}

template <typename ElementType>
int32 VSet<ElementType>::Num() const
{
	return Elements.Num();
}

template <typename ElementType>
int32 VSet<ElementType>::Max() const
{
	return Elements.Max();
}

template <typename ElementType>
bool VSet<ElementType>::IsEmpty() const
{
	return Elements.IsEmpty();
}

template <typename ElementType>
bool VSet<ElementType>::Contains(const ElementType& Element) const
{
	return FindIndex(Element) != INDEX_NONE;
}

template <typename ElementType>
VSet<ElementType>& VSet<ElementType>::operator=(const VSet& OtherSet)
{
	if (this == &OtherSet) return *this;

	Empty();
	Reserve(OtherSet.Num());

	for (const auto& Element : OtherSet.Elements)
	{
		Add(Element.Value);
	}

	return *this;
}

template <typename ElementType>
VSet<ElementType>& VSet<ElementType>::operator=(VSet&& OtherSet) noexcept
{
	if (this == &OtherSet) return *this;
	Elements = std::move(OtherSet.Elements);
	NextLinks = std::move(OtherSet.NextLinks);
	HashTable = std::move(OtherSet.HashTable);
	return *this;
}

template <typename ElementType>
template <typename TValue>
VSet<ElementType>::VElement::VElement(TValue&& InValue)
	: Value(std::forward<TValue>(InValue)), Hash(VHash<ElementType>::Hash(Value))
{}

template <typename ElementType>
int32 VSet<ElementType>::FindIndex(const ElementType& Element) const
{
	if (Elements.Num() == 0) return INDEX_NONE;

	const uint32 Hash = VHash<ElementType>::Hash(Element);

	return HashTable.Find(Hash, NextLinks, [&](const int32 Index)
		{
			const VElement& Current = Elements[Index];
			return Current.Hash == Hash && Current.Value == Element;
		});
}

template <typename ElementType>
int32 VSet<ElementType>::AddInternal(VElement&& Element)
{
	const int32 Index = Elements.Emplace(std::move(Element));

	while (NextLinks.Num() <= Index)
	{
		NextLinks.Add(INDEX_NONE);
	}

	HashTable.Insert(Elements[Index].Hash, Index, NextLinks);

	return Index;
}

template <typename ElementType>
VSet<ElementType>::Iterator::Iterator(VSet* InOwner, const int32 InIndex)
	: Owner(InOwner), Index(InIndex)
{
	Advance();
}

template <typename ElementType>
int32 VSet<ElementType>::Iterator::GetIndex() const
{
	return Index;
}

template <typename ElementType>
ElementType& VSet<ElementType>::Iterator::operator*() const
{
	return Owner->Elements[Index].Value;
}

template <typename ElementType>
ElementType* VSet<ElementType>::Iterator::operator->() const
{
	return &Owner->Elements[Index].Value;
}

template <typename ElementType>
VSet<ElementType>::Iterator& VSet<ElementType>::Iterator::operator++()
{
	Index++;
	Advance();
	return *this;
}

template <typename ElementType>
VSet<ElementType>::Iterator& VSet<ElementType>::Iterator::operator--()
{
	Index--;
	Retreat();
	return *this;
}

template <typename ElementType>
bool VSet<ElementType>::Iterator::operator==(const Iterator& OtherIterator) const
{
	return Owner == OtherIterator.Owner && Index == OtherIterator.Index;
}

template <typename ElementType>
bool VSet<ElementType>::Iterator::operator!=(const Iterator& OtherIterator) const
{
	return !(*this == OtherIterator);
}

template <typename ElementType>
void VSet<ElementType>::Iterator::Advance()
{
	while (Owner && Index < Owner->Elements.Max() && !Owner->Elements.IsValidIndex(Index))
	{
		Index++;
	}
}

template <typename ElementType>
void VSet<ElementType>::Iterator::Retreat()
{
	while (Owner && Index >= 0 && !Owner->Elements.IsValidIndex(Index))
	{
		Index--;
	}
}

template <typename ElementType>
VSet<ElementType>::ConstIterator::ConstIterator(const VSet* InOwner, const int32 InIndex)
	: Owner(InOwner), Index(InIndex)
{
	Advance();
}

template <typename ElementType>
int32 VSet<ElementType>::ConstIterator::GetIndex() const
{
	return Index;
}

template <typename ElementType>
const ElementType& VSet<ElementType>::ConstIterator::operator*() const
{
	return Owner->Elements[Index].Value;
}

template <typename ElementType>
const ElementType* VSet<ElementType>::ConstIterator::operator->() const
{
	return &Owner->Elements[Index].Value;
}

template <typename ElementType>
VSet<ElementType>::ConstIterator& VSet<ElementType>::ConstIterator::operator++()
{
	Index++;
	Advance();
	return *this;
}

template <typename ElementType>
VSet<ElementType>::ConstIterator& VSet<ElementType>::ConstIterator::operator--()
{
	Index--;
	Retreat();
	return *this;
}

template <typename ElementType>
bool VSet<ElementType>::ConstIterator::operator==(const ConstIterator& OtherConstIterator) const
{
	return Owner == OtherConstIterator.Owner && Index == OtherConstIterator.Index;
}

template <typename ElementType>
bool VSet<ElementType>::ConstIterator::operator!=(const ConstIterator& OtherConstIterator) const
{
	return !(*this == OtherConstIterator);
}

template <typename ElementType>
void VSet<ElementType>::ConstIterator::Advance()
{
	while (Owner && Index < Owner->Elements.Max() && !Owner->Elements.IsValidIndex(Index))
	{
		Index++;
	}
}

template <typename ElementType>
void VSet<ElementType>::ConstIterator::Retreat()
{
	while (Owner && Index >= 0 && !Owner->Elements.IsValidIndex(Index))
	{
		Index--;
	}
}
}