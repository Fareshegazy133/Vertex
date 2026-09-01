// Vertex

#include "EntryPoint.h"

#include "Core/Application/Application.h"

void VertexStart()
{
	VCore::VApplicationSpecification ApplicationSpecification;
	ApplicationSpecification.ApplicationName = "Vertex Editor";
	ApplicationSpecification.WindowSpecification.WindowName = "Vertex Editor";
	ApplicationSpecification.WindowSpecification.Width = 1280;
	ApplicationSpecification.WindowSpecification.Height = 720;
	
	VCore::VApplication Application(ApplicationSpecification);
	Application.RunApplication();
}

int main()
{
	VertexStart();
}