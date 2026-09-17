// Vertex

#pragma once

#include "Core/Types/String.h"

namespace VCore
{
class VArchive
{
public:
	virtual ~VArchive() = default;

	virtual void Serialize(void* Data, TSize Size) = 0;
	virtual void SerializeString(VString& String) = 0;
	
	virtual bool IsLoading() const = 0;
	virtual bool IsSaving() const = 0;

	template<typename T>
	VArchive& operator<<(T& Value);
};

template <typename T>
VArchive& VArchive::operator<<(T& Value)
{
	Serialize(&Value, sizeof(T));
	return *this;
}
}