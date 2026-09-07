// Vertex

#pragma once

#include "Core/Layers/Layer.h"
#include "Core/Memory/ObjectPtr.h"
#include "Input/InputAction.h"

namespace VCore
{
class VAppLayer : public VLayer
{
public:
	VAppLayer();
	~VAppLayer() override = default;

	void Update(const float32 DeltaTime) override;
	
private:
	VObjectPtr<VInput::VInputAction> TestAction;
};
}