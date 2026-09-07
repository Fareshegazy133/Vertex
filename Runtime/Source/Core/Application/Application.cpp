// Vertex

#include "Core/Application/Application.h"

#include "raylib.h"

namespace VCore
{
VApplication::VApplication(const VApplicationSpecification& InApplicationSpecification)
	: ApplicationSpecification(InApplicationSpecification)
{
	MainWindow = MakeUnique<VWindow>(ApplicationSpecification.WindowSpecification);
	MainWindow->Construct();
}

VApplication::~VApplication()
{
	MainWindow->Destruct();
}

void VApplication::RunApplication()
{
	bIsRunning = true;
	
	while (bIsRunning)
	{
		Update();
		Render();
	}
}

void VApplication::Update()
{
	bIsRunning = !WindowShouldClose();
	
	for (const auto& Layer : LayerStack)
	{
		Layer->Update(GetFrameTime());
	}
}

void VApplication::Render()
{
	BeginDrawing();
	ClearBackground(BLACK);
	
	for (const auto& Layer : LayerStack)
	{
		Layer->Render();
	}
	
	EndDrawing();
}
}