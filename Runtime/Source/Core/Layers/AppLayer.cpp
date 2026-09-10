// Vertex

#include "Core/Layers/AppLayer.h"

#include <iostream>
#include "raylib.h"
#include "Input/InputManager.h"

namespace VCore
{
VAppLayer::VAppLayer()
{
	VInput::VInputBinding InputBinding;
	TestAction = new VInput::VInputAction(&VAppLayer::DrawSquare, this, "TestAction");

	InputBinding.TriggerInput = VInput::VKey::W;
	InputBinding.InputTrigger = VInput::EVInputTrigger::Press;

	TestAction->AddInputBinding(InputBinding);
	VInput::VInputManager::BindAction(TestAction);
}

void VAppLayer::Update(const float32 DeltaTime)
{
	VLayer::Update(DeltaTime);
	VInput::VInputManager::ProcessInput(DeltaTime);
}

void VAppLayer::DrawSquare() const
{
	DrawRectangle(200, 200, 50, 50, GREEN);
	std::cout << "TestAction Triggered" << '\n';
}
}