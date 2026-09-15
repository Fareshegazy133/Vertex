// Vertex

#include "Core/Object/ObjectBaseUtility.h"

namespace VCore
{
void VObjectBaseUtility::SetName(const VString& InName)
{
	Name = InName;
}

VString VObjectBaseUtility::GetName() const
{
	return Name;
}
}