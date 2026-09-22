// Vertex

#include "Core/Reflection/ObjectProperty.h"

namespace VCore
{
VObjectProperty::VObjectProperty()
	: PropertyClass(nullptr)
{}

VObjectProperty::VObjectProperty(VClass* InPropertyClass)
	: PropertyClass(InPropertyClass)
{}

void VObjectProperty::SetValue(void* Container, const void* Value) const
{
	*reinterpret_cast<VObject**>(static_cast<char*>(Container) + Offset) = *static_cast<VObject* const*>(Value);
}

void VObjectProperty::GetValue(const void* Container, void* OutValue) const
{
	*static_cast<VObject**>(OutValue) = *reinterpret_cast<VObject* const*>(static_cast<const char*>(Container) + Offset);
}

void* VObjectProperty::GetValuePtr(void* Container) const
{
	return static_cast<char*>(Container) + Offset;
}

const void* VObjectProperty::GetValuePtr(const void* Container) const
{
	return static_cast<const char*>(Container) + Offset;
}

EVPropertyType VObjectProperty::GetPropertyType() const
{
	return EVPropertyType::Object;
}

TSize VObjectProperty::GetSize() const
{
	return sizeof(VObject*);
}

TSize VObjectProperty::GetAlignment() const
{
	return alignof(VObject*);
}

VClass* VObjectProperty::GetPropertyClass() const
{
	return PropertyClass;
}
}