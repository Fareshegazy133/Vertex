// Vertex

#pragma once

#include "Core/Reflection/Struct.h"

namespace VCore
{
class VObject;
class VFunction;

using VObjectConstructor = VObject* (*)(VObject* Owner, VName Name);

class VClass : public VStruct
{
public:
	VClass(VClass* InParentClass = nullptr, const VName& InName = NAME_None, const TSize ClassSize = 0);
	virtual ~VClass() override;

	virtual VObject* CreateObject(VObject* Owner, const VName& InName) const;

	void SetConstructor(const VObjectConstructor InConstructor);
	void AddFunction(VFunction* Function);

	VFunction* FindFunction(const VName& FunctionName) const;
	
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