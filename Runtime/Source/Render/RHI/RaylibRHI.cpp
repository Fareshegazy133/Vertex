// Vertex

#include "Render/RHI/RaylibRHI.h"

#include "raylib.h"
#include "Render/RenderCore/RenderObject.h"

namespace VRender
{
void VRaylibRHI::DrawObject(const VRenderCore::VRenderObject& RenderObject)
{
	const VCore::float32 Width = 50.0f * RenderObject.Transform.Scale.X;
	const VCore::float32 Height = 50.0f * RenderObject.Transform.Scale.Y;

	const Rectangle Rectangle(RenderObject.Transform.Position.X, RenderObject.Transform.Position.Y, Width, Height);
	const Vector2 Origin(Width * 0.5f, Height * 0.5f);

	DrawRectanglePro(Rectangle, Origin, RenderObject.Transform.Rotation, GREEN);
}

void VRaylibRHI::Clear()
{
	ClearBackground(BLACK);
}

void VRaylibRHI::BeginFrame()
{
	BeginDrawing();
}

void VRaylibRHI::EndFrame()
{
	EndDrawing();
}
}