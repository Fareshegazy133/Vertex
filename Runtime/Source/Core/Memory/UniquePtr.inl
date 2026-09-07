// Vertex

#pragma once

namespace VCore
{
template <typename ElementType, typename... Args>
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
template <typename OtherElementType> requires std::is_convertible_v<OtherElementType*, ElementType*>
VUniquePtr<ElementType>::VUniquePtr(VUniquePtr<OtherElementType>&& OtherUniquePtr) noexcept
	: RawPtr(OtherUniquePtr.Release())
{}

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