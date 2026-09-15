// Vertex

#include "Render/RenderCore/Renderer.h"

#include "raylib.h"
#include "Render/RHI/RHI.h"

namespace VRenderCore
{
VRenderer::VRenderer(VRender::VRHI* InRHI)
	: RHI(InRHI)
{}

VRenderer::~VRenderer()
{}

void VRenderer::BeginFrame()
{
	RHI->BeginFrame();
	RHI->Clear();
	CommandQueue.Clear();
}

void VRenderer::EndFrame() const
{
	RHI->EndFrame();
}

void VRenderer::Render(const VRenderScene& Scene, const VRenderView& View)
{
	CurrentView = View;
	BuildCommands(Scene, View);
	SortCommands();
	ExecuteCommands();
}

VRender::VRHI& VRenderer::GetRHI() const
{
	return *RHI;
}

void VRenderer::BuildCommands(const VRenderScene& Scene, const VRenderView& View)
{
	for (const auto& Object : Scene.GetObjects())
	{
		VRenderCommand Command;
		Command.RenderCommandType = EVRenderCommandType::DrawObject;
		Command.RenderObject = Object;
		CommandQueue.Submit(Command);
	}
}

void VRenderer::SortCommands()
{
	// To be implemented
}

void VRenderer::ExecuteCommands()
{
	for (const auto& Command : CommandQueue.GetCommands())
	{
		switch (Command.RenderCommandType)
		{
			case EVRenderCommandType::DrawObject:
				RHI->DrawObject(Command.RenderObject);
				break;
		}
	}
}

void VRenderer::RenderWorld(const VWorld& World) const
{
	for (const auto& Actor : World.GetActors())
	{
		const VVector2D Location = Actor->GetActorPosition();
		const VVector2D Scale = Actor->GetActorScale();

		const float32 Width = 100.f * Scale.X;
		const float32 Height = 100.f * Scale.Y;

		DrawRectangle(Location.X, Location.Y, Width, Height, GREEN);
	}
}
}