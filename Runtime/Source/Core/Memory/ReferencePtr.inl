// Vertex

#include <strstream>

namespace VCore
{
template <typename T>
VReferencePtr<T>::VReferencePtr(T* InRawPtr) noexcept
	: RawPtr(InRawPtr)
{}

template <typename T>
void VReferencePtr<T>::Reset(T* NewRawPtr)
{
	RawPtr = NewRawPtr;
}

template <typename T>
void VReferencePtr<T>::Swap(VReferencePtr& OtherReferencePtr) noexcept
{
	std::swap(RawPtr, OtherReferencePtr.RawPtr);
}

template <typename T>
constexpr T* VReferencePtr<T>::Get() const
{
	return RawPtr;
}

template <typename T>
constexpr T& VReferencePtr<T>::GetRef() const
{
	return *RawPtr;
}

template <typename T>
bool VReferencePtr<T>::IsValid() const
{
	return RawPtr != nullptr;
}

template <typename T>
constexpr T* VReferencePtr<T>::operator->() const
{
	return RawPtr;
}

template <typename T>
constexpr T& VReferencePtr<T>::operator*() const
{
	return *RawPtr;
}

template <typename T>
VReferencePtr<T>& VReferencePtr<T>::operator=(T* NewRawPtr) noexcept
{
	RawPtr = NewRawPtr;
	return *this;
}

template <typename T>
VReferencePtr<T>& VReferencePtr<T>::operator=(std::nullptr_t) noexcept
{
	RawPtr = nullptr;
	return *this;
}

template <typename T>
constexpr bool VReferencePtr<T>::operator==(const VReferencePtr& OtherReferencePtr) const noexcept
{
	return RawPtr == OtherReferencePtr.RawPtr;
}

template <typename T>
constexpr bool VReferencePtr<T>::operator==(T* OtherRawPtr) const noexcept
{
	return RawPtr == OtherRawPtr;
}

template <typename T>
constexpr bool VReferencePtr<T>::operator!=(const VReferencePtr& OtherReferencePtr) const noexcept
{
	return RawPtr != OtherReferencePtr.RawPtr;
}

template <typename T>
constexpr bool VReferencePtr<T>::operator!=(T* OtherRawPtr) const noexcept
{
	return RawPtr != OtherRawPtr;
}

template <typename T>
VReferencePtr<T>::operator T*() const
{
	return RawPtr;
}

template <typename T>
VReferencePtr<T>::operator bool() const
{
	return IsValid();
}
}