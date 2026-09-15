// Vertex

#pragma once

#include "Core/Containers/Hash.h"
#include "Core/Containers/Pair.h"
#include "Core/Types/Numeric.h"

namespace VCore
{
template <typename KeyType, typename ValueType, typename Hasher>
class VHashTable;

template <typename KeyType, typename ValueType, typename Hasher>
class VMap;

template <typename KeyType, typename ValueType, typename Hasher>
class VMapIterator;

template <typename KeyType, typename ValueType, typename Hasher>
class VMapConstIterator;

template <typename KeyType, typename ValueType>
struct VHashNode
{
	VHashNode(const KeyType& Key, const ValueType& Value);
	VHashNode(const KeyType& Key, ValueType&& Value);
	
	~VHashNode() = default;
	
	VPair<const KeyType, ValueType> Pair;
	VHashNode* Next = nullptr;
};

template <typename KeyType, typename ValueType, typename Hasher = VHash<KeyType>>
class VHashTable
{
public:
	VHashTable();
	VHashTable(const VHashTable& OtherHashTable);
	VHashTable(VHashTable&& OtherHashTable) noexcept;
	
	~VHashTable();
	
	void Reserve(const int32 NewCapacity);
	void Rehash(const int32 NewBucketCount);
	void Shrink();
	
	bool Remove(const KeyType& Key);
	void Empty();
	
	ValueType& Insert(const KeyType& Key, const ValueType& Value);
	ValueType& Insert(const KeyType& Key, ValueType&& Value);
	
	ValueType* Find(const KeyType& Key);
	const ValueType* Find(const KeyType& Key) const;
	
	float32 LoadFactor() const;
	
	int32 Num() const;
	int32 Max() const;
	
	bool Contains(const KeyType& Key) const;
	bool IsEmpty() const;
	
	VHashTable& operator=(const VHashTable& OtherHashTable);
	VHashTable& operator=(VHashTable&& OtherHashTable) noexcept;
	
private:
	VHashNode<KeyType, ValueType>* CreateNode(const KeyType& Key, const ValueType& Value);
	VHashNode<KeyType, ValueType>* CreateNode(const KeyType& Key, ValueType&& Value);
	
	void DestroyNodes();
	
	void RehashInternal(const int32 NewBucketCount);
	int32 GetBucketIndex(const KeyType& Key) const;
	
	VHashNode<KeyType, ValueType>* FindNode(const KeyType& Key);
	const VHashNode<KeyType, ValueType>* FindNode(const KeyType& Key) const;
	
private:
	template <typename, typename, typename>
	friend class VMap;
	friend class VMapIterator<KeyType, ValueType, Hasher>;
	friend class VMapConstIterator<KeyType, ValueType, Hasher>;
	
	VHashNode<KeyType, ValueType>** Buckets = nullptr;
	Hasher HashFunction;
	
	int32 BucketCount = 0;
	int32 ElementCount = 0;
};
}

#include "Core/Containers/HashTable.inl"