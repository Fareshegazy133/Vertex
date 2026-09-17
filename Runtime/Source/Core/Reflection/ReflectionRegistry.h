// Vertex

#pragma once

#include "Core/Containers/Map.h"
#include "Core/Types/String.h"

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

	VClass* FindClass(const VString& Name) const;
	VStruct* FindStruct(const VString& Name) const;
	VEnum* FindEnum(const VString& Name) const;

	const VMap<VString, VClass*>& GetClasses() const;
	const VMap<VString, VStruct*>& GetStructs() const;
	const VMap<VString, VEnum*>& GetEnums() const;

private:
	VReflectionRegistry() = default;

private:
	VMap<VString, VClass*> Classes;
	VMap<VString, VStruct*> Structs;
	VMap<VString, VEnum*> Enums;
};
}