// Vertex

#include "Core/Reflection/StringProperty.h"

namespace VCore
{
void VStringProperty::SetValue(void* Container, const void* Value) const
{
	*reinterpret_cast<VString*>(static_cast<char*>(Container) + Offset) = *static_cast<const VString*>(Value);
}

void VStringProperty::GetValue(const void* Container, void* OutValue) const
{
	*static_cast<VString*>(OutValue) = *reinterpret_cast<const VString*>(static_cast<const char*>(Container) + Offset);
}

void* VStringProperty::GetValuePtr(void* Container) const
{
	return static_cast<char*>(Container) + Offset;
}

const void* VStringProperty::GetValuePtr(const void* Container) const
{
	return static_cast<const char*>(Container) + Offset;
}

EVPropertyType VStringProperty::GetPropertyType() const
{
	return EVPropertyType::String;
}

TSize VStringProperty::GetSize() const
{
	return sizeof(VString);
}

TSize VStringProperty::GetAlignment() const
{
	return alignof(VString);
}
}