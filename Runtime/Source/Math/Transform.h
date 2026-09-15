// Vertex

#pragma once

#include "Rotator.h"
#include "Vector.h"

namespace VMath
{
struct VTransform2
{
	VTransform2() = default;
	VTransform2(const VVector2D& Location, const VRotator2D& Rotation, const VVector2D& Scale);
	
	VVector2D Position = VVector2D::ZeroVector;
	VRotator2D Rotation = VRotator2D::ZeroRotator;
	VVector2D Scale = VVector2D(1.f);
};

struct VTransform3
{
	VTransform3() = default;
	VTransform3(const VVector& Location, const VRotator& Rotation, const VVector& Scale);
	
	VVector Position = VVector::ZeroVector;
	VRotator Rotation = VRotator::ZeroRotator;
	VVector Scale = VVector(1.f);
};

using VTransform2D = VTransform2;
using VTransform3D = VTransform3;
using VTransform = VTransform3D;
}