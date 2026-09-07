// Vertex

#pragma once

#include "Core/Types/Numeric.h"

namespace VCore
{
class VLayer
{
public:
	VLayer() = default;
	virtual ~VLayer() = default;

	virtual void OnAttach();
	virtual void OnDetach();

	virtual void Update(float32 DeltaTime);
	virtual void Render();
};
}