// Vertex

#pragma once

#include "Math/Transform.h"

namespace VRenderCore
{
struct VRenderView
{
	VMath::VTransform Transform;
	VCore::float32 Zoom = 1.f;
	
	VCore::float32 Width = 1280.f;
	VCore::float32 Height = 720.f;
};
}