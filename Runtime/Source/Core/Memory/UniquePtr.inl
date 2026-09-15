// Vertex

#pragma once

namespace VCore
{
template <typename T, typename... Args>
VUniquePtr<T> MakeUnique(Args&&... Arguments)
{
	return VUniquePtr(new T(std::forward<Args>(Arguments)...));
}

template <typename T>
VUniquePtr<T>::VUniquePtr(T* InRawPtr) noexcept
	: RawPtr(InRawPtr)
{}

template <typename T>
VUniquePtr<T>::~VUniquePtr()
{
	delete RawPtr;
}

template <typename T>
void VUniquePtr<T>::Reset(T* NewRawPtr)
{
	if (RawPtr != NewRawPtr)
	{
		delete RawPtr;
		RawPtr = NewRawPtr;
	}
}

template <typename T>
void VUniquePtr<T>::Swap(VUniquePtr& OtherUniquePtr)
{
	std::swap(RawPtr, OtherUniquePtr.RawPtr);
}

template <typename T>
T* VUniquePtr<T>::Release()
{
	T* ReleasedPtr = RawPtr;
	RawPtr = nullptr;
	return ReleasedPtr;
}

template <typename T>
T* VUniquePtr<T>::Get() const
{
	return RawPtr;
}

template <typename T>
bool VUniquePtr<T>::IsValid() const
{
	return RawPtr != nullptr;
}

template <typename T>
VUniquePtr<T>::VUniquePtr(VUniquePtr&& OtherUniquePtr) noexcept : RawPtr(OtherUniquePtr.RawPtr)
{
	OtherUniquePtr.RawPtr = nullptr;
}

template <typename T>
template <typename OtherT> requires std::is_convertible_v<OtherT*, T*>
VUniquePtr<T>::VUniquePtr(VUniquePtr<OtherT>&& OtherUniquePtr) noexcept
	: RawPtr(OtherUniquePtr.Release())
{}

template <typename T>
VUniquePtr<T>& VUniquePtr<T>::operator=(VUniquePtr&& OtherUniquePtr) noexcept
{
	if (this != &OtherUniquePtr)
	{
		delete RawPtr;
		RawPtr = OtherUniquePtr.RawPtr;
		OtherUniquePtr.RawPtr = nullptr;
	}

	return *this;
}

template <typename T>
T& VUniquePtr<T>::operator*() const
{
	return *RawPtr;
}

template <typename T>
T* VUniquePtr<T>::operator->() const
{
	return RawPtr;
}

template <typename T>
VUniquePtr<T>::operator T*() const
{
	return RawPtr;
}

template <typename T>
VUniquePtr<T>::operator bool() const
{
	return RawPtr != nullptr;
}
}