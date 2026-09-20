// Vertex

#pragma once

#include "Core/Containers/Array.h"

namespace VCore
{
class VHashTable
{
public:
	VHashTable();
	explicit VHashTable(const int32 InitialBucketCount);
	
	template<typename TPredicate>
	int32 Find(const VArray<int32>& NextLinks, TPredicate&& Predicate, const uint32 Hash) const;

	void Reserve(const int32 ElementCount);
	void Rehash(const int32 BucketCount);
	
	void Reset();
	void Empty();

	void Insert(const VArray<int32>& NextLinks, const int32 ElementIndex, const uint32 Hash);
	void Remove(VArray<int32>& NextLinks, const int32 ElementIndex, const uint32 Hash);

	int32 GetBucketCount() const;

private:
	void InitializeBuckets(const int32 Count);
	int32 GetBucketIndex(const uint32 Hash) const;

private:
	VArray<int32> Buckets;
};
}

#include "Core/Containers/HashTable.inl"