// Vertex

namespace VCore
{
template <typename KeyType, typename ValueType, typename Hasher>
VMap<KeyType, ValueType, Hasher>::VMap(const VMap& OtherMap)
	: HashTable(OtherMap.HashTable)
{}

template <typename KeyType, typename ValueType, typename Hasher>
VMap<KeyType, ValueType, Hasher>::VMap(VMap&& OtherMap) noexcept
	: HashTable(std::move(OtherMap.HashTable))
{}

template <typename KeyType, typename ValueType, typename Hasher>
void VMap<KeyType, ValueType, Hasher>::Reserve(int32 NewCapacity)
{
	HashTable.Reserve(NewCapacity);
}

template <typename KeyType, typename ValueType, typename Hasher>
ValueType& VMap<KeyType, ValueType, Hasher>::Add(const KeyType& Key, const ValueType& Value)
{
	return HashTable.Insert(Key, Value);
}

template <typename KeyType, typename ValueType, typename Hasher>
ValueType& VMap<KeyType, ValueType, Hasher>::Add(const KeyType& Key, ValueType&& Value)
{
	return HashTable.Insert(Key, std::move(Value));
}

template <typename KeyType, typename ValueType, typename Hasher>
template <typename ... Args>
ValueType& VMap<KeyType, ValueType, Hasher>::Emplace(const KeyType& Key, Args&&... Arguments)
{
	if (ValueType* Existing = Find(Key)) return *Existing;

	ValueType Value(std::forward<Args>(Arguments)...);
	return HashTable.Insert(Key, std::move(Value));
}

template <typename KeyType, typename ValueType, typename Hasher>
bool VMap<KeyType, ValueType, Hasher>::Remove(const KeyType& Key)
{
	return HashTable.Remove(Key);
}

template <typename KeyType, typename ValueType, typename Hasher>
void VMap<KeyType, ValueType, Hasher>::Shrink()
{
	HashTable.Shrink();
}

template <typename KeyType, typename ValueType, typename Hasher>
void VMap<KeyType, ValueType, Hasher>::Empty()
{
	HashTable.Empty();
}

template <typename KeyType, typename ValueType, typename Hasher>
template <typename TFunction>
void VMap<KeyType, ValueType, Hasher>::ForEach(TFunction&& Function)
{
	for (VIterator Iterator = Begin(); Iterator != End(); ++Iterator)
	{
		Function(*Iterator);
	}
}

template <typename KeyType, typename ValueType, typename Hasher>
template <typename TFunction>
void VMap<KeyType, ValueType, Hasher>::ForEach(TFunction&& Function) const
{
	for (VConstIterator Iterator = Begin(); Iterator != End(); ++Iterator)
	{
		Function(*Iterator);
	}
}

template <typename KeyType, typename ValueType, typename Hasher>
VMap<KeyType, ValueType, Hasher>::VIterator VMap<KeyType, ValueType, Hasher>::Begin()
{
	for (int32 BucketIndex = 0; BucketIndex < HashTable.BucketCount; BucketIndex++)
	{
		if (HashTable.Buckets[BucketIndex])
		{
			return VIterator(&HashTable, HashTable.Buckets[BucketIndex], BucketIndex);
		}
	}

	return End();
}

template <typename KeyType, typename ValueType, typename Hasher>
VMap<KeyType, ValueType, Hasher>::VIterator VMap<KeyType, ValueType, Hasher>::begin()
{
	return Begin();
}

template <typename KeyType, typename ValueType, typename Hasher>
VMap<KeyType, ValueType, Hasher>::VConstIterator VMap<KeyType, ValueType, Hasher>::Begin() const
{
	for (int32 BucketIndex = 0; BucketIndex < HashTable.BucketCount; BucketIndex++)
	{
		if (HashTable.Buckets[BucketIndex])
		{
			return VConstIterator(&HashTable, HashTable.Buckets[BucketIndex], BucketIndex);
		}
	}

	return End();
}

template <typename KeyType, typename ValueType, typename Hasher>
VMap<KeyType, ValueType, Hasher>::VConstIterator VMap<KeyType, ValueType, Hasher>::begin() const
{
	return Begin();
}

template <typename KeyType, typename ValueType, typename Hasher>
VMap<KeyType, ValueType, Hasher>::VIterator VMap<KeyType, ValueType, Hasher>::End()
{
	return VIterator(&HashTable, nullptr, HashTable.BucketCount);
}

template <typename KeyType, typename ValueType, typename Hasher>
VMap<KeyType, ValueType, Hasher>::VIterator VMap<KeyType, ValueType, Hasher>::end()
{
	return End();
}

template <typename KeyType, typename ValueType, typename Hasher>
VMap<KeyType, ValueType, Hasher>::VConstIterator VMap<KeyType, ValueType, Hasher>::End() const
{
	return VConstIterator(&HashTable, nullptr, HashTable.BucketCount);
}

template <typename KeyType, typename ValueType, typename Hasher>
VMap<KeyType, ValueType, Hasher>::VConstIterator VMap<KeyType, ValueType, Hasher>::end() const
{
	return End();
}

template <typename KeyType, typename ValueType, typename Hasher>
VArray<KeyType> VMap<KeyType, ValueType, Hasher>::GetKeys() const
{
	VArray<KeyType> KeysResult;

	ForEach([&KeysResult](const VPair<const KeyType, ValueType>& Pair)
		{
			KeysResult.Add(Pair.First);
		});
	
	return KeysResult;
}

template <typename KeyType, typename ValueType, typename Hasher>
VArray<ValueType> VMap<KeyType, ValueType, Hasher>::GetValues() const
{
	VArray<ValueType> ValuesResult;

	ForEach([&ValuesResult](const VPair<const KeyType, ValueType>& Pair)
		{
			ValuesResult.Add(Pair.Second);
		});
	
	return ValuesResult;
}

template <typename KeyType, typename ValueType, typename Hasher>
ValueType* VMap<KeyType, ValueType, Hasher>::Find(const KeyType& Key)
{
	return HashTable.Find(Key);
}

template <typename KeyType, typename ValueType, typename Hasher>
const ValueType* VMap<KeyType, ValueType, Hasher>::Find(const KeyType& Key) const
{
	return HashTable.Find(Key);
}

template <typename KeyType, typename ValueType, typename Hasher>
ValueType& VMap<KeyType, ValueType, Hasher>::FindChecked(const KeyType& Key)
{
	ValueType* Value = Find(Key);
	assert(Value);
	return *Value;
}

template <typename KeyType, typename ValueType, typename Hasher>
const ValueType& VMap<KeyType, ValueType, Hasher>::FindChecked(const KeyType& Key) const
{
	const ValueType* Value = Find(Key);
	assert(Value);
	return *Value;
}

template <typename KeyType, typename ValueType, typename Hasher>
ValueType& VMap<KeyType, ValueType, Hasher>::FindOrAdd(const KeyType& Key)
{
	if (ValueType* Existing = Find(Key)) return *Existing;
	return Add(Key, ValueType());
}

template <typename KeyType, typename ValueType, typename Hasher>
ValueType& VMap<KeyType, ValueType, Hasher>::FindOrAdd(const KeyType& Key, const ValueType& DefaultValue)
{
	if (ValueType* Existing = Find(Key)) return *Existing;
	return Add(Key, DefaultValue);
}

template <typename KeyType, typename ValueType, typename Hasher>
int32 VMap<KeyType, ValueType, Hasher>::Num() const
{
	return HashTable.Num();
}

template <typename KeyType, typename ValueType, typename Hasher>
int32 VMap<KeyType, ValueType, Hasher>::Max() const
{
	return HashTable.Max();
}

template <typename KeyType, typename ValueType, typename Hasher>
float32 VMap<KeyType, ValueType, Hasher>::LoadFactor() const
{
	return HashTable.LoadFactor();
}

template <typename KeyType, typename ValueType, typename Hasher>
bool VMap<KeyType, ValueType, Hasher>::IsEmpty() const
{
	return HashTable.IsEmpty();
}

template <typename KeyType, typename ValueType, typename Hasher>
bool VMap<KeyType, ValueType, Hasher>::Contains(const KeyType& Key) const
{
	return HashTable.Contains(Key);
}

template <typename KeyType, typename ValueType, typename Hasher>
ValueType& VMap<KeyType, ValueType, Hasher>::operator[](const KeyType& Key)
{
	return FindOrAdd(Key);
}

template <typename KeyType, typename ValueType, typename Hasher>
const ValueType& VMap<KeyType, ValueType, Hasher>::operator[](const KeyType& Key) const
{
	return FindChecked(Key);
}

template <typename KeyType, typename ValueType, typename Hasher>
VMap<KeyType, ValueType, Hasher>& VMap<KeyType, ValueType, Hasher>::operator=(const VMap& OtherMap)
{
	if (this == &OtherMap) return *this;

	HashTable = OtherMap.HashTable;
	return *this;
}

template <typename KeyType, typename ValueType, typename Hasher>
VMap<KeyType, ValueType, Hasher>& VMap<KeyType, ValueType, Hasher>::operator=(VMap&& OtherMap) noexcept
{
	if (this == &OtherMap) return *this;

	HashTable = std::move(OtherMap.HashTable);
	return *this;
}
}