// Vertex

#pragma once

#include "Core/Containers/Map.h"
#include "Core/Types/Name.h"

namespace VCore
{
class VClass;
class VStruct;
class VEnum;

class VReflectionRegistry
{
public:
	static VReflectionRegistry& Get();

	void RegisterClass(VClass* Class);
	void RegisterStruct(VStruct* Struct);
	void RegisterEnum(VEnum* Enum);

	VClass* FindClass(const VName& Name) const;
	VStruct* FindStruct(const VName& Name) const;
	VEnum* FindEnum(const VName& Name) const;

	const VMap<VName, VClass*>& GetClasses() const;
	const VMap<VName, VStruct*>& GetStructs() const;
	const VMap<VName, VEnum*>& GetEnums() const;

private:
	VReflectionRegistry() = default;

private:
	VMap<VName, VClass*> Classes;
	VMap<VName, VStruct*> Structs;
	VMap<VName, VEnum*> Enums;
};
}