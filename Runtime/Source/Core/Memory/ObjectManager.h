// Vertex

#pragma once

#include <type_traits>
#include <utility>
#include "Core/Containers/Array.h"
#include "Core/Memory/ObjectHandle.h"
#include "Core/Memory/UniquePtr.h"
#include "Core/Object/Object.h"

namespace VCore
{
template<typename T>
class VObjectPtr;
class VClass;

class VObjectManager
{
public:
	VObjectManager() = default;
	VObjectManager(const VObjectManager&) = delete;
	VObjectManager(VObjectManager&&) = delete;
	~VObjectManager() = default;

	template<typename T, typename... TArgs>
	VObjectPtr<T> CreateObject(TArgs&&... Arguments);
	
	template<typename T, typename... TArgs>
	VObjectPtr<T> CreateObject(VObject* Outer, const VName& Name, TArgs&&... Arguments);

	template<typename T>
	T* GetObject(const VObjectHandle& ObjectHandle);

	template<typename T>
	const T* GetObject(const VObjectHandle& ObjectHandle) const;
	
	static VObjectManager& Get();
	
	void Initialize();
	void Shutdown();
	void Tick();
	
	VObject* CreateObject(const VClass* Class, VObject* Outer, const VName& Name);
	
	void DestroyObject(const VObjectHandle& ObjectHandle);
	void DestroyObject(VObject* Object);
	
	void RegisterObject(const VObject* Object);
	void UnregisterObject(const VObject* Object);
	
	void MarkPendingKill(VObject* Object);
	void CollectPendingKill();
	void ProcessDeferredDestruction();

	VObjectHandle FindHandle(const VObject* RawPtr) const;
	VObjectHandle FindHandle(const VName& ObjectName) const;
	VObject* FindObject(const VObjectHandle& ObjectHandle);
	VObject* FindObject(const VName& ObjectName);
	VObject* FindObject(const VClass* Class, const VObject* Outer, const VName& Name);
	VObject* FindObjectByPath(const VString& Path);
	const VObject* FindObject(const VObjectHandle& ObjectHandle) const;
	const VObject* FindObject(const VName& ObjectName) const;
	
	VArray<VObject*> GetObjects(const VClass* Class);
	VArray<const VObject*> GetObjects(const VClass* Class) const;
	
	VClass* GetObjectClass(const VObjectHandle& ObjectHandle);
	VClass* GetObjectClass(const VObject* Object);
	const VClass* GetObjectClass(const VObjectHandle& ObjectHandle) const;
	const VClass* GetObjectClass(const VObject* Object) const;
	int32 GetObjectCount() const;
	int32 GetPendingKillCount() const;
	
	bool IsA(const VObjectHandle& ObjectHandle, const VClass* Class) const;
	bool IsA(const VObject* Object, const VClass* Class) const;
	bool IsValid(const VObjectHandle& ObjectHandle) const;
	
	VObjectManager& operator=(const VObjectManager&) = delete;
	VObjectManager& operator=(VObjectManager&&) = delete;

private:
	struct VObjectEntry
	{
		VUniquePtr<VObject> Object;
		int32 Generation = 1;
	};

private:
	VArray<VObjectEntry> ObjectEntries;
	VArray<VObject*> PendingKillObjects;

	bool bIsInitialized = false;
};

template <typename T, typename... TArgs>
VObjectPtr<T> VObjectManager::CreateObject(TArgs&&... Arguments)
{
	static_assert(std::is_base_of_v<VObject, T>, "T must derive from VObject.");

	VObjectEntry ObjectEntry;
	ObjectEntry.Object = MakeUnique<T>(std::forward<TArgs>(Arguments)...);

	const int32 ArrayIndex = ObjectEntries.Num();
	ObjectEntries.Add(std::move(ObjectEntry));

	const VObjectHandle ObjectHandle(ArrayIndex, ObjectEntries[ArrayIndex].Generation);
	return VObjectPtr<T>(this, ObjectHandle);
}

template <typename T, typename ... TArgs>
VObjectPtr<T> VObjectManager::CreateObject(VObject* Outer, const VName& Name, TArgs&&... Arguments)
{
	static_assert(std::is_base_of_v<VObject, T>, "T must derive from VObject.");

	VObjectEntry ObjectEntry;
	ObjectEntry.Object = MakeUnique<T>(std::forward<TArgs>(Arguments)...);

	T* Object = static_cast<T*>(ObjectEntry.Object.Get());
	Object->SetOuter(Outer);
	Object->SetName(Name);

	const int32 ArrayIndex = ObjectEntries.Num();
	ObjectEntries.Add(std::move(ObjectEntry));

	const VObjectHandle ObjectHandle(ArrayIndex, ObjectEntries[ArrayIndex].Generation);
	return VObjectPtr<T>(this, ObjectHandle);
}

template <typename T>
T* VObjectManager::GetObject(const VObjectHandle& ObjectHandle)
{
	static_assert(std::is_base_of_v<VObject, T>, "T must derive from VObject.");
	if (!IsValid(ObjectHandle)) return nullptr;
	return static_cast<T*>(ObjectEntries[ObjectHandle.Index].Object.Get());
}

template <typename T>
const T* VObjectManager::GetObject(const VObjectHandle& ObjectHandle) const
{
	static_assert(std::is_base_of_v<VObject, T>, "T must derive from VObject.");
	if (!IsValid(ObjectHandle))return nullptr;
	return static_cast<const T*>(ObjectEntries[ObjectHandle.Index].Object.Get());
}

template<typename T, typename... TArgs>
T* NewObject(VObject* Outer = nullptr, const VName& Name = NAME_None, TArgs&&... Arguments)
{
	static_assert(std::is_base_of_v<VObject, T>, "NewObject<T>: T must derive from VObject.");
	return VObjectManager::Get().CreateObject<T>(Outer, Name, std::forward<TArgs>(Arguments)...).Get();
}

VObjectManager& GetObjectManager();
VObject* NewObject(const VClass* Class, VObject* Outer = nullptr, const VName& Name = NAME_None);
}