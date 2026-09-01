// Vertex

#pragma once

#include <utility>

namespace VCore
{
template<typename ElementType>
class VUniquePtr
{
public:
	VUniquePtr() noexcept = default;
	VUniquePtr(ElementType* RawPtr) noexcept;
	VUniquePtr(const VUniquePtr&) = delete;
	VUniquePtr(VUniquePtr&& OtherUniquePtr) noexcept;
	
	~VUniquePtr();
	
	void Reset(ElementType* NewRawPtr = nullptr);
	void Swap(VUniquePtr& OtherUniquePtr);
	
	ElementType* Release();

	ElementType* Get() const;
	bool IsValid() const;
	
	VUniquePtr& operator=(const VUniquePtr&) = delete;
	VUniquePtr& operator=(VUniquePtr&& OtherUniquePtr) noexcept;

	ElementType& operator*() const;
	ElementType* operator->() const;

	explicit operator bool() const;
	
private:
	ElementType* RawPtr = nullptr;
};

template<typename ElementType, typename... Args>
VUniquePtr<ElementType> MakeUnique(Args&&... Arguments)
{
	return VUniquePtr(new ElementType(std::forward<Args>(Arguments)...));
}

template <typename T>
VUniquePtr<T>::VUniquePtr(T* RawPtr) noexcept : RawPtr(RawPtr)
{}

template <typename ElementType>
VUniquePtr<ElementType>::~VUniquePtr()
{
	delete RawPtr;
}

template <typename ElementType>
void VUniquePtr<ElementType>::Reset(ElementType* NewRawPtr)
{
	if (RawPtr != NewRawPtr)
	{
		delete RawPtr;
		RawPtr = NewRawPtr;
	}
}

template <typename ElementType>
void VUniquePtr<ElementType>::Swap(VUniquePtr& OtherUniquePtr)
{
	std::swap(RawPtr, OtherUniquePtr.RawPtr);
}

template <typename ElementType>
ElementType* VUniquePtr<ElementType>::Release()
{
	ElementType* ReleasedPtr = RawPtr;
	RawPtr = nullptr;
	return ReleasedPtr;
}

template <typename ElementType>
ElementType* VUniquePtr<ElementType>::Get() const
{
	return RawPtr;
}

template <typename ElementType>
bool VUniquePtr<ElementType>::IsValid() const
{
	return RawPtr != nullptr;
}

template <typename ElementType>
VUniquePtr<ElementType>::VUniquePtr(VUniquePtr&& OtherUniquePtr) noexcept : RawPtr(OtherUniquePtr.RawPtr)
{
	OtherUniquePtr.RawPtr = nullptr;
}

template <typename ElementType>
VUniquePtr<ElementType>& VUniquePtr<ElementType>::operator=(VUniquePtr&& OtherUniquePtr) noexcept
{
	if (this != &OtherUniquePtr)
	{
		delete RawPtr;
		RawPtr = OtherUniquePtr.RawPtr;
		OtherUniquePtr.RawPtr = nullptr;
	}

	return *this;
}

template <typename ElementType>
ElementType& VUniquePtr<ElementType>::operator*() const
{
	return *RawPtr;
}

template <typename ElementType>
ElementType* VUniquePtr<ElementType>::operator->() const
{
	return RawPtr;
}

template <typename ElementType>
VUniquePtr<ElementType>::operator bool() const
{
	return RawPtr != nullptr;
}
}