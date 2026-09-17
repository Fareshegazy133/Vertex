// Vertex

#pragma once

#include "Core/Reflection/Struct.h"

namespace VCore
{
class VArchive;

class VScriptStruct : public VStruct
{
public:
	using VStruct::VStruct;
	virtual ~VScriptStruct() override = default;

	virtual void InitializeStruct(void* Data) const;
	virtual void DestroyStruct(void* Data) const;

	virtual void Serialize(VArchive& Archive, void* Data) const;
};
}