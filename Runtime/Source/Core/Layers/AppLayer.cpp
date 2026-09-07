// Vertex

#include "Core/Layers/AppLayer.h"

#include <iostream>
#include "raylib.h"
#include "Input/InputManager.h"

namespace VCore
{
VAppLayer::VAppLayer()
	: TestAction([this]()
	{
		DrawRectangle(200, 200, 50, 50, GREEN);
		std::cout << "TestAction Triggered" << '\n';
	}, "TestAction")
{
	VInput::VInputBinding InputBinding;

	InputBinding.TriggerKey = VInput::VKey::W;
	InputBinding.InputTrigger = VInput::EVInputTrigger::InitialPress;
	InputBinding.TriggerTime = 0.25f;

	TestAction->AddInputBinding(InputBinding);
	VInput::VInputManager::BindAction(TestAction.Get());
}

void VAppLayer::Update(const float32 DeltaTime)
{
	VLayer::Update(DeltaTime);
	VInput::VInputManager::ProcessInput(DeltaTime);
}
}