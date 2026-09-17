// Vertex

#pragma once

#include "Core/Types/String.h"

namespace VCore
{
class VField
{
public:
	VField() = default;
	virtual ~VField() = default;

	void SetNextField(VField* InNextField);
	void SetName(const VString& InName);
	
	VField* GetNextField() const;
	VString GetName() const;

protected:
	VField* NextField = nullptr;
	VString FieldName = VString::NullString;
};
}