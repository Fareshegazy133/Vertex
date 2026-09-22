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
	if (!Class || !Class->GetName().IsValid()) return;
	Classes[Class->GetName()] = Class;
}

void VReflectionRegistry::RegisterStruct(VStruct* Struct)
{
	if (!Struct || !Struct->GetName().IsValid()) return;
	Structs[Struct->GetName()] = Struct;
}

void VReflectionRegistry::RegisterEnum(VEnum* Enum)
{
	if (!Enum || !Enum->GetName().IsValid()) return;
	Enums[Enum->GetName()] = Enum;
}

VClass* VReflectionRegistry::FindClass(const VName& Name) const
{
	if (!Name.IsValid()) return nullptr;

	VClass* const* Value = Classes.Find(Name);
	if (!Value) return nullptr;
	
	return *Value;
}

VStruct* VReflectionRegistry::FindStruct(const VName& Name) const
{
	if (!Name.IsValid()) return nullptr;

	VStruct* const* Value = Structs.Find(Name);
	if (!Value) return nullptr;
	
	return *Value;
}

VEnum* VReflectionRegistry::FindEnum(const VName& Name) const
{
	if (!Name.IsValid()) return nullptr;

	VEnum* const* Value = Enums.Find(Name);
	if (!Value) return nullptr;

	return *Value;
}

const VMap<VName, VClass*>& VReflectionRegistry::GetClasses() const
{
	return Classes;
}

const VMap<VName, VStruct*>& VReflectionRegistry::GetStructs() const
{
	return Structs;
}

const VMap<VName, VEnum*>& VReflectionRegistry::GetEnums() const
{
	return Enums;
}
}