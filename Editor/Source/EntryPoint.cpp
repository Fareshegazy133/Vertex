// Vertex

#include "EntryPoint.h"

#include "Core/Application/Application.h"
#include "Core/Layers/AppLayer.h"
#include "Core/Layers/OverlayLayer.h"

void VertexStart()
{
	VCore::VApplicationSpecification ApplicationSpecification;
	ApplicationSpecification.ApplicationName = "Vertex Editor";
	ApplicationSpecification.WindowSpecification.WindowName = "Vertex Editor";
	ApplicationSpecification.WindowSpecification.Width = 1280;
	ApplicationSpecification.WindowSpecification.Height = 720;
	
	VCore::VApplication Application(ApplicationSpecification);
	Application.PushLayer<VCore::VAppLayer>();
	Application.PushLayer<VCore::VOverlayLayer>();
	Application.RunApplication();
}

int main()
{
	VertexStart();
}