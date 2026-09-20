// Vertex

#pragma once

#include "Core/Containers/Array.h"
#include "Core/Containers/Hash.h"
#include "Core/Containers/HashTable.h"
#include "Core/Containers/SparseArray.h"

namespace VCore
{
template<typename ElementType>
class VSet
{
public:
	class Iterator;
	class ConstIterator;

public:
	VSet() = default;
	VSet(VSet&& OtherSet) noexcept;
	VSet(const VSet& OtherSet);
	explicit VSet(const int32 InitialCapacity);
	~VSet() = default;
	
	template<typename TFunction>
	void ForEach(TFunction&& Function);

	template<typename TFunction>
	void ForEach(TFunction&& Function) const;
	
	template<typename... TArgs>
	ElementType& Emplace(TArgs&&... Arguments);
	
	template<typename TPredicate>
	int32 RemoveAll(TPredicate&& Predicate);
	
	template<typename TPredicate>
	ElementType* FindByPredicate(TPredicate&& Predicate);
	
	template<typename TPredicate>
	const ElementType* FindByPredicate(TPredicate&& Predicate) const;
	
	void Reset();
	void Reserve(const int32 NewCapacity);
	void Empty();
	
	bool Add(const ElementType& Element);
	bool Add(ElementType&& Element);

	bool Remove(const ElementType& Element);

	Iterator Begin();
	Iterator begin();
	ConstIterator Begin() const;
	ConstIterator begin() const;
	
	Iterator End();
	Iterator end();
	ConstIterator End() const;
	ConstIterator end() const;
	
	int32 Num() const;
	int32 Max() const;
	
	bool IsEmpty() const;
	bool Contains(const ElementType& Element) const;
	
	VSet& operator=(const VSet& OtherSet);
	VSet& operator=(VSet&& OtherSet) noexcept;

private:
	struct VElement
	{
		VElement() = default;
		VElement(VElement&&) noexcept = default;
		VElement(const VElement&) = default;
		
		template<typename TValue> requires (!std::is_same_v<std::remove_cvref_t<TValue>, VElement>)
		explicit VElement(TValue&& InValue);

		VElement& operator=(const VElement&) = default;
		VElement& operator=(VElement&&) noexcept = default;

		ElementType Value;
		uint32 Hash = 0;
	};

private:
	int32 FindIndex(const ElementType& Element) const;
	int32 AddInternal(VElement&& Element);

private:
	VSparseArray<VElement> Elements;
	VArray<int32> NextLinks;
	VHashTable HashTable;
};

template<typename ElementType>
class VSet<ElementType>::Iterator
{
public:
    Iterator(VSet* InOwner, const int32 InIndex);
	int32 GetIndex() const;

    ElementType& operator*() const;
    ElementType* operator->() const;

    Iterator& operator++();
	Iterator& operator--();

    bool operator==(const Iterator& OtherIterator) const;
    bool operator!=(const Iterator& OtherIterator) const;

private:
	void Advance();
	void Retreat();
	
private:
    VSet* Owner;
    int32 Index;
};


template<typename ElementType>
class VSet<ElementType>::ConstIterator
{
public:
    ConstIterator(const VSet* InOwner, const int32 InIndex);
	int32 GetIndex() const;

    const ElementType& operator*() const;
    const ElementType* operator->() const;

    ConstIterator& operator++();
	ConstIterator& operator--();

    bool operator==(const ConstIterator& OtherConstIterator) const;
    bool operator!=(const ConstIterator& OtherConstIterator) const;

private:
	void Advance();
	void Retreat();
	
private:
    const VSet* Owner;
    int32 Index;
};
}

#include "Core/Containers/Set.inl"