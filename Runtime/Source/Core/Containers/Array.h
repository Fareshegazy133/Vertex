// Vertex

#pragma once

#include <initializer_list>
#include "Core/Types/Numeric.h"

namespace VCore
{
template <typename ElementType>
class VArray
{
public:
	using ValueType = ElementType;
	using Iterator = ElementType*;
	using ConstIterator = const ElementType*;
	
public:
	VArray();
	VArray(VArray&& OtherArray) noexcept;
	VArray(const VArray& OtherArray);
	explicit VArray(const int32 InitialCapacity);
	~VArray();
	
	template <typename TPredicate>
	void Sort(TPredicate&& Predicate);

	template <typename TPredicate>
	void StableSort(TPredicate&& Predicate);
	
	template <typename TFunction>
	void ForEach(TFunction&& Function);
	
	template <typename TFunction>
	void ForEach(TFunction&& Function) const;
	
	template <typename... TArgs>
	ElementType& Emplace(TArgs&&... Arguments);
	
	template <typename... TArgs>
	ElementType& EmplaceAt(const int32 ArrayIndex, TArgs&&... Arguments);
	
	template <typename TPredicate>
	int32 RemoveAll(TPredicate&& Predicate);
	
	template <typename TPredicate>
	ElementType* FindByPredicate(TPredicate&& Predicate);
	
	template <typename TPredicate>
	const ElementType* FindByPredicate(TPredicate&& Predicate) const;
	
	template <typename TPredicate>
	int32 IndexOfByPredicate(TPredicate&& Predicate) const;

	void Init(const ElementType& Element, const int32 NewCapacity);
	void Reset();
	
	void Reserve(const int32 NewCapacity);
	void Shrink();
	
	void Empty();
	void Swap(VArray& OtherArray);
	
	void Add(ElementType&& Element);
	void Add(const ElementType& Element);
	
	void AddDefaulted(const int32 Count);
	ElementType& AddDefaulted();
	
	void Append(VArray&& OtherArray);
	void Append(const VArray& OtherArray);
	void Append(const ElementType* Elements, const int32 Count);
	void Append(std::initializer_list<ElementType> Elements);
	
	void Insert(ElementType&& Element, const int32 ArrayIndex);
	void Insert(const ElementType& Element, const int32 ArrayIndex);
	
	bool Remove(const ElementType& Element);
	bool RemoveSingle(const ElementType& Element);
	void RemoveAt(const int32 ArrayIndex);
	void RemoveAtSwap(const int32 ArrayIndex);
		
	Iterator Begin();
	Iterator begin();
	ConstIterator Begin() const;
	ConstIterator begin() const;
	
	Iterator End();
	Iterator end();
	ConstIterator End() const;
	ConstIterator end() const;

	ElementType* GetData();
	const ElementType* GetData() const;
	
	int32 Num() const;
	int32 Max() const;
	int32 Find(const ElementType& Element) const;
	int32 FindLast(const ElementType& Element) const;

	bool IsEmpty() const;
	bool Contains(const ElementType& Element) const;
	bool IsValidIndex(const int32 ArrayIndex) const;
	
	VArray& operator=(VArray&& OtherArray) noexcept;
	VArray& operator=(const VArray& OtherArray);
	
	bool operator==(const VArray& OtherArray) const;
	bool operator!=(const VArray& OtherArray) const;
	
	ElementType& operator[](const int32 ArrayIndex);
	const ElementType& operator[](const int32 ArrayIndex) const;

private:
	void GrowFor(const int32 RequiredNumber);
	void Relocate(const int32 NewCapacity);
	
private:
	ElementType* Data;
	int32 Indices = 0;
	int32 Capacity = 0;
};
}

#include "Core/Containers/Array.inl"