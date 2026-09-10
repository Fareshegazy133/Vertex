// Vertex

#include "Input/InputCodes.h"

namespace VInput
{
constexpr VInputCode::VInputCode()
	: Device(EVInputDevice::Keyboard), Code(VKey::Null)
{}

VInputCode::VInputCode(const VKeyCode Key)
	: Device(EVInputDevice::Keyboard), Code(Key)
{}

VInputCode::VInputCode(const VMouseButtonCode MouseButton)
	: Device(EVInputDevice::Mouse), Code(MouseButton)
{}
	
bool VInputCode::operator==(const VInputCode& OtherInputCode) const
{
	return Device == OtherInputCode.Device && Code == OtherInputCode.Code;
}
	
bool VInputCode::operator!=(const VInputCode& OtherInputCode) const
{
	return !(*this == OtherInputCode);
}
}