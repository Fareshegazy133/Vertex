// Vertex

#pragma once

#include "Core/Types/Numeric.h"

namespace VCore
{
struct VObjectHandle
{
	VObjectHandle() = default;
	VObjectHandle(const int32 Index, const int32 Generation);

	bool IsValid() const;

	bool operator==(const VObjectHandle& OtherObjectHandle) const;
	bool operator!=(const VObjectHandle& OtherObjectHandle) const;

	int32 Index = INDEX_NONE;
	int32 Generation = 0;
};
}