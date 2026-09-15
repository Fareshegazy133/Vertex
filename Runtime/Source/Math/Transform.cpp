// Vertex

#include "Math/Transform.h"

namespace VMath
{
VTransform2::VTransform2(const VVector2D& Location, const VRotator2D& Rotation, const VVector2D& Scale)
	: Position(Location), Rotation(Rotation), Scale(Scale)
{}

VTransform3::VTransform3(const VVector& Location, const VRotator& Rotation, const VVector& Scale)
	: Position(Location), Rotation(Rotation), Scale(Scale)
{}
}