// Vertex

#include "Core/Reflection/Field.h"

namespace VCore
{
void VField::SetNextField(VField* InNextField)
{
	NextField = InNextField;
}

void VField::SetName(const VName& InName)
{
	FieldName = InName;
}

VField* VField::GetNextField() const
{
	return NextField;
}

VName VField::GetName() const
{
	return FieldName;
}
}