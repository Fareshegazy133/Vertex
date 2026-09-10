// Vertex

#include <cassert>
#include <utility>

namespace VCore
{
template <typename KeyType, typename ValueType>
VHashNode<KeyType, ValueType>::VHashNode(const KeyType& Key, const ValueType& Value)
	: Pair(Key, Value)
{}

template <typename KeyType, typename ValueType>
VHashNode<KeyType, ValueType>::VHashNode(const KeyType& Key, ValueType&& Value)
	: Pair(Key, std::move(Value))
{}

template <typename KeyType, typename ValueType, typename Hasher>
VHashTable<KeyType, ValueType, Hasher>::VHashTable()
{
	RehashInternal(8);
}

template <typename KeyType, typename ValueType, typename Hasher>
VHashTable<KeyType, ValueType, Hasher>::VHashTable(const VHashTable& OtherHashTable)
{
	RehashInternal(OtherHashTable.BucketCount > 0 ? OtherHashTable.BucketCount : 8);
	
	for (int32 BucketIndex = 0; BucketIndex < OtherHashTable.BucketCount; BucketIndex++)
	{
		const VHashNode<KeyType, ValueType>* Node = OtherHashTable.Buckets[BucketIndex];
		
		while (Node)
		{
			Insert(Node->Pair.First, Node->Pair.Second);
			Node = Node->Next;
		}
	}
}

template <typename KeyType, typename ValueType, typename Hasher>
VHashTable<KeyType, ValueType, Hasher>::VHashTable(VHashTable&& OtherHashTable) noexcept
	: Buckets(OtherHashTable.Buckets), HashFunction(std::move(OtherHashTable.HashFunction)), BucketCount(OtherHashTable.BucketCount), ElementCount(OtherHashTable.ElementCount)
{
	OtherHashTable.Buckets = nullptr;
	OtherHashTable.BucketCount = 0;
	OtherHashTable.ElementCount = 0;
}

template <typename KeyType, typename ValueType, typename Hasher>
VHashTable<KeyType, ValueType, Hasher>::~VHashTable()
{
	DestroyNodes();
	delete[] Buckets;
	
	Buckets = nullptr;
	BucketCount = 0;
	ElementCount = 0;
}
	
template <typename KeyType, typename ValueType, typename Hasher>
void VHashTable<KeyType, ValueType, Hasher>::Reserve(const int32 NewCapacity)
{
	if (NewCapacity <= 0) return;
	
	const int32 RequiredBuckets = static_cast<int32>(static_cast<float>(NewCapacity) / 0.75f) + 1;
	
	if (RequiredBuckets > BucketCount)
	{
		RehashInternal(RequiredBuckets);
	}
}
	
template <typename KeyType, typename ValueType, typename Hasher>
void VHashTable<KeyType, ValueType, Hasher>::Rehash(const int32 NewBucketCount)
{
	if (NewBucketCount <= 0) return;
	if (NewBucketCount == BucketCount) return;
	
	RehashInternal(NewBucketCount);
}

template <typename KeyType, typename ValueType, typename Hasher>
void VHashTable<KeyType, ValueType, Hasher>::Shrink()
{
	const int32 DesiredBuckets = ElementCount == 0 ? 8 : static_cast<int32>(static_cast<float>(ElementCount) / 0.75f) + 1;
	
	if (DesiredBuckets < BucketCount)
	{
		RehashInternal(DesiredBuckets);
	}
}
	
template <typename KeyType, typename ValueType, typename Hasher>
bool VHashTable<KeyType, ValueType, Hasher>::Remove(const KeyType& Key)
{
	if (BucketCount == 0) return false;
	const int32 BucketIndex = GetBucketIndex(Key);
	
	VHashNode<KeyType, ValueType>* Node = Buckets[BucketIndex];
	VHashNode<KeyType, ValueType>* Previous = nullptr;
	
	while (Node)
	{
		if (Node->Pair.First == Key)
		{
			if (Previous)
			{
				Previous->Next = Node->Next;
			}
			else
			{
				Buckets[BucketIndex] = Node->Next;
			}
			
			delete Node;
			--ElementCount;
			return true;
		}
		
		Previous = Node;
		Node = Node->Next;
	}
	
	return false;
}
	
template <typename KeyType, typename ValueType, typename Hasher>
void VHashTable<KeyType, ValueType, Hasher>::Empty()
{
	DestroyNodes();
}
	
template <typename KeyType, typename ValueType, typename Hasher>
ValueType&
VHashTable<KeyType, ValueType, Hasher>::Insert(const KeyType& Key, const ValueType& Value)
{
	if (VHashNode<KeyType, ValueType>* Existing = FindNode(Key))
	{
		Existing->Pair.Second = Value;
		return Existing->Pair.Second;
	}
	
	if (BucketCount == 0 || static_cast<float>(ElementCount + 1) / static_cast<float>(BucketCount) > 0.75f)
	{
		RehashInternal(BucketCount == 0 ? 8 : BucketCount * 2);
	}
	
	const int32 BucketIndex = GetBucketIndex(Key);
	VHashNode<KeyType, ValueType>* NewNode = CreateNode(Key, Value);
	
	NewNode->Next = Buckets[BucketIndex];
	Buckets[BucketIndex] = NewNode;
	
	++ElementCount;
	return NewNode->Pair.Second;
}

template <typename KeyType, typename ValueType, typename Hasher>
ValueType&
VHashTable<KeyType, ValueType, Hasher>::Insert(const KeyType& Key, ValueType&& Value)
{
	if (VHashNode<KeyType, ValueType>* Existing = FindNode(Key))
	{
		Existing->Pair.Second = std::move(Value);
		return Existing->Pair.Second;
	}
	
	if (BucketCount == 0 || static_cast<float>(ElementCount + 1) / static_cast<float>(BucketCount) > 0.75f)
	{
		RehashInternal(BucketCount == 0 ? 8 : BucketCount * 2);
	}
	
	const int32 BucketIndex = GetBucketIndex(Key);
	VHashNode<KeyType, ValueType>* NewNode = CreateNode(Key, std::move(Value));
	
	NewNode->Next = Buckets[BucketIndex];
	Buckets[BucketIndex] = NewNode;
	
	++ElementCount;
	return NewNode->Pair.Second;
}
	
template <typename KeyType, typename ValueType, typename Hasher>
ValueType*
VHashTable<KeyType, ValueType, Hasher>::Find(const KeyType& Key)
{
	VHashNode<KeyType, ValueType>* Node = FindNode(Key);
	return Node ? &Node->Pair.Second : nullptr;
}

template <typename KeyType, typename ValueType, typename Hasher>
const ValueType*
VHashTable<KeyType, ValueType, Hasher>::Find(const KeyType& Key) const
{
	const VHashNode<KeyType, ValueType>* Node = FindNode(Key);
	return Node ? &Node->Pair.Second : nullptr;
}
	
template <typename KeyType, typename ValueType, typename Hasher>
float32 VHashTable<KeyType, ValueType, Hasher>::LoadFactor() const
{
	if (BucketCount == 0) return 0.f;
	return static_cast<float>(ElementCount) / static_cast<float>(BucketCount);
}
	
template <typename KeyType, typename ValueType, typename Hasher>
int32 VHashTable<KeyType, ValueType, Hasher>::Num() const
{
	return ElementCount;
}

template <typename KeyType, typename ValueType, typename Hasher>
int32 VHashTable<KeyType, ValueType, Hasher>::Max() const
{
	return BucketCount;
}
	
template <typename KeyType, typename ValueType, typename Hasher>
bool VHashTable<KeyType, ValueType, Hasher>::Contains(const KeyType& Key) const
{
	return FindNode(Key);
}

template <typename KeyType, typename ValueType, typename Hasher>
bool VHashTable<KeyType, ValueType, Hasher>::IsEmpty() const
{
	return ElementCount == 0;
}
	
template <typename KeyType, typename ValueType, typename Hasher>
VHashNode<KeyType, ValueType>*
VHashTable<KeyType, ValueType, Hasher>::FindNode(const KeyType& Key)
{
	if (BucketCount == 0) return nullptr;
	
	const int32 BucketIndex = GetBucketIndex(Key);
	VHashNode<KeyType, ValueType>* Node = Buckets[BucketIndex];
	
	while (Node)
	{
		if (Node->Pair.First == Key) return Node;
		Node = Node->Next;
	}
	
	return nullptr;
}

template <typename KeyType, typename ValueType, typename Hasher>
const VHashNode<KeyType, ValueType>*
VHashTable<KeyType, ValueType, Hasher>::FindNode(const KeyType& Key) const
{
	if (BucketCount == 0) return nullptr;
	
	const int32 BucketIndex = GetBucketIndex(Key);
	const VHashNode<KeyType, ValueType>* Node = Buckets[BucketIndex];
	
	while (Node)
	{
		if (Node->Pair.First == Key) return Node;
		Node = Node->Next;
	}
	
	return nullptr;
}
	
template <typename KeyType, typename ValueType, typename Hasher>
VHashTable<KeyType, ValueType, Hasher>&
VHashTable<KeyType, ValueType, Hasher>::operator=(const VHashTable& OtherHashTable)
{
	if (this == &OtherHashTable) return *this;
	
	Empty();
	RehashInternal(OtherHashTable.BucketCount > 0 ? OtherHashTable.BucketCount : 8);
	
	for (int32 BucketIndex = 0; BucketIndex < OtherHashTable.BucketCount; BucketIndex++)
	{
		const VHashNode<KeyType, ValueType>* Node = OtherHashTable.Buckets[BucketIndex];
		
		while (Node)
		{
			Insert(Node->Pair.First, Node->Pair.Second);
			Node = Node->Next;
		}
	}
	
	return *this;
}

template <typename KeyType, typename ValueType, typename Hasher>
VHashTable<KeyType, ValueType, Hasher>&
VHashTable<KeyType, ValueType, Hasher>::operator=(VHashTable&& OtherHashTable) noexcept
{
	if (this == &OtherHashTable) return *this;
	
	DestroyNodes();
	delete[] Buckets;
	
	Buckets = OtherHashTable.Buckets;
	BucketCount = OtherHashTable.BucketCount;
	ElementCount = OtherHashTable.ElementCount;
	HashFunction = std::move(OtherHashTable.HashFunction);
	
	OtherHashTable.Buckets = nullptr;
	OtherHashTable.BucketCount = 0;
	OtherHashTable.ElementCount = 0;
	return *this;
}
	
template <typename KeyType, typename ValueType, typename Hasher>
VHashNode<KeyType, ValueType>*
VHashTable<KeyType, ValueType, Hasher>::CreateNode(const KeyType& Key, const ValueType& Value)
{
	return new VHashNode<KeyType, ValueType>(Key, Value);
}

template <typename KeyType, typename ValueType, typename Hasher>
VHashNode<KeyType, ValueType>*
VHashTable<KeyType, ValueType, Hasher>::CreateNode(const KeyType& Key, ValueType&& Value)
{
	return new VHashNode<KeyType, ValueType>(Key, std::move(Value));
}

template <typename KeyType, typename ValueType, typename Hasher>
void VHashTable<KeyType, ValueType, Hasher>::DestroyNodes()
{
	if (!Buckets) return;
	
	for (int32 BucketIndex = 0; BucketIndex < BucketCount; BucketIndex++)
	{
		VHashNode<KeyType, ValueType>* Node = Buckets[BucketIndex];
		
		while (Node)
		{
			VHashNode<KeyType, ValueType>* Next = Node->Next;
			delete Node;
			Node = Next;
		}
		
		Buckets[BucketIndex] = nullptr;
	}
	
	ElementCount = 0;
}
	
template <typename KeyType, typename ValueType, typename Hasher>
void VHashTable<KeyType, ValueType, Hasher>::RehashInternal(const int32 NewBucketCount)
{
	if (NewBucketCount <= 0) return;
	
	VHashNode<KeyType, ValueType>** NewBuckets = new VHashNode<KeyType, ValueType>*[NewBucketCount]();
	
	for (int32 BucketIndex = 0; BucketIndex < BucketCount; BucketIndex++)
	{
		VHashNode<KeyType, ValueType>* Node = Buckets[BucketIndex];
		
		while (Node)
		{
			VHashNode<KeyType, ValueType>* Next = Node->Next;
			const int32 NewBucketIndex = static_cast<int32>(HashFunction(Node->Pair.First) % static_cast<VHashType>(NewBucketCount));
			
			Node->Next = NewBuckets[NewBucketIndex];
			NewBuckets[NewBucketIndex] = Node;
			Node = Next;
		}
	}
	
	delete[] Buckets;
	Buckets = NewBuckets;
	BucketCount = NewBucketCount;
}
	
template <typename KeyType, typename ValueType, typename Hasher>
int32 VHashTable<KeyType, ValueType, Hasher>::GetBucketIndex(const KeyType& Key) const
{
	assert(BucketCount > 0);
	return static_cast<int32>(HashFunction(Key) % static_cast<VHashType>(BucketCount));
}
}