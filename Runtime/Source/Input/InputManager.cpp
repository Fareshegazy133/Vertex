// Vertex

#include "Input/InputManager.h"

#include "InputAction.h"
#include "Core/CoreMinimal.h"

namespace VInput
{
VCore::VMap<VString, VCore::VReferencePtr<VInputAction>> VInputManager::ActionBindings;
	
void VInputManager::Initialize()
{
	
}

void VInputManager::ProcessInput(const VCore::float32 DeltaTime)
{
	ProcessKeyboardInputs(DeltaTime);
	ProcessMouseInputs(DeltaTime);
}

void VInputManager::BindAction(VInputAction* InputAction)
{
	if (!InputAction) return;
	const VCore::VString ActionName = InputAction->GetActionName();
	
	if (ActionName.IsEmpty()) return;
	if (ActionBindings.Contains(ActionName)) return;

	ActionBindings.Add(ActionName, InputAction);
}

void VInputManager::UnbindAction(const VCore::VString& ActionName)
{
	if (ActionName.IsEmpty()) return;
	if (!ActionBindings.Contains(ActionName)) return;
	ActionBindings.Remove(ActionName);
}

VCore::float32 VInputManager::GetMouseX()
{
	return ::GetMousePosition().x;
}

VCore::float32 VInputManager::GetMouseY()
{
	return ::GetMousePosition().y;
}

VCore::float32 VInputManager::GetMouseDeltaX()
{
	return ::GetMouseDelta().x;
}

VCore::float32 VInputManager::GetMouseDeltaY()
{
	return ::GetMouseDelta().y;
}

bool VInputManager::IsInputPressed(const VInputCode& Input)
{
	switch (Input.Device)
	{
		case EVInputDevice::Keyboard:
			return IsKeyPressed(static_cast<VKeyCode>(Input.Code));

		case EVInputDevice::Mouse:
			return IsMouseButtonPressed(static_cast<VMouseButtonCode>(Input.Code));
	}

	return false;
}

bool VInputManager::IsInputInitiallyPressed(const VInputCode& Input)
{
	switch (Input.Device)
	{
		case EVInputDevice::Keyboard:
			return IsKeyInitiallyPressed(static_cast<VKeyCode>(Input.Code));

		case EVInputDevice::Mouse:
			return IsMouseButtonInitiallyPressed(static_cast<VMouseButtonCode>(Input.Code));
	}

	return false;
}

bool VInputManager::IsInputReleased(const VInputCode& Input)
{
	switch (Input.Device)
	{
		case EVInputDevice::Keyboard:
			return IsKeyReleased(static_cast<VKeyCode>(Input.Code));

		case EVInputDevice::Mouse:
			return IsMouseButtonReleased(static_cast<VMouseButtonCode>(Input.Code));
	}

	return false;
}

bool VInputManager::IsKeyPressed(const VKeyCode& Key)
{
	return ::IsKeyDown(Key);
}

bool VInputManager::IsKeyInitiallyPressed(const VKeyCode& Key)
{
	return ::IsKeyPressed(Key);
}

bool VInputManager::IsKeyReleased(const VKeyCode& Key)
{
	return ::IsKeyReleased(Key);
}

bool VInputManager::IsMouseButtonPressed(const VMouseButtonCode& MouseButton)
{
	return ::IsMouseButtonDown(MouseButton);
}

bool VInputManager::IsMouseButtonInitiallyPressed(const VMouseButtonCode& MouseButton)
{
	return ::IsMouseButtonPressed(MouseButton);
}

bool VInputManager::IsMouseButtonReleased(const VMouseButtonCode& MouseButton)
{
	return ::IsMouseButtonReleased(MouseButton);
}

void VInputManager::ProcessKeyboardInputs(const VCore::float32 DeltaTime)
{
	ActionBindings.ForEach([DeltaTime](const VCore::VString&, const VCore::VReferencePtr<VInputAction>& InputActionPtr)
		{
			VInputAction* InputAction = InputActionPtr.Get();
			if (!InputAction) return;

			for (auto& InputBinding : InputAction->GetInputBindings())
			{
				ProcessBinding(InputAction, InputBinding.FirstValue, InputBinding.SecondValue, DeltaTime);
			}
		}
	);
}

void VInputManager::ProcessMouseInputs(const VCore::float32 DeltaTime)
{

}

void VInputManager::ProcessBinding(const VInputAction* InputAction, const VInputBinding& InputBinding, VInputBindingState& InputBindingState, const VCore::float32 DeltaTime)
{
	if (!InputAction) return;
	if (InputBinding.TriggerInput == NullInput) return;
	if (!IsBindingActive(InputBinding)) return;
	
	const bool bIsPressed = IsInputPressed(InputBinding.TriggerInput);
	const bool bIsInitiallyPressed = IsInputInitiallyPressed(InputBinding.TriggerInput);
	const bool bIsReleased = IsInputReleased(InputBinding.TriggerInput);
	
	switch (InputBinding.InputTrigger)
	{
	case EVInputTrigger::Press:
	{
		if (!bIsPressed) break;
		InputAction->Execute();
		break;
	}
	case EVInputTrigger::InitialPress:
	{
		if (!bIsInitiallyPressed) break;
		InputAction->Execute();
		break;
	}
	case EVInputTrigger::Release:
	{
		if (!bIsReleased) break;
		InputAction->Execute();
		break;
	}
	case EVInputTrigger::Click:
	{
		if (bIsInitiallyPressed)
		{
			InputBindingState.ClickTime = 0.f;
		}
		else if (bIsPressed)
		{
			InputBindingState.ClickTime += DeltaTime;
		}
		else if (bIsReleased)
		{
			if (InputBindingState.ClickTime <= InputBinding.TriggerTime)
			{
				InputAction->Execute();
			}

			InputBindingState.ClickTime = 0.f;
		}

		break;
	}
	case EVInputTrigger::DoubleClick:
	{
		static bool bDoubleClickStart = false;
		
		if (bIsInitiallyPressed && InputBindingState.ClickCount == 0)
		{
			InputBindingState.ClickTime = 0.f;
			bDoubleClickStart = true;
		}
		else if (bDoubleClickStart && (InputBindingState.ClickCount == 0 || InputBindingState.ClickCount == 1))
		{
			InputBindingState.ClickTime += DeltaTime;
			
			if (InputBindingState.ClickTime > InputBinding.TriggerTime)
			{
				InputBindingState.ClickCount = 0;
				InputBindingState.ClickTime = 0.f;
				bDoubleClickStart = false;
			}
		}
		
		if (bIsReleased && bDoubleClickStart)
		{
			if (InputBindingState.ClickTime <= InputBinding.TriggerTime)
			{
				InputBindingState.ClickCount++;
				
				if (InputBindingState.ClickCount == 2)
				{
					InputAction->Execute();

					InputBindingState.ClickCount = 0;
					InputBindingState.ClickTime = 0.f;
					bDoubleClickStart = false;
				}
			}
			else
			{
				InputBindingState.ClickCount = 0;
				InputBindingState.ClickTime = 0.f;
				bDoubleClickStart = false;
			}
		}

		break;
	}
	case EVInputTrigger::Hold:
	{
		if (bIsPressed)
		{
			InputBindingState.HeldTime += DeltaTime;

			if (InputBindingState.HeldTime >= InputBinding.TriggerTime)
			{
				if (InputBinding.bRepeatWhileHeld)
				{
					InputAction->Execute();
				}
				else if (!InputBindingState.bHoldTriggered)
				{
					InputBindingState.bHoldTriggered = true;
					InputAction->Execute();
				}
			}
		}
		else
		{
			InputBindingState.HeldTime = 0.f;
			InputBindingState.bHoldTriggered = false;
		}

		break;
	}
	}
}

bool VInputManager::AreModifiersDown(const VInputBinding& InputBinding)
{
	for (const auto& ModifierInput : InputBinding.ModifierInputs)
	{
		if (!IsInputPressed(ModifierInput)) return false;
	}

	return true;
}

bool VInputManager::IsBindingActive(const VInputBinding& InputBinding)
{
	if (InputBinding.ModifierInputs.IsEmpty()) return true;
	return AreModifiersDown(InputBinding);
}
}