// Vertex

#pragma once

#include "Core/Containers/Array.h"
#include "Render/RenderCore/RenderObject.h"

namespace VRenderCore
{
class VRenderScene
{
public:
	void AddObject(const VRenderObject& RenderObject);
	void ClearScene();
	
	VCore::VArray<VRenderObject> GetObjects() const;

private:
	VCore::VArray<VRenderObject> RenderObjects;
};
}