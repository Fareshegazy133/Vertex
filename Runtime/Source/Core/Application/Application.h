// Vertex

#pragma once

#include "Core/Application/Window.h"

namespace VCore
{
struct VApplicationSpecification
{
	std::string ApplicationName = "Vertex";
	VWindowSpecification WindowSpecification;
};

class VApplication
{
public:
	VApplication();
	~VApplication();

private:
	VApplicationSpecification ApplicationSpecification;
	
	bool bIsRunning = false;
};
}