// Vertex

#include "Math/Rotator.h"

#include <cmath>

namespace VMath
{
const VRotator2 VRotator2::ZeroRotator = VRotator2();

VRotator2::VRotator2(const VCore::float32 Angle)
	: Angle(VRotator3::NormalizeAngle(Angle))
{}

bool VRotator2::IsZero() const
{
	return Angle == 0.f;
}

VRotator2 VRotator2::operator+(const VRotator2& OtherRotator) const
{
	return VRotator2(Angle + OtherRotator.Angle);
}

VRotator2 VRotator2::operator-(const VRotator2& OtherRotator) const
{
	return VRotator2(Angle - OtherRotator.Angle);
}

VRotator2 VRotator2::operator*(const VRotator2& OtherRotator) const
{
	return VRotator2(Angle * OtherRotator.Angle);
}

VRotator2 VRotator2::operator/(const VRotator2& OtherRotator) const
{
	return VRotator2(Angle / OtherRotator.Angle);
}

VRotator2 VRotator2::operator+=(const VRotator2& OtherRotator)
{
	Angle += OtherRotator.Angle;
	return *this;
}

VRotator2 VRotator2::operator-=(const VRotator2& OtherRotator)
{
	Angle -= OtherRotator.Angle;
	return *this;
}

VRotator2 VRotator2::operator*=(const VRotator2& OtherRotator)
{
	Angle *= OtherRotator.Angle;
	return *this;
}

VRotator2 VRotator2::operator/=(const VRotator2& OtherRotator)
{
	Angle /= OtherRotator.Angle;
	return *this;
}
	
bool VRotator2::operator==(const VRotator2& OtherRotator) const
{
	return Angle == OtherRotator.Angle;
}

bool VRotator2::operator!=(const VRotator2& OtherRotator) const
{
	return !(*this == OtherRotator);
}

VRotator2::operator float() const
{
	return Angle;
}

const VRotator3 VRotator3::ZeroRotator = VRotator3();

VRotator3::VRotator3(const VCore::float32 Roll, const VCore::float32 Pitch, const VCore::float32 Yaw)
	: Roll(NormalizeAngle(Roll)), Pitch(NormalizeAngle(Pitch)), Yaw(NormalizeAngle(Yaw))
{}
	
VRotator3::VRotator3(const VCore::float32 RollPitchYaw)
	: Roll (NormalizeAngle(RollPitchYaw)), Pitch (NormalizeAngle(RollPitchYaw)), Yaw (NormalizeAngle(RollPitchYaw))
{}

VCore::float32 VRotator3::NormalizeAngle(VCore::float32 Angle)
{
	Angle = std::fmod(Angle, 360.f);

	if (Angle < 0.f) Angle += 360.f;
	return Angle;
}
	
bool VRotator3::IsZero() const
{
	return Roll == 0.f && Pitch == 0.f && Yaw == 0.f;
}
	
VRotator3 VRotator3::operator+(const VRotator3& OtherRotator) const
{
	return VRotator3(Roll + OtherRotator.Roll, Pitch + OtherRotator.Pitch, Yaw + OtherRotator.Yaw);
}

VRotator3 VRotator3::operator-(const VRotator3& OtherRotator) const
{
	return VRotator3(Roll - OtherRotator.Roll, Pitch - OtherRotator.Pitch, Yaw - OtherRotator.Yaw);
}

VRotator3 VRotator3::operator*(const VRotator3& OtherRotator) const
{
	return VRotator3(Roll * OtherRotator.Roll, Pitch * OtherRotator.Pitch, Yaw * OtherRotator.Yaw);
}

VRotator3 VRotator3::operator/(const VRotator3& OtherRotator) const
{
	return VRotator3(Roll / OtherRotator.Roll, Pitch / OtherRotator.Pitch, Yaw / OtherRotator.Yaw);
}

VRotator3 VRotator3::operator+=(const VRotator3& OtherRotator)
{
	Roll += OtherRotator.Roll;
	Pitch += OtherRotator.Pitch;
	Yaw += OtherRotator.Yaw;
	return *this;
}

VRotator3 VRotator3::operator-=(const VRotator3& OtherRotator)
{
	Roll -= OtherRotator.Roll;
	Pitch -= OtherRotator.Pitch;
	Yaw -= OtherRotator.Yaw;
	return *this;
}

VRotator3 VRotator3::operator*=(const VRotator3& OtherRotator)
{
	Roll *= OtherRotator.Roll;
	Pitch *= OtherRotator.Pitch;
	Yaw *= OtherRotator.Yaw;
	return *this;
}

VRotator3 VRotator3::operator/=(const VRotator3& OtherRotator)
{
	Roll /= OtherRotator.Roll;
	Pitch /= OtherRotator.Pitch;
	Yaw /= OtherRotator.Yaw;
	return *this;
}

bool VRotator3::operator==(const VRotator3& OtherRotator) const
{
	return Roll == OtherRotator.Roll && Pitch == OtherRotator.Pitch && Yaw == OtherRotator.Yaw;
}

bool VRotator3::operator!=(const VRotator3& OtherRotator) const
{
	return !(*this == OtherRotator);
}
}