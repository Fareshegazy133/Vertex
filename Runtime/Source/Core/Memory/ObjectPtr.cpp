// Vertex

#include "Core/Memory/ObjectPtr.h"

namespace VCore
{
VObjectHandle::VObjectHandle(const int32 Index, const int32 Generation) : Index(Index), Generation(Generation)
{}

bool VObjectHandle::IsValid() const
{
	return Generation != 0;
}

bool VObjectHandle::operator==(const VObjectHandle& OtherObjectHandle) const
{
	return Index == OtherObjectHandle.Index && Generation == OtherObjectHandle.Generation;
}

bool VObjectHandle::operator!=(const VObjectHandle& OtherObjectHandle) const
{
	return !(*this == OtherObjectHandle);
}

void VObjectManager::DestroyObject(const VObjectHandle& ObjectHandle)
{
	if (!IsValid(ObjectHandle)) return;

	ObjectEntry& Entry = Objects[ObjectHandle.Index];
	Entry.Object.Reset();

	Entry.Generation++;
}

VObjectHandle VObjectManager::FindHandle(const VObject* RawPtr) const
{
	if (!RawPtr) return VObjectHandle();

	for (int32 Index = 0; Index < Objects.Num(); Index++)
	{
		const ObjectEntry& Entry = Objects[Index];

		if (Entry.Object.Get() == RawPtr)
		{
			return VObjectHandle(Index, Entry.Generation);
		}
	}

	return VObjectHandle();
}

bool VObjectManager::IsValid(const VObjectHandle& ObjectHandle) const
{
	if (!ObjectHandle.IsValid()) return false;
	if (ObjectHandle.Index < 0) return false;
	if (static_cast<size_t>(ObjectHandle.Index) >= Objects.Num()) return false;

	const ObjectEntry& Entry = Objects[ObjectHandle.Index];
	if (!Entry.Object) return false;
	return Entry.Generation == ObjectHandle.Generation;
}

VObjectManager& GetObjectManager()
{
	static VObjectManager ObjectManager;
	return ObjectManager;
}
}