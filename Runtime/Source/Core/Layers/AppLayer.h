// Vertex

#pragma once

#include "Core/Layers/Layer.h"
#include "Render/RenderCore/RenderView.h"
#include "Scene/World.h"

namespace VCore
{
class VAppLayer : public VLayer
{
public:
	VAppLayer();
	
	void Update(const float32 DeltaTime) override;
	void Render(VRenderCore::VRenderer* Renderer) override;
	
	
};
}