// Vertex

#pragma once

#include "Core/Types/Numeric.h"

namespace VRenderCore
{
	class VRenderer;
}

namespace VCore
{
class VLayer
{
public:
	VLayer() = default;
	virtual ~VLayer() = default;

	virtual void OnAttach();
	virtual void OnDetach();

	virtual void Update(const float32 DeltaTime);
	virtual void Render(VRenderCore::VRenderer* Renderer);
};
}