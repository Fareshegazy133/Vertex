// Vertex

#include "Core/Reflection/Property.h"

namespace VCore
{
void VProperty::SetOffset(const TSize InOffset)
{
	Offset = InOffset;
}

void VProperty::SetIsEditable(const bool IsEditable)
{
	bIsEditable = IsEditable;
}
	
TSize VProperty::GetOffset() const
{
	return Offset;
}

bool VProperty::IsEditable() const
{
	return bIsEditable;
}
}