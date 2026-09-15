// Vertex

#pragma once

#include "Core/CoreAliases.h"
#include "Core/Object/ObjectBaseUtility.h"

namespace VCore
{
class VObject : public VObjectBaseUtility
{
public:
	~VObject() override = default;
	
protected:
	bool IsValid(const VObject* Object) const;
};
}