// Vertex

#include "Core/Reflection/Function.h"

namespace VCore
{
VFunction::VFunction(const VNativeFunction InInvoker, const VName& InName)
	: Invoker(InInvoker), ParameterStruct(nullptr), ReturnProperty(nullptr)
{
	FieldName = InName;
}

void VFunction::Invoke(VObject* Object, void* Parameters) const
{
	if (!Invoker) return;
	Invoker(Object, Parameters);
}

void VFunction::SetInvoker(const VNativeFunction InInvoker)
{
	Invoker = InInvoker;
}

void VFunction::SetParameterStruct(VStruct* InParameterStruct)
{
	ParameterStruct = InParameterStruct;
}

void VFunction::SetReturnProperty(VProperty* InReturnProperty)
{
	ReturnProperty = InReturnProperty;
}

void VFunction::SetIsStatic(const bool IsStatic)
{
	bIsStatic = IsStatic;
}

void VFunction::SetIsConst(const bool IsConst)
{
	bIsConst = IsConst;
}

VNativeFunction VFunction::GetInvoker() const
{
	return Invoker;
}

VStruct* VFunction::GetParameterStruct() const
{
	return ParameterStruct;
}

VProperty* VFunction::GetReturnProperty() const
{
	return ReturnProperty;
}

bool VFunction::IsStatic() const
{
	return bIsStatic;
}

bool VFunction::IsConst() const
{
	return bIsConst;
}
}