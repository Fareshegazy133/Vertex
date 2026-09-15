// Vertex

#pragma once

#include "RenderScene.h"
#include "Core/Memory/ReferencePtr.h"
#include "Render/RenderCore/RenderCommand.h"
#include "Render/RenderCore/RenderView.h"
#include "Scene/World.h"

namespace VRender
{
	class VRHI;
}

namespace VRenderCore
{
class VRenderer
{
public:
	VRenderer(VRender::VRHI* InRHI);
	~VRenderer();
	
	void BeginFrame();
	void EndFrame() const;
	
	void Render(const VRenderScene& Scene, const VRenderView& View);
	void RenderWorld(const VWorld& World) const;

	VRender::VRHI& GetRHI() const;
	
private:
	void BuildCommands(const VRenderScene& Scene, const VRenderView& View);
	void SortCommands();
	void ExecuteCommands();
	
private:
	VCore::VReferencePtr<VRender::VRHI> RHI;

	VRenderCommandQueue CommandQueue;
	VRenderView CurrentView;
};
}