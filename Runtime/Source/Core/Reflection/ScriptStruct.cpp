// Vertex

#include "Core/Reflection/ScriptStruct.h"

namespace VCore
{
void VScriptStruct::InitializeStruct(void* Data) const
{
	Initialize(Data);
}

void VScriptStruct::DestroyStruct(void* Data) const
{
	Destroy(Data);
}

void VScriptStruct::Serialize(VArchive& Archive, void* Data) const
{
	(void)Archive;
	(void)Data;
}
}