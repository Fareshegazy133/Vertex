// Vertex

#pragma once

#include "Render/Passes/RenderPass.h"

namespace VRender
{
class VWorldPass : public VRenderPass
{
public:
	void Execute(VRenderCore::VRenderer& Renderer) override;
};
}