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

	virtual void Update(const float32 DeltaTime) override;
	virtual void Render(VRenderCore::VRenderer* Renderer) override;
	
	
};
}