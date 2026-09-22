// Vertex

#include "Core/Reflection/NameProperty.h"

namespace VCore
{
void VNameProperty::SetValue(void* Container, const void* Value) const
{
	*reinterpret_cast<VName*>(static_cast<char*>(Container) + Offset) = *static_cast<const VName*>(Value);
}

void VNameProperty::GetValue(const void* Container, void* OutValue) const
{
	*static_cast<VName*>(OutValue) = *reinterpret_cast<const VName*>(static_cast<const char*>(Container) + Offset);
}

void* VNameProperty::GetValuePtr(void* Container) const
{
	return static_cast<char*>(Container) + Offset;
}

const void* VNameProperty::GetValuePtr(const void* Container) const
{
	return static_cast<const char*>(Container) + Offset;
}

EVPropertyType VNameProperty::GetPropertyType() const
{
	return EVPropertyType::Name;
}

TSize VNameProperty::GetSize() const
{
	return sizeof(VName);
}

TSize VNameProperty::GetAlignment() const
{
	return alignof(VName);
}
}