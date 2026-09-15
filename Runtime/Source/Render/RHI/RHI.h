// Vertex

#pragma once

namespace VRenderCore
{
	struct VRenderObject;
}

namespace VRender
{
class VRHI
{
public:
	virtual ~VRHI() = default;
	
	virtual void DrawObject(const VRenderCore::VRenderObject& RenderObject) = 0;
	virtual void Clear() = 0;
	
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
};
}