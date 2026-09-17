// Vertex

#include "Core/Object/Object.h"

#include "Core/Reflection/Class.h"

namespace VCore
{
VObject::VObject()
	: Outer(nullptr), ObjectFlags(EVObjectFlags::None)
{}

VClass* VObject::GetClass() const
{
	return StaticClass();
}

VClass* VObject::StaticClass()
{
	static VClass Class(nullptr, "VObject", sizeof(VObject));
	return &Class;
}

void VObject::AddToRoot()
{
	AddFlags(EVObjectFlags::Root);
}

void VObject::RemoveFromRoot()
{
	RemoveFlags(EVObjectFlags::Root);
}

void VObject::AddFlags(const EVObjectFlags InObjectFlags)
{
	ObjectFlags |= InObjectFlags;
}

void VObject::RemoveFlags(EVObjectFlags InObjectFlags)
{
	ObjectFlags = static_cast<EVObjectFlags>(static_cast<uint64_t>(ObjectFlags) & ~static_cast<uint64_t>(InObjectFlags));
}

bool VObject::HasAnyObjectFlags(const EVObjectFlags InObjectFlags) const
{
	return static_cast<uint64>(ObjectFlags & InObjectFlags) != 0;
}

bool VObject::HasAllObjectFlags(EVObjectFlags InObjectFlags) const
{
	return (static_cast<uint64>(ObjectFlags & InObjectFlags) == static_cast<uint64>(InObjectFlags));
}

void VObject::MarkPendingKill()
{
	AddFlags(EVObjectFlags::PendingKill);
}

void VObject::SetOuter(VObject* InOuter)
{
	Outer = InOuter;
}
	
void VObject::SetName(const VString& InName)
{
	Name = InName;
}

VObject* VObject::GetOuter() const
{
	return Outer;
}

VObject* VObject::GetOuterMost() const
{
	VObject* CurrentOuter = const_cast<VObject*>(this);

	while (CurrentOuter && CurrentOuter->GetOuter())
	{
		CurrentOuter = CurrentOuter->GetOuter();
	}

	return CurrentOuter;
}

EVObjectFlags VObject::GetObjectFlags() const
{
	return ObjectFlags;
}

VString VObject::GetName() const
{
	return Name;
}

VString VObject::GetPathName() const
{
	if (!Name) return VString::NullString;
	if (!Outer) return Name;

	const VString OuterPath = Outer->GetPathName();
	if (OuterPath.IsEmpty()) return Name;

	return OuterPath + "." + Name;
}

bool VObject::IsRootObject() const
{
	return HasAnyObjectFlags(EVObjectFlags::Root);
}

bool VObject::IsPendingKill() const
{
	return HasAnyObjectFlags(EVObjectFlags::PendingKill);
}
}