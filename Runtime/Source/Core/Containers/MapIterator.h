// Vertex

#pragma once

#include "Core/Containers/HashTable.h"
#include "Core/Types/Numeric.h"

namespace VCore
{
template <typename KeyType, typename ValueType, typename Hasher>
class VMapIterator
{
public:
	using NodeType = const VHashNode<KeyType, ValueType>;
	using PairType = const VPair<const KeyType, ValueType>;

	VMapIterator() = default;
	VMapIterator(VHashTable<KeyType, ValueType, Hasher>* InHashTable, NodeType* InNode, const int32 InBucketIndex);

	PairType& operator*() const;
	PairType* operator->() const;

	VMapIterator& operator++();

	bool operator==(const VMapIterator& OtherMapIterator) const;
	bool operator!=(const VMapIterator& OtherMapIterator) const;

private:
	VHashTable<KeyType, ValueType, Hasher>* HashTable = nullptr;
	NodeType* Node = nullptr;
	int32 BucketIndex = 0;
};

template <typename KeyType, typename ValueType, typename Hasher>
class VMapConstIterator
{
public:
	using NodeType = const VHashNode<KeyType, ValueType>;
	using PairType = const VPair<const KeyType, ValueType>;

	VMapConstIterator() = default;
	VMapConstIterator(const VHashTable<KeyType, ValueType, Hasher>* InHashTable, NodeType* InNode, const int32 InBucketIndex);

	PairType& operator*() const;
	PairType* operator->() const;

	VMapConstIterator& operator++();

	bool operator==(const VMapConstIterator& OtherMapIterator) const;
	bool operator!=(const VMapConstIterator& OtherMapIterator) const;

private:
	const VHashTable<KeyType, ValueType, Hasher>* HashTable = nullptr;
	NodeType* Node = nullptr;
	int32 BucketIndex = 0;
};
}

#include "Core/Containers/MapIterator.inl"