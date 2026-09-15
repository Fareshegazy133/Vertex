// Vertex

#include "Render/RenderCore/RenderCommand.h"

namespace VRenderCore
{
void VRenderCommandQueue::Submit(const VRenderCommand& RenderCommand)
{
	RenderCommands.Add(RenderCommand);
}

void VRenderCommandQueue::Clear()
{
	RenderCommands.Empty();
}

const VCore::VArray<VRenderCommand>& VRenderCommandQueue::GetCommands() const
{
	return RenderCommands;
}
}