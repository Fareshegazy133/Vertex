// Vertex

#include "Core/Reflection/ReflectionRegistry.h"

#include "Core/Reflection/Class.h"
#include "Core/Reflection/Enum.h"

namespace VCore
{
VReflectionRegistry& VReflectionRegistry::Get()
{
	static VReflectionRegistry Instance;
	return Instance;
}

void VReflectionRegistry::RegisterClass(VClass* Class)
{
	if (!Class || !Class->GetName()) return;
	Classes[Class->GetName()] = Class;
}

void VReflectionRegistry::RegisterStruct(VStruct* Struct)
{
	if (!Struct || !Struct->GetName()) return;
	Structs[Struct->GetName()] = Struct;
}

void VReflectionRegistry::RegisterEnum(VEnum* Enum)
{
	if (!Enum || !Enum->GetName()) return;
	Enums[Enum->GetName()] = Enum;
}

VClass* VReflectionRegistry::FindClass(const VString& Name) const
{
	if (!Name) return nullptr;

	VClass* const* Value = Classes.Find(Name);
	if (!Value) return nullptr;
	
	return *Value;
}

VStruct* VReflectionRegistry::FindStruct(const VString& Name) const
{
	if (!Name) return nullptr;

	VStruct* const* Value = Structs.Find(Name);
	if (!Value) return nullptr;
	
	return *Value;
}

VEnum* VReflectionRegistry::FindEnum(const VString& Name) const
{
	if (!Name) return nullptr;

	VEnum* const* Value = Enums.Find(Name);
	if (!Value) return nullptr;

	return *Value;
}

const VMap<VString, VClass*>& VReflectionRegistry::GetClasses() const
{
	return Classes;
}

const VMap<VString, VStruct*>& VReflectionRegistry::GetStructs() const
{
	return Structs;
}

const VMap<VString, VEnum*>& VReflectionRegistry::GetEnums() const
{
	return Enums;
}
}