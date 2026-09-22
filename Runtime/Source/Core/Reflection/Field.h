// Vertex

#pragma once

#include "Core/Types/Name.h"

namespace VCore
{
class VField
{
public:
	VField() = default;
	virtual ~VField() = default;

	void SetNextField(VField* InNextField);
	void SetName(const VName& InName);
	
	VField* GetNextField() const;
	VName GetName() const;

protected:
	VField* NextField = nullptr;
	VName FieldName = NAME_None;
};
}