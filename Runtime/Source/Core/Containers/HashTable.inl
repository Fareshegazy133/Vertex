// Vertex

#pragma once

namespace VCore
{
inline VHashTable::VHashTable()
{
	InitializeBuckets(8);
}

inline VHashTable::VHashTable(const int32 InitialBucketCount)
{
	InitializeBuckets(InitialBucketCount > 0 ? InitialBucketCount : 8);
}

template <typename TPredicate>
int32 VHashTable::Find(const VArray<int32>& NextLinks, TPredicate&& Predicate, const uint32 Hash) const
{
	const int32 BucketIndex = GetBucketIndex(Hash);
	int32 Current = Buckets[BucketIndex];

	while (Current != INDEX_NONE)
	{
		if (Predicate(Current)) return Current;
		Current = NextLinks[Current];
	}

	return INDEX_NONE;
}

inline void VHashTable::Reserve(const int32 ElementCount)
{
	if (ElementCount <= 0) return;
	const int32 DesiredBuckets = ElementCount * 2;

	if (DesiredBuckets > Buckets.Num())
	{
		Rehash(DesiredBuckets);
	}
}

inline void VHashTable::Rehash(const int32 BucketCount)
{
	InitializeBuckets(BucketCount > 0 ? BucketCount : 8);
}

inline void VHashTable::Reset()
{
	for (auto& Bucket : Buckets)
	{
		Bucket = INDEX_NONE;
	}
}

inline void VHashTable::Empty()
{
	Buckets.Empty();
}

inline void VHashTable::Insert(const VArray<int32>& NextLinks, const int32 ElementIndex, const uint32 Hash)
{
	const int32 BucketIndex = GetBucketIndex(Hash);
	const_cast<VArray<int32>&>(NextLinks)[ElementIndex] = Buckets[BucketIndex];
	Buckets[BucketIndex] = ElementIndex;
}

inline void VHashTable::Remove(VArray<int32>& NextLinks, const int32 ElementIndex, const uint32 Hash)
{
	const int32 BucketIndex = GetBucketIndex(Hash);
	int32 Current = Buckets[BucketIndex];
	int32 Previous = INDEX_NONE;

	while (Current != INDEX_NONE)
	{
		if (Current == ElementIndex)
		{
			if (Previous == INDEX_NONE)
			{
				Buckets[BucketIndex] = NextLinks[Current];
			}
			else
			{
				NextLinks[Previous] = NextLinks[Current];
			}

			NextLinks[Current] = INDEX_NONE;
			return;
		}

		Previous = Current;
		Current = NextLinks[Current];
	}
}

inline int32 VHashTable::GetBucketCount() const
{
	return Buckets.Num();
}

inline void VHashTable::InitializeBuckets(const int32 Count)
{
	Buckets.Empty();
	Buckets.AddDefaulted(Count);

	for (auto& Bucket : Buckets)
	{
		Bucket = INDEX_NONE;
	}
}

inline int32 VHashTable::GetBucketIndex(const uint32 Hash) const
{
	return static_cast<int32>(Hash % static_cast<uint32>(Buckets.Num()));
}
}