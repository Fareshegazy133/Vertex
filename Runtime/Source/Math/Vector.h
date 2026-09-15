// Vertex

#pragma once

#include "Core/Types/Numeric.h"

namespace VMath
{
struct VVector2
{
	VVector2() = default;
	VVector2(const VCore::float32 X, const VCore::float32 Y);
	VVector2(const VCore::float32 XY);
	
	bool IsZero() const;
	
	VVector2 operator+(const VVector2& OtherVector) const;
	VVector2 operator-(const VVector2& OtherVector) const;
	VVector2 operator*(const VVector2& OtherVector) const;
	VVector2 operator/(const VVector2& OtherVector) const;
	
	VVector2 operator+=(const VVector2& OtherVector);
	VVector2 operator-=(const VVector2& OtherVector);
	VVector2 operator*=(const VVector2& OtherVector);
	VVector2 operator/=(const VVector2& OtherVector);
	
	bool operator==(const VVector2& OtherVector) const;
	bool operator!=(const VVector2& OtherVector) const;
	
	static const VVector2 ZeroVector;
	
	VCore::float32 X = 0.f;
	VCore::float32 Y = 0.f;
};

struct VVector3
{
	VVector3() = default;
	VVector3(const VCore::float32 X, const VCore::float32 Y, const VCore::float32 Z);
	VVector3(const VCore::float32 XYZ);

	bool IsZero() const;

	VVector3 operator+(const VVector3& OtherVector) const;
	VVector3 operator-(const VVector3& OtherVector) const;
	VVector3 operator*(const VVector3& OtherVector) const;
	VVector3 operator/(const VVector3& OtherVector) const;

	VVector3 operator+=(const VVector3& OtherVector);
	VVector3 operator-=(const VVector3& OtherVector);
	VVector3 operator*=(const VVector3& OtherVector);
	VVector3 operator/=(const VVector3& OtherVector);

	bool operator==(const VVector3& OtherVector) const;
	bool operator!=(const VVector3& OtherVector) const;

	static const VVector3 ZeroVector;

	VCore::float32 X = 0.f;
	VCore::float32 Y = 0.f;
	VCore::float32 Z = 0.f;
};

using VVector2D = VVector2;
using VVector3D = VVector3;
using VVector = VVector3D;
}