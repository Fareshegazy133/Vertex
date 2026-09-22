// Vertex

#include "Core/Object/ObjectBaseUtility.h"

#include "Core/Memory/ObjectPtr.h"
#include "Core/Object/Object.h"

namespace VCore
{
void VObjectBaseUtility::Rename(VObject* Object, const VName& NewName)
{
	if (!Object) return;
	Object->SetName(NewName);
}

void VObjectBaseUtility::DestroyObject(VObject* Object)
{
	VObjectManager::Get().DestroyObject(Object);
}

void VObjectBaseUtility::SetOuter(VObject* Object, VObject* NewOuter)
{
	if (!Object) return;
	Object->SetOuter(NewOuter);
}

VObject* VObjectBaseUtility::FindObject(const VClass* Class, const VObject* Outer, const VName& Name)
{
	return VObjectManager::Get().FindObject(Class, Outer, Name);
}

VObject* VObjectBaseUtility::FindObjectByName(const VName& Name)
{
	return VObjectManager::Get().FindObject(Name);
}

VObject* VObjectBaseUtility::FindObjectByPath(const VString& Path)
{
	return VObjectManager::Get().FindObjectByPath(Path);
}

bool VObjectBaseUtility::IsValid(const VObject* Object)
{
	return Object && !Object->IsPendingKill();
}
}