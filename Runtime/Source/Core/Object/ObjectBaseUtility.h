// Vertex

#pragma once

#include "Core/Types/String.h"

namespace VCore
{
class VObject;
class VClass;

class VObjectBaseUtility
{
public:
	static void Rename(VObject* Object, const VString& NewName);
	static void DestroyObject(VObject* Object);
	
	static void SetOuter(VObject* Object, VObject* NewOuter);
	
	static VObject* FindObject(const VClass* Class, const VObject* Outer, const VString& Name);
	static VObject* FindObjectByName(const VString& Name);
	static VObject* FindObjectByPath(const VString& Path);
	
	static bool IsValid(const VObject* Object);
};
}