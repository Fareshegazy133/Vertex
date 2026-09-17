// Vertex

#pragma once

#include "Core/Reflection/Field.h"
#include "Core/Types/String.h"

namespace VCore
{
class VObject;
class VStruct;
class VProperty;

using VNativeFunction = void (*)(VObject* Object, void* Parameters);

class VFunction : public VField
{
public:
	VFunction(const VNativeFunction InInvoker = nullptr, const VString& InName = VString::NullString);
	virtual ~VFunction() override = default;

	virtual void Invoke(VObject* Object, void* Parameters) const;

	void SetInvoker(const VNativeFunction InInvoker);
	void SetParameterStruct(VStruct* InParameterStruct);
	void SetReturnProperty(VProperty* InReturnProperty);
	
	void SetIsStatic(const bool IsStatic);
	void SetIsConst(const bool IsConst);
	
	VNativeFunction GetInvoker() const;
	VStruct* GetParameterStruct() const;
	VProperty* GetReturnProperty() const;
	
	bool IsStatic() const;
	bool IsConst() const;

private:
	VNativeFunction Invoker;
	VStruct* ParameterStruct;
	VProperty* ReturnProperty;

	bool bIsStatic = false;
	bool bIsConst = false;
};
}