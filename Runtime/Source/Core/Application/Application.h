// Vertex

#pragma once

#include "Core/Application/Window.h"
#include "Core/Memory/UniquePtr.h"

namespace VCore
{
struct VApplicationSpecification
{
	VString ApplicationName = "Vertex";
	VWindowSpecification WindowSpecification;
};

class VApplication
{
public:
	VApplication(const VApplicationSpecification& InApplicationSpecification = VApplicationSpecification());
	~VApplication();
	
	void RunApplication();

private:
	void Update();
	void Render();
	
private:
	VUniquePtr<VWindow> MainWindow;
	
	VApplicationSpecification ApplicationSpecification;
	
	bool bIsRunning = false;
};
}