// Vertex

#include "Core/Memory/ObjectManager.h"

namespace VCore
{
VObjectManager& VObjectManager::Get()
{
	static VObjectManager Instance;
	return Instance;
}

void VObjectManager::Initialize()
{
	if (bIsInitialized) return;
	bIsInitialized = true;
}

void VObjectManager::Shutdown()
{
	if (!bIsInitialized) return;
	CollectPendingKill();
	
	ObjectEntries.Empty();
	PendingKillObjects.Empty();
	bIsInitialized = false;
}

void VObjectManager::Tick()
{
	ProcessDeferredDestruction();
}

void VObjectManager::DestroyObject(const VObjectHandle& ObjectHandle)
{
	DestroyObject(FindObject(ObjectHandle));
}

void VObjectManager::DestroyObject(VObject* Object)
{
	if (!Object) return;
	MarkPendingKill(Object);
}

void VObjectManager::RegisterObject(const VObject* Object)
{
	if (!Object) return;

	// Creation already registers the object in the table.
	// This function exists so the object system can later
	// support externally-created objects.
}

void VObjectManager::UnregisterObject(const VObject* Object)
{
	if (!Object) return;
	
	const VObjectHandle Handle = FindHandle(Object);
	if (!Handle.IsValid()) return;

	VObjectEntry& ObjectEntry = ObjectEntries[Handle.Index];
	ObjectEntry.Object.Reset();
	ObjectEntry.Generation++;
}

void VObjectManager::MarkPendingKill(VObject* Object)
{
	if (!Object) return;
	const VObjectHandle Handle = FindHandle(Object);
	
	if (!Handle.IsValid()) return;
	VObjectEntry& ObjectEntry = ObjectEntries[Handle.Index];

	if (ObjectEntry.bIsPendingKill) return;
	ObjectEntry.bIsPendingKill = true;
	PendingKillObjects.Add(Object);
}

void VObjectManager::CollectPendingKill()
{
	for (const auto& PendingKillObject : PendingKillObjects)
	{
		if (!PendingKillObject) continue;
		const VObjectHandle Handle = FindHandle(PendingKillObject);

		if (!Handle.IsValid()) continue;

		VObjectEntry& ObjectEntry = ObjectEntries[Handle.Index];
		ObjectEntry.Object.Reset();
		ObjectEntry.bIsPendingKill = false;

		ObjectEntry.Generation++;
		if (ObjectEntry.Generation <= 0) ObjectEntry.Generation = 1;
	}

	PendingKillObjects.Empty();
}

void VObjectManager::ProcessDeferredDestruction()
{
	CollectPendingKill();
}

VObjectHandle VObjectManager::FindHandle(const VObject* RawPtr) const
{
	if (!RawPtr) return VObjectHandle();

	for (int32 Index = 0; Index < ObjectEntries.Num(); Index++)
	{
		const VObjectEntry& ObjectEntry = ObjectEntries[Index];

		if (ObjectEntry.Object && ObjectEntry.Object.Get() == RawPtr)
		{
			return VObjectHandle(Index, ObjectEntry.Generation);
		}
	}

	return VObjectHandle();
}

VObjectHandle VObjectManager::FindHandle(const VName& ObjectName) const
{
	for (int32 Index = 0; Index < ObjectEntries.Num(); Index++)
	{
		const VObjectEntry& ObjectEntry = ObjectEntries[Index];
		if (!ObjectEntry.Object) continue;
		if (ObjectEntry.Object->GetName() == ObjectName) return VObjectHandle(Index, ObjectEntry.Generation);
	}

	return VObjectHandle();
}

VObject* VObjectManager::FindObject(const VObjectHandle& ObjectHandle)
{
	if (!IsValid(ObjectHandle)) return nullptr;
	return ObjectEntries[ObjectHandle.Index].Object.Get();
}

VObject* VObjectManager::FindObject(const VName& ObjectName)
{
	for (int32 Index = 0; Index < ObjectEntries.Num(); Index++)
	{
		const VObjectEntry& ObjectEntry = ObjectEntries[Index];
		if (!ObjectEntry.Object) continue;
		if (ObjectEntry.bIsPendingKill) continue;

		if (ObjectEntry.Object->GetName() == ObjectName)
		{
			return ObjectEntry.Object.Get();
		}
	}

	return nullptr;
}

VObject* VObjectManager::FindObject(const VClass* Class, const VObject* Outer, const VName& Name)
{
	for (int32 Index = 0; Index < ObjectEntries.Num(); Index++)
	{
		VObjectEntry& ObjectEntry = ObjectEntries[Index];
		if (!ObjectEntry.Object) continue;
		if (ObjectEntry.bIsPendingKill) continue;
		VObject* Object = ObjectEntry.Object.Get();
		
		if (Class && !Object->IsA(Class)) continue;
		if (Object->GetOuter() != Outer) continue;
		if (Object->GetName() != Name) continue;
		return Object;
	}

	return nullptr;
}

VObject* VObjectManager::FindObjectByPath(const VString& Path)
{
	for (int32 Index = 0; Index < ObjectEntries.Num(); Index++)
	{
		VObjectEntry& ObjectEntry = ObjectEntries[Index];
		if (!ObjectEntry.Object) continue;
		if (ObjectEntry.bIsPendingKill) continue;

		VObject* Object = ObjectEntry.Object.Get();
		if (Object->GetPathName() == Path) return Object;
	}

	return nullptr;
}

const VObject* VObjectManager::FindObject(const VObjectHandle& ObjectHandle) const
{
	if (!IsValid(ObjectHandle)) return nullptr;
	return ObjectEntries[ObjectHandle.Index].Object.Get();
}

const VObject* VObjectManager::FindObject(const VName& ObjectName) const
{
	for (int32 Index = 0; Index < ObjectEntries.Num(); Index++)
	{
		const VObjectEntry& ObjectEntry = ObjectEntries[Index];
		if (!ObjectEntry.Object) continue;
		if (ObjectEntry.bIsPendingKill) continue;

		if (ObjectEntry.Object->GetName() == ObjectName)
		{
			return ObjectEntry.Object.Get();
		}
	}

	return nullptr;
}

VArray<VObject*> VObjectManager::GetObjects(const VClass* Class)
{
	VArray<VObject*> Result;
	if (!Class) return Result;

	for (int32 Index = 0; Index < ObjectEntries.Num(); Index++)
	{
		VObjectEntry& ObjectEntry = ObjectEntries[Index];
		if (!ObjectEntry.Object) continue;

		if (ObjectEntry.bIsPendingKill)continue;
		VObject* Object = ObjectEntry.Object.Get();

		if (Object->IsA(Class))
		{
			Result.Add(Object);
		}
	}

	return Result;
}

VArray<const VObject*> VObjectManager::GetObjects(const VClass* Class) const
{
	VArray<const VObject*> Result;
	if (!Class) return Result;

	for (int32 Index = 0; Index < ObjectEntries.Num(); Index++)
	{
		const VObjectEntry& ObjectEntry = ObjectEntries[Index];
		if (!ObjectEntry.Object) continue;
		
		if (ObjectEntry.bIsPendingKill) continue;
		const VObject* Object = ObjectEntry.Object.Get();

		if (Object->IsA(Class))
		{
			Result.Add(Object);
		}
	}

	return Result;
}

VClass* VObjectManager::GetObjectClass(const VObjectHandle& ObjectHandle)
{
	const VObject* Object = FindObject(ObjectHandle);
	if (!Object) return nullptr;
	return Object->GetClass();
}

VClass* VObjectManager::GetObjectClass(const VObject* Object)
{
	if (!Object) return nullptr;
	return Object->GetClass();
}

const VClass* VObjectManager::GetObjectClass(const VObjectHandle& ObjectHandle) const
{
	const VObject* Object = FindObject(ObjectHandle);
	if (!Object) return nullptr;
	return Object->GetClass();
}

const VClass* VObjectManager::GetObjectClass(const VObject* Object) const
{
	if (!Object) return nullptr;
	return Object->GetClass();
}

int32 VObjectManager::GetObjectCount() const
{
	int32 CountResult = 0;

	for (const auto& ObjectEntry : ObjectEntries)
	{
		if (ObjectEntry.Object && !ObjectEntry.bIsPendingKill)
		{
			CountResult++;
		}
	}

	return CountResult;
}

int32 VObjectManager::GetPendingKillCount() const
{
	return PendingKillObjects.Num();
}

bool VObjectManager::IsA(const VObjectHandle& ObjectHandle, const VClass* Class) const
{
	if (!Class) return false;

	const VObject* Object = FindObject(ObjectHandle);
	if (!Object) return false;

	return Object->IsA(Class);
}

bool VObjectManager::IsA(const VObject* Object, const VClass* Class) const
{
	if (!Object || !Class) return false;
	return Object->IsA(Class);
}

bool VObjectManager::IsValid(const VObjectHandle& ObjectHandle) const
{
	if (!ObjectHandle.IsValid()) return false;
	if (ObjectHandle.Index < 0) return false;
	if (ObjectHandle.Index >= ObjectEntries.Num()) return false;
	
	const VObjectEntry& ObjectEntry = ObjectEntries[ObjectHandle.Index];
	if (!ObjectEntry.Object) return false;
	if (ObjectEntry.bIsPendingKill) return false;
	return ObjectEntry.Generation == ObjectHandle.Generation;
}

VObjectManager& GetObjectManager()
{
	static VObjectManager ObjectManager;
	return ObjectManager;
}
}