// Vertex

#pragma once

namespace VCore
{
class VGarbageCollector
{
public:
	static VGarbageCollector& Get();
	void Collect();

private:
	VGarbageCollector() = default;
};
}