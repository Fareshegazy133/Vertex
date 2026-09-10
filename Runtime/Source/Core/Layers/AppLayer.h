// Vertex

#pragma once

#include "Core/Layers/Layer.h"
#include "Core/Memory/ReferencePtr.h"
#include "Input/InputAction.h"

namespace VCore
{
class VAppLayer : public VLayer
{
public:
	VAppLayer();
	~VAppLayer() override = default;

	void Update(const float32 DeltaTime) override;
	void DrawSquare() const;
	
private:
	VReferencePtr<VInput::VInputAction> TestAction;
};
}