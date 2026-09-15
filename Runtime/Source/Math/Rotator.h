// Vertex

#pragma once

#include "Core/Types/Numeric.h"

namespace VMath
{
struct VRotator2
{
	VRotator2() = default;
	VRotator2(const VCore::float32 Angle);

	bool IsZero() const;

	VRotator2 operator+(const VRotator2& OtherRotator) const;
	VRotator2 operator-(const VRotator2& OtherRotator) const;
	VRotator2 operator*(const VRotator2& OtherRotator) const;
	VRotator2 operator/(const VRotator2& OtherRotator) const;

	VRotator2 operator+=(const VRotator2& OtherRotator);
	VRotator2 operator-=(const VRotator2& OtherRotator);
	VRotator2 operator*=(const VRotator2& OtherRotator);
	VRotator2 operator/=(const VRotator2& OtherRotator);

	bool operator==(const VRotator2& OtherRotator) const;
	bool operator!=(const VRotator2& OtherRotator) const;
	
	operator VCore::float32() const;
	static const VRotator2 ZeroRotator;
	
	VCore::float32 Angle = 0.f;
};

struct VRotator3
{
	VRotator3() = default;
	VRotator3(const VCore::float32 Roll, const VCore::float32 Pitch, const VCore::float32 Yaw);
	VRotator3(const VCore::float32 RollPitchYaw);
	
	static VCore::float32 NormalizeAngle(VCore::float32 Angle);
	bool IsZero() const;
	
	VRotator3 operator+(const VRotator3& OtherRotator) const;
	VRotator3 operator-(const VRotator3& OtherRotator) const;
	VRotator3 operator*(const VRotator3& OtherRotator) const;
	VRotator3 operator/(const VRotator3& OtherRotator) const;
	
	VRotator3 operator+=(const VRotator3& OtherRotator);
	VRotator3 operator-=(const VRotator3& OtherRotator);
	VRotator3 operator*=(const VRotator3& OtherRotator);
	VRotator3 operator/=(const VRotator3& OtherRotator);
	
	bool operator==(const VRotator3& OtherRotator) const;
	bool operator!=(const VRotator3& OtherRotator) const;
	
	static const VRotator3 ZeroRotator;
	
	VCore::float32 Roll = 0.f;
	VCore::float32 Pitch = 0.f;
	VCore::float32 Yaw = 0.f;
};

using VRotator2D = VRotator2;
using VRotator3D = VRotator3;
using VRotator = VRotator3D;
}