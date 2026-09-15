// Vertex

#pragma once

namespace VRenderCore
{
	class VRenderer;
}

namespace VRender
{
class VRenderPass
{
public:
	virtual ~VRenderPass() = default;
	virtual void Execute(VRenderCore::VRenderer& Renderer) = 0;
};
}