// Vertex

#include "Core/Reflection/Struct.h"

#include "Core/Reflection/Property.h"

namespace VCore
{
VStruct::VStruct(VStruct* InParentStruct, const VName& InName, const TSize InSize)
	: ParentStruct(InParentStruct), Properties(nullptr), StructAlignment(1), StructSize(InSize)
{
	FieldName = InName;
}

VStruct::~VStruct()
{
	const VProperty* CurrentProperty = Properties;

	while (CurrentProperty)
	{
		const VProperty* NextProperty = dynamic_cast<VProperty*>(CurrentProperty->GetNextField());
		
		delete CurrentProperty;
		CurrentProperty = NextProperty;
	}
}

void VStruct::Initialize(void* Data) const
{
	if (!Data) return;
	std::memset(Data, 0, StructSize);
}

void VStruct::Destroy(void* Data) const
{
	(void)Data;
}

void VStruct::Copy(void* Dest, const void* Source) const
{
	if (!Dest || !Source) return;
	std::memcpy(Dest, Source, StructSize);
}

void VStruct::AddProperty(VProperty* Property)
{
	if (!Property) return;
	Property->SetNextField(Properties);
	Properties = Property;
}

void VStruct::SetStructSize(const TSize InStructSize)
{
	StructSize = InStructSize;
}

void VStruct::SetStructAlignment(const TSize InStructAlignment)
{
	StructAlignment = InStructAlignment;
}

VProperty* VStruct::FindProperty(const VName& PropertyName) const
{
	if (!PropertyName.IsValid()) return nullptr;

	for (VProperty* Property = Properties; Property; Property = dynamic_cast<VProperty*>(Property->GetNextField()))
	{
		if (Property->GetName() == PropertyName) return Property;
	}

	if (ParentStruct) return ParentStruct->FindProperty(PropertyName);
	return nullptr;
}

VStruct* VStruct::GetParentStruct() const
{
	return ParentStruct;
}

VProperty* VStruct::GetProperties() const
{
	return Properties;
}

TSize VStruct::GetStructAlignment() const
{
	return StructAlignment;
}

TSize VStruct::GetStructSize() const
{
	return StructSize;
}
}