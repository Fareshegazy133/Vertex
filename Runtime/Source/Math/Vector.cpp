// Vertex

#include "Math/Vector.h"

namespace VMath
{
const VVector2 VVector2::ZeroVector = VVector2();

VVector2::VVector2(const VCore::float32 X, const VCore::float32 Y)
	: X(X), Y(Y)
{}

VVector2::VVector2(const VCore::float32 XY)
	: X(XY), Y(XY)
{}

bool VVector2::IsZero() const
{
	return X == 0.f && Y == 0.f;
}

VVector2 VVector2::operator+(const VVector2& OtherVector) const
{
	return VVector2(X + OtherVector.X, Y + OtherVector.Y);
}

VVector2 VVector2::operator-(const VVector2& OtherVector) const
{
	return VVector2(X - OtherVector.X, Y - OtherVector.Y);
}

VVector2 VVector2::operator*(const VVector2& OtherVector) const
{
	return VVector2(X * OtherVector.X, Y * OtherVector.Y);
}

VVector2 VVector2::operator/(const VVector2& OtherVector) const
{
	return VVector2(X / OtherVector.X, Y / OtherVector.Y);
}

VVector2 VVector2::operator+=(const VVector2& OtherVector)
{
	X += OtherVector.X;
	Y += OtherVector.Y;
	return *this;
}

VVector2 VVector2::operator-=(const VVector2& OtherVector)
{
	X -= OtherVector.X;
	Y -= OtherVector.Y;
	return *this;
}

VVector2 VVector2::operator*=(const VVector2& OtherVector)
{
	X *= OtherVector.X;
	Y *= OtherVector.Y;
	return *this;
}

VVector2 VVector2::operator/=(const VVector2& OtherVector)
{
	X /= OtherVector.X;
	Y /= OtherVector.Y;
	return *this;
}

bool VVector2::operator==(const VVector2& OtherVector) const
{
	return X == OtherVector.X && Y == OtherVector.Y;
}

bool VVector2::operator!=(const VVector2& OtherVector) const
{
	return !(*this == OtherVector);
}

const VVector3 VVector3::ZeroVector = VVector3();

VVector3::VVector3(const VCore::float32 X, const VCore::float32 Y, const VCore::float32 Z)
	: X(X), Y(Y), Z(Z)
{}

VVector3::VVector3(const VCore::float32 XYZ)
	: X(XYZ), Y(XYZ), Z(XYZ)
{}

bool VVector3::IsZero() const
{
	return X == 0.f && Y == 0.f && Z == 0.f;
}

VVector3 VVector3::operator+(const VVector3& OtherVector) const
{
	return VVector3(X + OtherVector.X, Y + OtherVector.Y, Z + OtherVector.Z);
}

VVector3 VVector3::operator-(const VVector3& OtherVector) const
{
	return VVector3(X - OtherVector.X, Y - OtherVector.Y, Z - OtherVector.Z);
}

VVector3 VVector3::operator*(const VVector3& OtherVector) const
{
	return VVector3(X * OtherVector.X, Y * OtherVector.Y, Z * OtherVector.Z);
}

VVector3 VVector3::operator/(const VVector3& OtherVector) const
{
	return VVector3(X / OtherVector.X, Y / OtherVector.Y, Z / OtherVector.Z);
}

VVector3 VVector3::operator+=(const VVector3& OtherVector)
{
	X += OtherVector.X;
	Y += OtherVector.Y;
	return *this;	
}

VVector3 VVector3::operator-=(const VVector3& OtherVector)
{
	X -= OtherVector.X;
	Y -= OtherVector.Y;
	return *this;
}

VVector3 VVector3::operator*=(const VVector3& OtherVector)
{
	X *= OtherVector.X;
	Y *= OtherVector.Y;
	return *this;
}

VVector3 VVector3::operator/=(const VVector3& OtherVector)
{
	X /= OtherVector.X;
	Y /= OtherVector.Y;
	return *this;
}

bool VVector3::operator==(const VVector3& OtherVector) const
{
	return X == OtherVector.X && Y == OtherVector.Y && Z == OtherVector.Z;
}

bool VVector3::operator!=(const VVector3& OtherVector) const
{
	return !(*this == OtherVector);
}
}