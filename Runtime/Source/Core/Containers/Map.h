// Vertex

#pragma once

#include "Array.h"
#include "Hash.h"
#include "HashTable.h"
#include "Core/Containers/MapIterator.h"

namespace VCore
{
template <typename KeyType, typename ValueType, typename Hasher = VHash<KeyType>>
class VMap
{
public:
	using VIterator = VMapIterator<KeyType, ValueType, Hasher>;
	using VConstIterator = VMapConstIterator<KeyType, ValueType, Hasher>;
	
	VMap() = default;
	VMap(const VMap& OtherMap);
	VMap(VMap&& OtherMap) noexcept;
	
	~VMap() = default;
	
	void Reserve(int32 NewCapacity);
	
	ValueType& Add(const KeyType& Key, const ValueType& Value);
	ValueType& Add(const KeyType& Key, ValueType&& Value);

	template <typename... Args>
	ValueType& Emplace(const KeyType& Key, Args&&... Arguments);

	bool Remove(const KeyType& Key);
	void Shrink();
	void Empty();
	
	template <typename TFunction>
	void ForEach(TFunction&& Function);

	template <typename TFunction>
	void ForEach(TFunction&& Function) const;
	
	VIterator Begin();
	VIterator begin();
	VConstIterator Begin() const;
	VConstIterator begin() const;
	
	VIterator End();
	VIterator end();
	VConstIterator End() const;
	VConstIterator end() const;
	
	VArray<KeyType> GetKeys() const;
	VArray<ValueType> GetValues() const;

	ValueType* Find(const KeyType& Key);
	const ValueType* Find(const KeyType& Key) const;

	ValueType& FindChecked(const KeyType& Key);
	const ValueType& FindChecked(const KeyType& Key) const;

	ValueType& FindOrAdd(const KeyType& Key);
	ValueType& FindOrAdd(const KeyType& Key,const ValueType& DefaultValue);
	
	int32 Num() const;
	int32 Max() const;

	float32 LoadFactor() const;
	
	bool IsEmpty() const;
	bool Contains(const KeyType& Key) const;

	ValueType& operator[](const KeyType& Key);
	const ValueType& operator[](const KeyType& Key) const;

	VMap& operator=(const VMap& OtherMap);
	VMap& operator=(VMap&& OtherMap) noexcept;

private:
	VHashTable<KeyType, ValueType, Hasher> HashTable;
};
}

#include "Core/Containers/Map.inl"