// Vertex

#pragma once

#include "Math/Transform.h"

namespace VRenderCore
{
struct VRenderObject
{
	void* Mesh = nullptr;
	void* Material = nullptr;
	
	VMath::VTransform2D Transform;
};
}