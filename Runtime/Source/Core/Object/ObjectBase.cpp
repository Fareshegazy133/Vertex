// Vertex

#include "Core/Object/ObjectBase.h"

#include "Core/Reflection/Class.h"

namespace VCore
{
bool VObjectBase::IsA(const VClass* Class) const
{
	if (!Class) return false;
	const VClass* CurrentClass = GetClass();

	while (CurrentClass)
	{
		if (CurrentClass == Class) return true;
		CurrentClass = CurrentClass->GetParentClass();
	}

	return false;
}
}