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
}

void VApplication::Render()
{
	BeginDrawing();
	ClearBackground(BLACK);
	EndDrawing();
}
}