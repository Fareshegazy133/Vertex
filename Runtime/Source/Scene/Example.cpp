// Vertex

#include "Scene/Example.h"

#include "Core/Reflection/Class.h"

VExample::VExample()
{
	SetName("Example");
}

VCore::VClass* VExample::GetClass() const
{
	return StaticClass();
}

VCore::VClass* VExample::StaticClass()
{
	static VCore::VClass Class(VObject::StaticClass(), "Example");
	return &Class;
}