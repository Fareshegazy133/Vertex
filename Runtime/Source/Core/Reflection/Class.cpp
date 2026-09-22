// Vertex

#include "Core/Reflection/Class.h"

#include "Core/Reflection/Function.h"

namespace VCore
{
VClass::VClass(VClass* InParentClass, const VName& InName, const TSize ClassSize)
	: VStruct(InParentClass, InName, ClassSize), ParentClass(InParentClass), Constructor(nullptr), Functions(nullptr)
{}

VClass::~VClass()
{
	const VFunction* CurrentFunction = Functions;

	while (CurrentFunction)
	{
		const VFunction* NextFunction = dynamic_cast<VFunction*>(CurrentFunction->GetNextField());
		
		delete CurrentFunction;
		CurrentFunction = NextFunction;
	}
}

VObject* VClass::CreateObject(VObject* Owner, const VName& InName) const
{
	if (!Constructor) return nullptr;
	return Constructor(Owner, InName);
}

void VClass::SetConstructor(const VObjectConstructor InConstructor)
{
	Constructor = InConstructor;
}

void VClass::AddFunction(VFunction* Function)
{
	if (!Function) return;

	Function->SetNextField(Functions);
	Functions = Function;
}

VFunction* VClass::FindFunction(const VName& FunctionName) const
{
	if (!FunctionName.IsValid()) return nullptr;

	for (VFunction* Function = Functions; Function; Function = dynamic_cast<VFunction*>(Function->GetNextField()))
	{
		if (Function->GetName() == FunctionName) return Function;
	}

	if (ParentClass) return ParentClass->FindFunction(FunctionName);
	return nullptr;
}

VObjectConstructor VClass::GetConstructor() const
{
	return Constructor;
}

VClass* VClass::GetParentClass() const
{
	return ParentClass;
}

VFunction* VClass::GetFunctions() const
{
	return Functions;
}

TSize VClass::GetClassSize() const
{
	return StructSize;
}

bool VClass::IsChildOf(const VClass* Parent) const
{
	if (!Parent) return false;
	const VClass* CurrentClass = this;

	while (CurrentClass)
	{
		if (CurrentClass == Parent) return true;
		CurrentClass = CurrentClass->GetParentClass();
	}

	return false;
}
}
