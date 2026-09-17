// Vertex

#include "Core/Reflection/Field.h"

namespace VCore
{
void VField::SetNextField(VField* InNextField)
{
	NextField = InNextField;
}

void VField::SetName(const VString& InName)
{
	FieldName = InName;
}

VField* VField::GetNextField() const
{
	return NextField;
}

VString VField::GetName() const
{
	return FieldName;
}
}