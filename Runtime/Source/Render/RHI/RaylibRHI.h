// Vertex

#pragma once

#include "Render/RHI/RHI.h"

namespace VRender
{
class VRaylibRHI : public VRHI
{
public:
	void DrawObject(const VRenderCore::VRenderObject& RenderObject) override;
	void Clear() override;
	
	void BeginFrame() override;
	void EndFrame() override;
};
}