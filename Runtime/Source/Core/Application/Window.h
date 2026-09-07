// Vertex

#pragma once

#include "Core/Types/Numeric.h"
#include "Core/Types/String.h"

namespace VCore
{
struct VWindowSpecification
{
	VString WindowName;
	
	uint16 Width = 1280;
	uint16 Height = 720;
	
	bool bVSync = true;
	bool bResizable = true;
};

class VWindow
{
public:
	VWindow(const VWindowSpecification& InWindowSpecification = VWindowSpecification());
	~VWindow();
	
	void Construct();
	void Destruct();

private:
	VWindowSpecification WindowSpecification;
	
	bool bIsConstructed = false;
};
}