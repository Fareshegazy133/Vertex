// Vertex

#pragma once

#include "Core/Object/ObjectBase.h"
#include "Core/Types/String.h"

namespace VCore
{
class VObjectBaseUtility : public VObjectBase
{
public:
	void SetName(const VString& InName);
	VString GetName() const;

private:
	VString Name = VString::EmptyString;
};
}