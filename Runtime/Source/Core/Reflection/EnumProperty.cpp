// Vertex

#include "Core/Reflection/EnumProperty.h"

namespace VCore
{
VEnumProperty::VEnumProperty()
	: Enum(nullptr)
{}

VEnumProperty::VEnumProperty(VEnum* InEnum)
	: Enum(InEnum)
{}

void VEnumProperty::SetValue(void* Container, const void* Value) const
{
	*reinterpret_cast<int64*>(static_cast<char*>(Container) + Offset) = *static_cast<const int64*>(Value);
}

void VEnumProperty::GetValue(const void* Container, void* OutValue) const
{
	*static_cast<int64*>(OutValue) = *reinterpret_cast<const int64*>(static_cast<const char*>(Container) + Offset);
}

void* VEnumProperty::GetValuePtr(void* Container) const
{
	return static_cast<char*>(Container) + Offset;
}

const void* VEnumProperty::GetValuePtr(const void* Container) const
{
	return static_cast<const char*>(Container) + Offset;
}

EVPropertyType VEnumProperty::GetPropertyType() const
{
	return EVPropertyType::Enum;
}

TSize VEnumProperty::GetSize() const
{
	return sizeof(int64);
}

TSize VEnumProperty::GetAlignment() const
{
	return alignof(int64);
}

VEnum* VEnumProperty::GetEnum() const
{
	return Enum;
}
}