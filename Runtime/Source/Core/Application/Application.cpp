// Vertex

#include "Core/Application/Application.h"

#include "raylib.h"
#include "Render/RenderCore/Renderer.h"

namespace VCore
{
VApplication::VApplication(const VApplicationSpecification& InApplicationSpecification)
	: Renderer(&RHI), ApplicationSpecification(InApplicationSpecification)
{
	MainWindow = MakeUnique<VWindow>(ApplicationSpecification.WindowSpecification);
	MainWindow->Construct();
	World.SpawnActor<VActor>();
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
		//Layer->Update(GetFrameTime());
	}
	
	World.Update(GetFrameTime());
}

void VApplication::Render()
{
	World.BuildRenderScene(RenderScene);
	Renderer.BeginFrame();
	
	for (const auto& Layer : LayerStack)
	{
		//Layer->Render(Renderer);
	}
	
	Renderer.Render(RenderScene, RenderView);
	Renderer.EndFrame();
}
}