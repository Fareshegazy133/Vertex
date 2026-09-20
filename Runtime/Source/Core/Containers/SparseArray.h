#pragma once

namespace VCore
{
template<typename ElementType>
class VSparseArray
{
public:
	class Iterator
	{
	public:
		Iterator(VSparseArray* InOwner, const int32 InIndex);
		int32 GetIndex() const;
		
		ElementType& operator*() const;
		ElementType* operator->() const;

		Iterator& operator++();
		Iterator& operator--();

		bool operator==(const Iterator& OtherIterator) const;
		bool operator!=(const Iterator& OtherIterator) const;

	private:
		void AdvanceToValid();
		void RetreatToValid();
		
	private:
		VSparseArray* Owner;
		int32 Index;
	};

	class ConstIterator
	{
	public:
		ConstIterator(const VSparseArray* InOwner, const int32 InIndex);
		int32 GetIndex() const;
		
		const ElementType& operator*() const;
		const ElementType* operator->() const;

		ConstIterator& operator++();
		ConstIterator& operator--();

		bool operator==(const ConstIterator& OtherConstIterator) const;
		bool operator!=(const ConstIterator& OtherConstIterator) const;

	private:
		void AdvanceToValid();
		void RetreatToValid();
		
	private:
		const VSparseArray* Owner;
		int32 Index;
	};
	
public:
    VSparseArray() = default;
    VSparseArray(VSparseArray&& OtherSparseArray) noexcept;
	VSparseArray(const VSparseArray& OtherSparseArray);
	explicit VSparseArray(const int32 InitialCapacity);
    ~VSparseArray();

	template<typename TFunction>
	void ForEach(TFunction&& Function);

	template<typename TFunction>
	void ForEach(TFunction&& Function) const;
	
    template<typename... TArgs>
    int32 Emplace(TArgs&&... Arguments);
	
	template<typename TPredicate>
	int32 IndexOfByPredicate(TPredicate&& Predicate) const;

	void Reserve(const int32 NewCapacity);
	void Reset();
	void Empty();
	
	int32 Add(ElementType&& Element);
    int32 Add(const ElementType& Element);

    void RemoveAt(const int32 ArrayIndex);
	
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
	bool IsValidIndex(const int32 ArrayIndex) const;
	
	VSparseArray& operator=(const VSparseArray& OtherSparseArray);
	VSparseArray& operator=(VSparseArray&& OtherSparseArray) noexcept;

	ElementType& operator[](const int32 ArrayIndex);
	const ElementType& operator[](const int32 ArrayIndex) const;
	
private:
	struct VSlot
	{
		union
		{
			ElementType Element;
			int32 NextFreeIndex;
		};
		
		VSlot();
		~VSlot();
		
		VSlot(const VSlot&) = delete;
		VSlot& operator=(const VSlot&) = delete;
		
		bool bIsAllocated = false;
	};
	
private:
    VSlot* Slots;
	int32 Indices = 0;
    int32 Capacity = 0;
    int32 FirstFreeIndex = INDEX_NONE;

private:
    void Grow();
    void DestroyAll();
};
}

#include "Core/Containers/SparseArray.inl"