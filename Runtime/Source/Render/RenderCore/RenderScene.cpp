// Vertex

#include "Render/RenderCore/RenderScene.h"

namespace VRenderCore
{

void VRenderScene::AddObject(const VRenderObject& RenderObject)
{
	RenderObjects.Add(RenderObject);
}

void VRenderScene::ClearScene()
{
	RenderObjects.Empty();
}

VCore::VArray<VRenderObject> VRenderScene::GetObjects() const
{
	return RenderObjects;
}
}