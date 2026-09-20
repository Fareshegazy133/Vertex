// Vertex

#pragma once

#include "Core/Containers/Hash.h"
#include "Core/Containers/HashTable.h"
#include "Core/Containers/Pair.h"
#include "Core/Containers/SparseArray.h"

namespace VCore
{
template<typename KeyType, typename ValueType>
class VMap
{
public:
	class Iterator;
	class ConstIterator;

public:
    VMap() = default;
    VMap(VMap&& OtherMap) noexcept;
	VMap(const VMap& OtherMap);
	explicit VMap(const int32 InitialCapacity);
    ~VMap() = default;
	
	template<typename TFunction>
	void ForEach(TFunction&& Function);

	template<typename TFunction>
	void ForEach(TFunction&& Function) const;
	
	template<typename... TArgs>
	ValueType& Emplace(KeyType Key, TArgs&&... Arguments);
	
	template<typename TPredicate>
	int32 RemoveAll(TPredicate&& Predicate);
	
	template<typename... TArgs>
	ValueType& FindOrAdd(const KeyType& Key, TArgs&&... Arguments);
	
	void Reset();
	
	void Reserve(const int32 NewCapacity);
	void Shrink();
	
	void Empty();
	void Swap(VMap& OtherMap) noexcept;
	
    ValueType& Add(KeyType&& Key, ValueType&& Value);
	ValueType& Add(const KeyType& Key, ValueType&& Value);
	ValueType& Add(const KeyType& Key, const ValueType& Value);
	bool TryAdd(const KeyType& Key, const ValueType& Value);
	
	bool Remove(const KeyType& Key);
    
	void GenerateKeyArray(VArray<KeyType>& OutKeys) const;
	void GenerateValueArray(VArray<ValueType>& OutValues) const;
    
	VArray<KeyType> GetKeys() const;
	VArray<ValueType> GetValues() const;
	
	Iterator Begin();
	Iterator begin();
	ConstIterator Begin() const;
	ConstIterator begin() const;
	
	Iterator End();
	Iterator end();
	ConstIterator End() const;
	ConstIterator end() const;
	
    ValueType* Find(const KeyType& Key);
    const ValueType* Find(const KeyType& Key) const;

    ValueType& FindChecked(const KeyType& Key);
    const ValueType& FindChecked(const KeyType& Key) const;
	
	ValueType& FindOrAdd(const KeyType& Key);

    bool TryGetValue(const KeyType& Key, ValueType*& OutValue);
    bool TryGetValue(const KeyType& Key, const ValueType*& OutValue) const;

    int32 Num() const;
    int32 Max() const;

    bool IsEmpty() const;
	bool Contains(const KeyType& Key) const;

	VMap& operator=(VMap&& OtherMap) noexcept;
	VMap& operator=(const VMap& OtherMap);
	
	ValueType& operator[](const KeyType& Key);
	const ValueType& operator[](const KeyType& Key) const;
	
private:
	struct VElement
	{
		VElement() = default;
    	
		template<typename TKey, typename TValue>
		VElement(TKey&& InKey, TValue&& InValue);
    	
		KeyType Key;
		ValueType Value;
		uint32 Hash = 0;
	};

private:
	void RebuildHashTable();
	
	int32 AddElement(VElement&& Element);
    int32 FindIndex(const KeyType& Key) const;
	
private:
	VSparseArray<VElement> Elements;
	VArray<int32> NextLinks;
	VHashTable HashTable;
};

template<typename KeyType, typename ValueType>
class VMap<KeyType, ValueType>::Iterator
{
public:
	Iterator(VMap* InOwner, const int32 InIndex);

	KeyType& Key() const;
	ValueType& Value() const;

	int32 GetIndex() const;
	
	VPair<KeyType&, ValueType&> operator*() const;
	Iterator& operator++();
	
	bool operator==(const Iterator& OtherIterator) const;
	bool operator!=(const Iterator& OtherIterator) const;

private:
	void Advance();

private:
	VMap* Owner;
	int32 Index;
};

template<typename KeyType, typename ValueType>
class VMap<KeyType, ValueType>::ConstIterator
{
public:
	ConstIterator(const VMap* InOwner, const int32 InIndex);
	
	const KeyType& Key() const;
	const ValueType& Value() const;

	int32 GetIndex() const;

	VPair<const KeyType&, const ValueType&> operator*() const;
	ConstIterator& operator++();

	bool operator==(const ConstIterator& OtherConstIterator) const;
	bool operator!=(const ConstIterator& OtherConstIterator) const;

private:
	void Advance();
	
private:
	const VMap* Owner;
	int32 Index;
};
}

#include "Core/Containers/Map.inl"