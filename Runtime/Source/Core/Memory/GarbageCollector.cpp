// Vertex

#include "Core/Memory/GarbageCollector.h"

#include "Core/Memory/ObjectPtr.h"

namespace VCore
{
VGarbageCollector& VGarbageCollector::Get()
{
	static VGarbageCollector Instance;
	return Instance;
}

void VGarbageCollector::Collect()
{
	VObjectManager::Get().CollectPendingKill();
}
}