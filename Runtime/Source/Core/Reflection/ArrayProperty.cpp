// Vertex

#include "Core/Reflection/ArrayProperty.h"

namespace VCore
{
VArrayProperty::VArrayProperty()
	: InnerProperty(nullptr)
{}

VArrayProperty::VArrayProperty(VProperty* InInnerProperty)
	: InnerProperty(InInnerProperty)
{}

void VArrayProperty::SetValue(void* Container, const void* Value) const
{
	*reinterpret_cast<VArray<VObject*>*>(static_cast<char*>(Container) + Offset) = *static_cast<const VArray<VObject*>*>(Value);
}

void VArrayProperty::GetValue(const void* Container, void* OutValue) const
{
	
}

void* VArrayProperty::GetValuePtr(void* Container) const
{
	return nullptr;
}

const void* VArrayProperty::GetValuePtr(const void* Container) const
{
	return nullptr;
}

EVPropertyType VArrayProperty::GetPropertyType() const
{
	return EVPropertyType::Array;
}

TSize VArrayProperty::GetSize() const
{
	return sizeof(VArray<VObject*>);
}

TSize VArrayProperty::GetAlignment() const
{
	return alignof(VArray<VObject*>);
}

VProperty* VArrayProperty::GetInnerProperty() const
{
	return InnerProperty;
}
}