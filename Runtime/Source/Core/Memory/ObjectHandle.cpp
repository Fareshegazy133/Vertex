// Vertex

#include "Core/Memory/ObjectHandle.h"

namespace VCore
{
VObjectHandle::VObjectHandle(const int32 Index, const int32 Generation)
	: Index(Index), Generation(Generation)
{}

bool VObjectHandle::IsValid() const
{
	return Index >= 0 && Generation > 0;
}

bool VObjectHandle::operator==(const VObjectHandle& OtherObjectHandle) const
{
	return Index == OtherObjectHandle.Index && Generation == OtherObjectHandle.Generation;
}

bool VObjectHandle::operator!=(const VObjectHandle& OtherObjectHandle) const
{
	return !(*this == OtherObjectHandle);
}
}