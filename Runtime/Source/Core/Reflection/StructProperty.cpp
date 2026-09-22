// Vertex

#include "Core/Reflection/StructProperty.h"

#include "Core/Reflection/Struct.h"

namespace VCore
{
VStructProperty::VStructProperty()
	: Struct(nullptr)
{}

VStructProperty::VStructProperty(VStruct* InStruct)
	: Struct(InStruct)
{}

void VStructProperty::SetValue(void* Container, const void* Value) const
{
	void* Destination = static_cast<char*>(Container) + Offset;
	Struct->Copy(Destination, Value);
}

void VStructProperty::GetValue(const void* Container, void* OutValue) const
{
	const void* Source = static_cast<const char*>(Container) + Offset;
	Struct->Copy(OutValue, Source);
}

void* VStructProperty::GetValuePtr(void* Container) const
{
	return static_cast<char*>(Container) + Offset;
}

const void* VStructProperty::GetValuePtr(const void* Container) const
{
	return static_cast<const char*>(Container) + Offset;
}

EVPropertyType VStructProperty::GetPropertyType() const
{
	return EVPropertyType::Struct;
}

TSize VStructProperty::GetSize() const
{
	return Struct ? Struct->GetStructSize() : 0;
}

TSize VStructProperty::GetAlignment() const
{
	return Struct ? Struct->GetStructAlignment() : 1;
}

VStruct* VStructProperty::GetStruct() const
{
	return Struct;
}
}