// Vertex

namespace VCore
{
template <typename KeyType, typename ValueType, typename Hasher>
VMapIterator<KeyType, ValueType, Hasher>::VMapIterator(VHashTable<KeyType, ValueType, Hasher>* InHashTable, NodeType* InNode, const int32 InBucketIndex)
	: HashTable(InHashTable), Node(InNode), BucketIndex(InBucketIndex)
{}

template <typename KeyType, typename ValueType, typename Hasher>
const VPair<const KeyType, ValueType>& VMapIterator<KeyType, ValueType, Hasher>::operator*() const
{
	return Node->Pair;
}

template <typename KeyType, typename ValueType, typename Hasher>
const VPair<const KeyType, ValueType>* VMapIterator<KeyType, ValueType, Hasher>::operator->() const
{
	return &Node->Pair;
}

template <typename KeyType, typename ValueType, typename Hasher>
VMapIterator<KeyType, ValueType, Hasher>& VMapIterator<KeyType, ValueType, Hasher>::operator++()
{
	if (!HashTable || !Node) return *this;

	if (Node->Next)
	{
		Node = Node->Next;
		return *this;
	}

	BucketIndex++;

	while (BucketIndex < HashTable->BucketCount)
	{
		Node = HashTable->Buckets[BucketIndex];
		if (Node) return *this;

		BucketIndex++;
	}

	Node = nullptr;
	return *this;
}

template <typename KeyType, typename ValueType, typename Hasher>
bool VMapIterator<KeyType, ValueType, Hasher>::operator==(const VMapIterator& OtherMapIterator) const
{
	return HashTable == OtherMapIterator.HashTable && Node == OtherMapIterator.Node;
}

template <typename KeyType, typename ValueType, typename Hasher>
bool VMapIterator<KeyType, ValueType, Hasher>::operator!=(const VMapIterator& OtherMapIterator) const
{
	return !(*this == OtherMapIterator);
}

template <typename KeyType, typename ValueType, typename Hasher>
VMapConstIterator<KeyType, ValueType, Hasher>::VMapConstIterator(const VHashTable<KeyType, ValueType, Hasher>* InHashTable, NodeType* InNode, const int32 InBucketIndex)
	: HashTable(InHashTable), Node(InNode), BucketIndex(InBucketIndex)
{}

template <typename KeyType, typename ValueType, typename Hasher>
const VPair<const KeyType, ValueType>& VMapConstIterator<KeyType, ValueType, Hasher>::operator*() const
{
	return Node->Pair;
}

template <typename KeyType, typename ValueType, typename Hasher>
const VPair<const KeyType, ValueType>* VMapConstIterator<KeyType, ValueType, Hasher>::operator->() const
{
	return &Node->Pair;
}

template <typename KeyType, typename ValueType, typename Hasher>
VMapConstIterator<KeyType, ValueType, Hasher>& VMapConstIterator<KeyType, ValueType, Hasher>::operator++()
{
	if (!HashTable || !Node) return *this;

	if (Node->Next)
	{
		Node = Node->Next;
		return *this;
	}

	BucketIndex++;

	while (BucketIndex < HashTable->BucketCount)
	{
		Node = HashTable->Buckets[BucketIndex];
		if (Node) return *this;

		BucketIndex++;
	}

	Node = nullptr;
	return *this;
}

template <typename KeyType, typename ValueType, typename Hasher>
bool VMapConstIterator<KeyType, ValueType, Hasher>::operator==(const VMapConstIterator& OtherMapIterator) const
{
	return HashTable == OtherMapIterator.HashTable && Node == OtherMapIterator.Node;
}

template <typename KeyType, typename ValueType, typename Hasher>
bool VMapConstIterator<KeyType, ValueType, Hasher>::operator!=(const VMapConstIterator& OtherMapIterator) const
{
	return !(*this == OtherMapIterator);
}
}