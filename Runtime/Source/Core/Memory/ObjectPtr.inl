// Vertex

#pragma once

#include "Core/Memory/ObjectManager.h"

namespace VCore
{
template<typename T>
VObjectPtr<T>::VObjectPtr(T* RawPtr)
{
	if (!RawPtr) return;
	
	VObjectManager* Manager = &GetObjectManager();
	
	const VObjectHandle Handle = Manager->FindHandle(RawPtr);
	
	if (!Manager->IsValid(Handle)) return;
	
	ObjectManager = Manager;
	ObjectHandle = Handle;
}

template<typename T>
VObjectPtr<T>::VObjectPtr(VObjectManager* InObjectManager, const VObjectHandle& InObjectHandle)
	: ObjectManager(InObjectManager), ObjectHandle(InObjectHandle)
{}

template<typename T> template<typename... Args>
VObjectPtr<T>::VObjectPtr(Args&&... Arguments)
{
	*this = Create(std::forward<Args>(Arguments)...);
}

template<typename T> template<typename... Args>
VObjectPtr<T> VObjectPtr<T>::Create(Args&&... Arguments)
{
	return GetObjectManager().CreateObject<T>(std::forward<Args>(Arguments)...);
}

template<typename T>
void VObjectPtr<T>::Reset()
{
	ObjectManager = nullptr;
	ObjectHandle = VObjectHandle();
}

template<typename T>
VObjectHandle VObjectPtr<T>::GetHandle() const
{
    return ObjectHandle;
}

template<typename T>
T* VObjectPtr<T>::Get() const
{
	if (!ObjectManager) return nullptr;
	return ObjectManager->GetObject<T>(ObjectHandle);
}

template<typename T>
bool VObjectPtr<T>::IsValid() const
{
	if (!ObjectManager) return false;
	return ObjectManager->IsValid(ObjectHandle);
}

template<typename T>
T* VObjectPtr<T>::operator->() const
{
	return Get();
}

template<typename T>
T& VObjectPtr<T>::operator*() const
{
	return *Get();
}

template<typename T>
bool VObjectPtr<T>::operator==(std::nullptr_t) const
{
	return !IsValid();
}

template<typename T>
bool VObjectPtr<T>::operator==(const VObjectPtr& OtherObjectPtr) const
{
	return ObjectManager == OtherObjectPtr.ObjectManager && ObjectHandle == OtherObjectPtr.ObjectHandle;
}
	
template<typename T>
bool VObjectPtr<T>::operator!=(std::nullptr_t) const
{
	return IsValid();
}

template<typename T>
bool VObjectPtr<T>::operator!=(const VObjectPtr& OtherObjectPtr) const
{
	return !(*this == OtherObjectPtr);
}
	
template<typename T>
VObjectPtr<T>::operator T*() const
{
	return Get();
}

template<typename T>
VObjectPtr<T>::operator bool() const
{
	return IsValid();
}
}
