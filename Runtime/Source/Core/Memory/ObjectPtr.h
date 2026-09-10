// Vertex

#pragma once

#include <Core/Containers/Array.h>
#include "Core/Memory/UniquePtr.h"
#include "Core/Types/Numeric.h"
#include "Game/Object.h"

namespace VCore
{
template <typename T>
class VObjectPtr;
	
struct VObjectHandle
{
	VObjectHandle() = default;
	VObjectHandle(int32 Index, int32 Generation);

	bool IsValid() const;

	bool operator==(const VObjectHandle& OtherObjectHandle) const;
	bool operator!=(const VObjectHandle& OtherObjectHandle) const;

	int32 Index = -1;
	int32 Generation = 0;
};
	
class VObjectManager
{
public:
	VObjectManager() = default;
	~VObjectManager() = default;

	VObjectManager(const VObjectManager&) = delete;
	VObjectManager& operator=(const VObjectManager&) = delete;

	VObjectManager(VObjectManager&&) = delete;
	VObjectManager& operator=(VObjectManager&&) = delete;

	template<typename T, typename... Args>
	VObjectPtr<T> CreateObject(Args&&... Arguments);

	void DestroyObject(const VObjectHandle& ObjectHandle);
	
	VObjectHandle FindHandle(const VObject* RawPtr) const;

	template<typename T>
	T* GetObject(const VObjectHandle& ObjectHandle);

	template<typename T>
	const T* GetObject(const VObjectHandle& ObjectHandle) const;

	bool IsValid(const VObjectHandle& ObjectHandle) const;

private:
	struct ObjectEntry
	{
		VUniquePtr<VObject> Object;
		int32 Generation = 1;
	};

	VArray<ObjectEntry> Objects;
};

template <typename T, typename... Args>
VObjectPtr<T> VObjectManager::CreateObject(Args&&... Arguments)
{
	static_assert(std::is_base_of_v<VObject, T>, "T must derive from VObject.");

	ObjectEntry Entry;
	Entry.Object = MakeUnique<T>(std::forward<Args>(Arguments)...);

	const int32 ArrayIndex = Objects.Num();
	Objects.Add(std::move(Entry));

	const VObjectHandle ObjectHandle = VObjectHandle(ArrayIndex, Objects[ArrayIndex].Generation);
	return VObjectPtr<T>(this, ObjectHandle);
}

template <typename T>
T* VObjectManager::GetObject(const VObjectHandle& ObjectHandle)
{
	static_assert(std::is_base_of_v<VObject, T>, "T must derive from VObject.");

	if (!IsValid(ObjectHandle)) return nullptr;
	return static_cast<T*>(Objects[ObjectHandle.Index].Object.Get());
}

template <typename T>
const T* VObjectManager::GetObject(const VObjectHandle& ObjectHandle) const
{
	static_assert(std::is_base_of_v<VObject, T>, "T must derive from VObject.");

	if (!IsValid(ObjectHandle)) return nullptr;
	return static_cast<const T*>(Objects[ObjectHandle.Index].Object.Get());
}
	
template <typename T>
class VObjectPtr
{
public:
	VObjectPtr() = default;
	VObjectPtr(T* RawPtr);
	VObjectPtr(VObjectManager* ObjectManager, const VObjectHandle& ObjectHandle);
	
	template <typename... Args>
	VObjectPtr(Args&&... Arguments);
	
	template <typename... Args>
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
	VObjectManager* ObjectManager = nullptr;
	VObjectHandle ObjectHandle = VObjectHandle();
};
	
VObjectManager& GetObjectManager();
}

#include "Core/Memory/ObjectPtr.inl"