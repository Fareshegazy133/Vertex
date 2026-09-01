// Vertex

#pragma once

#include <string>
#include "Core/Types/Numeric.h"

namespace VCore
{
struct VWindowSpecification
{
	std::string WindowName;
	
	uint16 Width = 1280;
	uint16 Height = 720;
	
	bool bVSync = true;
	bool bResizable = true;
};

class VWindow
{
public:
	VWindow();
	~VWindow();

private:
	VWindowSpecification WindowSpecification;
};
}