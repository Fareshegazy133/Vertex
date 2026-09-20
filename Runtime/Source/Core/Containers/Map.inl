// Vertex

#pragma once

namespace VCore
{
template <typename KeyType, typename ValueType>
VMap<KeyType, ValueType>::VMap(VMap&& OtherMap) noexcept
	: Elements(std::move(OtherMap.Elements)),NextLinks(std::move(OtherMap.NextLinks)), HashTable(std::move(OtherMap.HashTable))
{}

template <typename KeyType, typename ValueType>
VMap<KeyType, ValueType>::VMap(const VMap& OtherMap)
{
	Reserve(OtherMap.Num());

	for (const auto& Element : OtherMap.Elements)
	{
		Add(Element.Key, Element.Value);
	}
}

template <typename KeyType, typename ValueType>
VMap<KeyType, ValueType>::VMap(const int32 InitialCapacity)
	: Elements(InitialCapacity), HashTable(InitialCapacity * 2)
{
	NextLinks.Reserve(InitialCapacity);
}

template <typename KeyType, typename ValueType>
template <typename TFunction>
void VMap<KeyType, ValueType>::ForEach(TFunction&& Function)
{
	for (auto& Element : Elements)
	{
		Function(Element.Key, Element.Value);
	}
}

template <typename KeyType, typename ValueType>
template <typename TFunction>
void VMap<KeyType, ValueType>::ForEach(TFunction&& Function) const
{
	for (const auto& Element : Elements)
	{
		Function(Element.Key, Element.Value);
	}
}

template <typename KeyType, typename ValueType>
template <typename ... TArgs>
ValueType& VMap<KeyType, ValueType>::Emplace(KeyType Key, TArgs&&... Arguments)
{
	const int32 Existing = FindIndex(Key);
	if (Existing != INDEX_NONE) return Elements[Existing].Value;
	
	VElement Element;
	Element.Key = std::move(Key);
	Element.Value = ValueType(std::forward<TArgs>(Arguments)...);
	Element.Hash = VHash<KeyType>::Hash(Element.Key);

	const int32 Index = AddElement(std::move(Element));
	return Elements[Index].Value;
}

template <typename KeyType, typename ValueType>
template <typename TPredicate>
int32 VMap<KeyType, ValueType>::RemoveAll(TPredicate&& Predicate)
{
	VArray<KeyType> KeysToRemove;
	int32 Removed = 0;
	
	for (const auto& Element : Elements)
	{
		if (Predicate(Element.Key, Element.Value))
		{
			KeysToRemove.Add(Element.Key);
		}
	}

	for (const auto& Key : KeysToRemove)
	{
		if (Remove(Key))
		{
			Removed++;
		}
	}

	return Removed;
}

template <typename KeyType, typename ValueType>
template <typename... TArgs>
ValueType& VMap<KeyType, ValueType>::FindOrAdd(const KeyType& Key, TArgs&&... Arguments)
{
	if (ValueType* ExistingValue = Find(Key)) return *ExistingValue;
	return Emplace(Key, std::forward<TArgs>(Arguments)...);
}

template <typename KeyType, typename ValueType>
void VMap<KeyType, ValueType>::Reset()
{
	Elements.Reset();
	NextLinks.Reset();
	HashTable.Reset();
}

template <typename KeyType, typename ValueType>
void VMap<KeyType, ValueType>::Reserve(const int32 NewCapacity)
{
	if (NewCapacity <= 0) return;
	Elements.Reserve(NewCapacity);
	NextLinks.Reserve(NewCapacity);
	HashTable.Reserve(NewCapacity);
}

template <typename KeyType, typename ValueType>
void VMap<KeyType, ValueType>::Shrink()
{
	RebuildHashTable();
}

template <typename KeyType, typename ValueType>
void VMap<KeyType, ValueType>::Empty()
{
	Elements.Empty();
	NextLinks.Empty();
	HashTable.Empty();
}

template <typename KeyType, typename ValueType>
void VMap<KeyType, ValueType>::Swap(VMap& OtherMap) noexcept
{
}

template <typename KeyType, typename ValueType>
ValueType& VMap<KeyType, ValueType>::Add(KeyType&& Key, ValueType&& Value)
{
	const int32 Existing = FindIndex(Key);

	if (Existing != INDEX_NONE)
	{
		Elements[Existing].Value = std::move(Value);
		return Elements[Existing].Value;
	}

	VElement Element;
	Element.Key = std::move(Key);
	Element.Value = std::move(Value);
	Element.Hash = VHash<KeyType>::Hash(Element.Key);

	const int32 Index = AddElement(std::move(Element));
	return Elements[Index].Value;
}

template <typename KeyType, typename ValueType>
ValueType& VMap<KeyType, ValueType>::Add(const KeyType& Key, ValueType&& Value)
{
	const int32 Existing = FindIndex(Key);

	if (Existing != INDEX_NONE)
	{
		Elements[Existing].Value = std::move(Value);
		return Elements[Existing].Value;
	}

	VElement Element;
	Element.Key = Key;
	Element.Value = std::move(Value);
	Element.Hash = VHash<KeyType>::Hash(Key);

	const int32 Index = AddElement(std::move(Element));
	return Elements[Index].Value;
}

template <typename KeyType, typename ValueType>
ValueType& VMap<KeyType, ValueType>::Add(const KeyType& Key, const ValueType& Value)
{
	const int32 Existing = FindIndex(Key);

	if (Existing != INDEX_NONE)
	{
		Elements[Existing].Value = Value;
		return Elements[Existing].Value;
	}

	VElement Element;
	Element.Key = Key;
	Element.Value = Value;
	Element.Hash = VHash<KeyType>::Hash(Key);

	const int32 Index = AddElement(std::move(Element));
	return Elements[Index].Value;
}

template <typename KeyType, typename ValueType>
bool VMap<KeyType, ValueType>::TryAdd(const KeyType& Key, const ValueType& Value)
{
	if (Contains(Key)) return false;
	Add(Key, Value);
	return true;
}

template <typename KeyType, typename ValueType>
bool VMap<KeyType, ValueType>::Remove(const KeyType& Key)
{
	const int32 Index = FindIndex(Key);
	if (Index == INDEX_NONE) return false;

	const uint32 Hash = Elements[Index].Hash;
	HashTable.Remove(NextLinks, Index, Hash);
	Elements.RemoveAt(Index);
	
	NextLinks[Index] = INDEX_NONE;
	return true;
}

template <typename KeyType, typename ValueType>
void VMap<KeyType, ValueType>::GenerateKeyArray(VArray<KeyType>& OutKeys) const
{
	OutKeys.Reset();
	OutKeys.Reserve(Num());

	for (const auto& Element : Elements)
	{
		OutKeys.Add(Element.Key);
	}
}

template <typename KeyType, typename ValueType>
void VMap<KeyType, ValueType>::GenerateValueArray(VArray<ValueType>& OutValues) const
{
	OutValues.Reset();
	OutValues.Reserve(Num());

	for (const auto& Element : Elements)
	{
		OutValues.Add(Element.Value);
	}
}

template <typename KeyType, typename ValueType>
VArray<KeyType> VMap<KeyType, ValueType>::GetKeys() const
{
	VArray<KeyType> Result;
	Result.Reserve(Num());

	for (const auto& Element : Elements)
	{
		Result.Add(Element.Key);
	}

	return Result;
}

template <typename KeyType, typename ValueType>
VArray<ValueType> VMap<KeyType, ValueType>::GetValues() const
{
	VArray<ValueType> Result;
	Result.Reserve(Num());

	for (const auto& Element : Elements)
	{
		Result.Add(Element.Value);
	}

	return Result;
}

template <typename KeyType, typename ValueType>
VMap<KeyType, ValueType>::Iterator VMap<KeyType, ValueType>::Begin()
{
	return Iterator(this, 0);
}

template <typename KeyType, typename ValueType>
VMap<KeyType, ValueType>::Iterator VMap<KeyType, ValueType>::begin()
{
	return Iterator(this, 0);
}

template <typename KeyType, typename ValueType>
VMap<KeyType, ValueType>::ConstIterator VMap<KeyType, ValueType>::Begin() const
{
	return ConstIterator(this, 0);
}

template <typename KeyType, typename ValueType>
VMap<KeyType, ValueType>::ConstIterator VMap<KeyType, ValueType>::begin() const
{
	return ConstIterator(this, 0);
}

template <typename KeyType, typename ValueType>
VMap<KeyType, ValueType>::Iterator VMap<KeyType, ValueType>::End()
{
	return Iterator(this, Elements.Max());
}

template <typename KeyType, typename ValueType>
VMap<KeyType, ValueType>::Iterator VMap<KeyType, ValueType>::end()
{
	return Iterator(this, Elements.Max());
}

template <typename KeyType, typename ValueType>
VMap<KeyType, ValueType>::ConstIterator VMap<KeyType, ValueType>::End() const
{
	return ConstIterator(this, Elements.Max());
}

template <typename KeyType, typename ValueType>
VMap<KeyType, ValueType>::ConstIterator VMap<KeyType, ValueType>::end() const
{
	return ConstIterator(this, Elements.Max());
}

template <typename KeyType, typename ValueType>
ValueType* VMap<KeyType, ValueType>::Find(const KeyType& Key)
{
	const int32 Index = FindIndex(Key);
	return Index == INDEX_NONE ? nullptr : &Elements[Index].Value;
}

template <typename KeyType, typename ValueType>
const ValueType* VMap<KeyType, ValueType>::Find(const KeyType& Key) const
{
	const int32 Index = FindIndex(Key);
	return Index == INDEX_NONE ? nullptr : &Elements[Index].Value;
}

template <typename KeyType, typename ValueType>
ValueType& VMap<KeyType, ValueType>::FindChecked(const KeyType& Key)
{
	ValueType* Value = Find(Key);
	assert(Value);
	return *Value;
}

template <typename KeyType, typename ValueType>
const ValueType& VMap<KeyType, ValueType>::FindChecked(const KeyType& Key) const
{
	const ValueType* Value = Find(Key);
	assert(Value);
	return *Value;
}

template <typename KeyType, typename ValueType>
ValueType& VMap<KeyType, ValueType>::FindOrAdd(const KeyType& Key)
{
	if (ValueType* ExistingValue = Find(Key)) return *ExistingValue;
	return Emplace(Key, ValueType{});
}

template <typename KeyType, typename ValueType>
bool VMap<KeyType, ValueType>::TryGetValue(const KeyType& Key, ValueType*& OutValue)
{
	OutValue = Find(Key);
	return OutValue;
}

template <typename KeyType, typename ValueType>
bool VMap<KeyType, ValueType>::TryGetValue(const KeyType& Key, const ValueType*& OutValue) const
{
	OutValue = Find(Key);
	return OutValue;
}

template <typename KeyType, typename ValueType>
int32 VMap<KeyType, ValueType>::Num() const
{
	return Elements.Num();
}

template <typename KeyType, typename ValueType>
int32 VMap<KeyType, ValueType>::Max() const
{
	return Elements.Max();
}

template <typename KeyType, typename ValueType>
bool VMap<KeyType, ValueType>::IsEmpty() const
{
	return Elements.IsEmpty();
}

template <typename KeyType, typename ValueType>
bool VMap<KeyType, ValueType>::Contains(const KeyType& Key) const
{
	return FindIndex(Key) != INDEX_NONE;
}

template <typename KeyType, typename ValueType>
VMap<KeyType, ValueType>& VMap<KeyType, ValueType>::operator=(VMap&& OtherMap) noexcept
{
	if (this == &OtherMap) return *this;
	Elements = std::move(OtherMap.Elements);
	NextLinks = std::move(OtherMap.NextLinks);
	HashTable = std::move(OtherMap.HashTable);
	return *this;
}

template <typename KeyType, typename ValueType>
VMap<KeyType, ValueType>& VMap<KeyType, ValueType>::operator=(const VMap& OtherMap)
{
	if (this == &OtherMap) return *this;

	Empty();
	Reserve(OtherMap.Num());

	for (const auto& Element : OtherMap.Elements)
	{
		Add(Element.Key, Element.Value);
	}

	return *this;
}

template <typename KeyType, typename ValueType>
ValueType& VMap<KeyType, ValueType>::operator[](const KeyType& Key)
{
	return FindOrAdd(Key);
}

template <typename KeyType, typename ValueType>
const ValueType& VMap<KeyType, ValueType>::operator[](const KeyType& Key) const
{
	return FindChecked(Key);
}

template <typename KeyType, typename ValueType>
template <typename TKey, typename TValue>
VMap<KeyType, ValueType>::VElement::VElement(TKey&& InKey, TValue&& InValue)
	: Key(std::forward<TKey>(InKey)), Value(std::forward<TValue>(InValue)), Hash(VHash<KeyType>::Hash(Key))
{}

template <typename KeyType, typename ValueType>
void VMap<KeyType, ValueType>::RebuildHashTable()
{
	HashTable.Rehash(Elements.Max() * 2);

	for (int32 Index = 0; Index < NextLinks.Num(); Index++)
	{
		NextLinks[Index] = INDEX_NONE;
	}

	for (Iterator Iterator = Elements.Begin(); Iterator != Elements.End(); ++Iterator)
	{
		const int32 Index = Iterator.GetIndex();
		HashTable.Insert(Elements[Index].Hash, Index, NextLinks);
	}
}

template <typename KeyType, typename ValueType>
int32 VMap<KeyType, ValueType>::AddElement(VElement&& Element)
{
	const int32 Index = Elements.Emplace(std::move(Element));

	if (NextLinks.Max() <= Index)
	{
		NextLinks.Reserve(Elements.Max());
	}

	if (NextLinks.Num() <= Index)
	{
		while (NextLinks.Num() <= Index)
		{
			NextLinks.Add(INDEX_NONE);
		}
	}

	HashTable.Insert(NextLinks, Index, Elements[Index].Hash);
	return Index;
}

template <typename KeyType, typename ValueType>
int32 VMap<KeyType, ValueType>::FindIndex(const KeyType& Key) const
{
	if (Elements.Num() == 0) return INDEX_NONE;
	const uint32 Hash = VHash<KeyType>::Hash(Key);

	return HashTable.Find(NextLinks, [&](const int32 Index)
		{
			const VElement& Element = Elements[Index];
			return Element.Hash == Hash && Element.Key == Key;
		}, Hash);
}

template <typename KeyType, typename ValueType>
VMap<KeyType, ValueType>::Iterator::Iterator(VMap* InOwner, const int32 InIndex)
	: Owner(InOwner), Index(InIndex)
{
	Advance();
}

template <typename KeyType, typename ValueType>
KeyType& VMap<KeyType, ValueType>::Iterator::Key() const
{
	return Owner->Elements[Index].Key;
}

template <typename KeyType, typename ValueType>
ValueType& VMap<KeyType, ValueType>::Iterator::Value() const
{
	return Owner->Elements[Index].Value;
}

template <typename KeyType, typename ValueType>
int32 VMap<KeyType, ValueType>::Iterator::GetIndex() const
{
	return Index;
}

template <typename KeyType, typename ValueType>
VPair<KeyType&, ValueType&> VMap<KeyType, ValueType>::Iterator::operator*() const
{
	VElement& Element = Owner->Elements[Index];
	return VPair(Element.Key, Element.Value);
}

template <typename KeyType, typename ValueType>
typename VMap<KeyType, ValueType>::Iterator& VMap<KeyType, ValueType>::Iterator::operator++()
{
	Index++;
	Advance();
	return *this;
}

template <typename KeyType, typename ValueType>
bool VMap<KeyType, ValueType>::Iterator::operator==(const Iterator& OtherIterator) const
{
	return Owner == OtherIterator.Owner && Index == OtherIterator.Index;
}

template <typename KeyType, typename ValueType>
bool VMap<KeyType, ValueType>::Iterator::operator!=(const Iterator& OtherIterator) const
{
	return !(*this == OtherIterator);
}

template <typename KeyType, typename ValueType>
void VMap<KeyType, ValueType>::Iterator::Advance()
{
	while (Owner && Index < Owner->Elements.Max() && !Owner->Elements.IsValidIndex(Index))
	{
		Index++;
	}
}

template <typename KeyType, typename ValueType>
VMap<KeyType, ValueType>::ConstIterator::ConstIterator(const VMap* InOwner, const int32 InIndex)
	: Owner(InOwner), Index(InIndex)
{
	Advance();
}

template <typename KeyType, typename ValueType>
const KeyType& VMap<KeyType, ValueType>::ConstIterator::Key() const
{
	return Owner->Elements[Index].Key;
}

template <typename KeyType, typename ValueType>
const ValueType& VMap<KeyType, ValueType>::ConstIterator::Value() const
{
	return Owner->Elements[Index].Value;
}

template <typename KeyType, typename ValueType>
int32 VMap<KeyType, ValueType>::ConstIterator::GetIndex() const
{
	return Index;
}

template <typename KeyType, typename ValueType>
VPair<const KeyType&, const ValueType&> VMap<KeyType, ValueType>::ConstIterator::operator*() const
{
	const VElement& Element = Owner->Elements[Index];
	return VPair(Element.Key, Element.Value);
}

template <typename KeyType, typename ValueType>
typename VMap<KeyType, ValueType>::ConstIterator& VMap<KeyType, ValueType>::ConstIterator::operator++()
{
	Index++;
	Advance();
	return *this;
}

template <typename KeyType, typename ValueType>
bool VMap<KeyType, ValueType>::ConstIterator::operator==(const ConstIterator& OtherConstIterator) const
{
	return Owner == OtherConstIterator.Owner && Index == OtherConstIterator.Index;
}

template <typename KeyType, typename ValueType>
bool VMap<KeyType, ValueType>::ConstIterator::operator!=(const ConstIterator& OtherConstIterator) const
{
	return !(*this == OtherConstIterator);
}

template <typename KeyType, typename ValueType>
void VMap<KeyType, ValueType>::ConstIterator::Advance()
{
	while (Owner && Index < Owner->Elements.Max() && !Owner->Elements.IsValidIndex(Index))
	{
		Index++;
	}
}
}