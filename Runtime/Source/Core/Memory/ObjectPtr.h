// Vertex

#pragma once

#include <cstddef>
#include "Core/Memory/ObjectHandle.h"

namespace VCore
{
class VObject;
class VObjectManager;

template<typename T>
class VObjectPtr
{
public:
	VObjectPtr() = default;
	VObjectPtr(T* RawPtr);
	VObjectPtr(VObjectManager* InObjectManager, const VObjectHandle& InObjectHandle);
	
	template<typename... Args>
	VObjectPtr(Args&&... Arguments);
	
	template<typename... Args>
	static VObjectPtr Create(Args&&... Arguments);
	
	void Reset();
	
	[[nodiscard]] VObjectHandle GetHandle() const;
	[[nodiscard]] T* Get() const;

    [[nodiscard]] bool IsValid() const;
	
	T* operator->() const;
	T& operator*() const;
	
	bool operator==(std::nullptr_t) const;
	bool operator==(const VObjectPtr& OtherObjectPtr) const;
	
	bool operator!=(std::nullptr_t) const;
	bool operator!=(const VObjectPtr& OtherObjectPtr) const;
	
	operator T* () const;
	operator bool() const;

private:
	VObjectManager* ObjectManager;
	VObjectHandle ObjectHandle;
};
}

#include "Core/Memory/ObjectPtr.inl"