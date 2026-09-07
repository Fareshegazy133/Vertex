// Vertex

#pragma once

#include "Core/Application/Window.h"
#include "Core/Containers/Array.h"
#include "Core/Layers/Layer.h"
#include "Core/Memory/UniquePtr.h"

namespace VCore
{
struct VApplicationSpecification
{
	VString ApplicationName = "Vertex";
	VWindowSpecification WindowSpecification;
};

class VApplication
{
public:
	VApplication(const VApplicationSpecification& InApplicationSpecification = VApplicationSpecification());
	~VApplication();
	
	void RunApplication();
	
	template<typename T> requires std::derived_from<T,VLayer>
	void PushLayer();

private:
	void Update();
	void Render();
	
private:
	VArray<VUniquePtr<VLayer>> LayerStack;
	
	VUniquePtr<VWindow> MainWindow;
	
	VApplicationSpecification ApplicationSpecification;
	
	bool bIsRunning = false;
};

template <typename TLayer> requires std::derived_from<TLayer, VLayer>
void VApplication::PushLayer()
{
	VUniquePtr<VLayer> Layer = MakeUnique<TLayer>();
	LayerStack.Add(std::move(Layer));
	LayerStack[LayerStack.Num() - 1]->OnAttach();
}
}