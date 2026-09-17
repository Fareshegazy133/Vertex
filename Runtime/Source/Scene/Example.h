// Vertex

#pragma once

#include "Core/Object/Object.h"

class VExample : public VCore::VObject
{
public:
	VExample();
	
	virtual VCore::VClass* GetClass() const override;
	static VCore::VClass* StaticClass();
};