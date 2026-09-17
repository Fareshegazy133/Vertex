// Vertex

#pragma once

#include "Core/Reflection/Struct.h"
#include "Core/Types/String.h"

namespace VCore
{
class VObject;
class VFunction;

using VObjectConstructor = VObject* (*)(VObject* Owner, VString Name);

class VClass : public VStruct
{
public:
	VClass(VClass* InParentClass = nullptr, const VString& InName = VString::NullString, const TSize ClassSize = 0);
	virtual ~VClass() override;

	virtual VObject* CreateObject(VObject* Owner, const VString& InName) const;

	void SetConstructor(const VObjectConstructor InConstructor);
	void AddFunction(VFunction* Function);

	VFunction* FindFunction(const VString& FunctionName) const;
	
	VObjectConstructor GetConstructor() const;
	VClass* GetParentClass() const;
	VFunction* GetFunctions() const;
	TSize GetClassSize() const;
	
	bool IsChildOf(const VClass* Parent) const;

private:
	VClass* ParentClass;
	
	VObjectConstructor Constructor;
	VFunction* Functions;
};
}