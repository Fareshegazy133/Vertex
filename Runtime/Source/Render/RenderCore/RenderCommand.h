// Vertex

#pragma once

#include "Core/Containers/Array.h"
#include "Core/Types/Numeric.h"
#include "Render/RenderCore/RenderObject.h"

namespace VRenderCore
{
enum class EVRenderCommandType : VCore::uint8
{
	DrawObject
};

struct VRenderCommand
{
	VRenderObject RenderObject;
	EVRenderCommandType RenderCommandType;
};

class VRenderCommandQueue
{
public:
	void Submit(const VRenderCommand& RenderCommand);
	void Clear();

	const VCore::VArray<VRenderCommand>& GetCommands() const;

private:
	VCore::VArray<VRenderCommand> RenderCommands;
};
}