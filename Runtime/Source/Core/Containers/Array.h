// Vertex

#pragma once

#include "Core/Types/Numeric.h"

namespace VCore
{
template <typename ElementType>
class VArray
{
public:
	VArray() = default;
	~VArray();
	
	VArray(const VArray& OtherArray);
	VArray(VArray&& OtherArray) noexcept;

	void Init(const ElementType& Element, int32 NewCapacity);
	void Reserve(const int32 NewCapacity);
	
	void Add(const ElementType& Element);
	void Add(ElementType&& Element);

	void RemoveAt(int32 ArrayIndex);
	void Empty();
	
	template <typename TFunction>
	void ForEach(TFunction&& Function);
	
	ElementType* Begin();
	ElementType* begin();
	const ElementType* Begin() const;
	const ElementType* begin() const;
	
	ElementType* End();
	ElementType* end();
	const ElementType* End() const;
	const ElementType* end() const;
	
	int32 Num() const;
	int32 Max() const;

	bool IsEmpty() const;
	bool IsValidIndex(const int32 ArrayIndex) const;
	
	VArray& operator=(const VArray& OtherArray);
	VArray& operator=(VArray&& OtherArray) noexcept;
	
	ElementType& operator[](int32 ArrayIndex);
	const ElementType& operator[](int32 ArrayIndex) const;

private:
	ElementType* Data = nullptr;
	int32 Indices = 0;
	int32 Capacity = 0;
};
}

#include "Core/Containers/Array.inl"