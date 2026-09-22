// Vertex

#pragma once

#include "Core/Types/Name.h"

namespace VCore
{
class VObject;
class VClass;

class VObjectBaseUtility
{
public:
	static void Rename(VObject* Object, const VName& NewName);
	static void DestroyObject(VObject* Object);
	
	static void SetOuter(VObject* Object, VObject* NewOuter);
	
	static VObject* FindObject(const VClass* Class, const VObject* Outer, const VName& Name);
	static VObject* FindObjectByName(const VName& Name);
	static VObject* FindObjectByPath(const VName& Path);
	
	static bool IsValid(const VObject* Object);
};
}
