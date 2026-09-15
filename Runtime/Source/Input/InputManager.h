// Vertex

#pragma once

#include "InputAction.h"
#include "Core/Containers/Map.h"
#include "Core/Memory/ReferencePtr.h"
#include "Core/Types/Numeric.h"
#include "Input/InputCodes.h"

namespace VInput
{
class VInputManager
{
public:
	static void Initialize();
	static void ProcessInput(VCore::float32 DeltaTime);
		
	static void BindAction(VInputAction* InputAction);
	static void UnbindAction(const VCore::VString& ActionName);
	
	static VCore::float32 GetMouseX();
	static VCore::float32 GetMouseY();

	static VCore::float32 GetMouseDeltaX();
	static VCore::float32 GetMouseDeltaY();
	
	static bool IsInputPressed(const VInputCode& Input);
	static bool IsInputInitiallyPressed(const VInputCode& Input);
	static bool IsInputReleased(const VInputCode& Input);
	
	static bool IsKeyPressed(const VKeyCode& Key);
	static bool IsKeyInitiallyPressed(const VKeyCode& Key);
	static bool IsKeyReleased(const VKeyCode& Key);

	static bool IsMouseButtonPressed(const VMouseButtonCode& MouseButton);
	static bool IsMouseButtonInitiallyPressed(const VMouseButtonCode& MouseButton);
	static bool IsMouseButtonReleased(const VMouseButtonCode& MouseButton);

private:
	static void ProcessKeyboardInputs(const VCore::float32 DeltaTime);
	static void ProcessMouseInputs(const VCore::float32 DeltaTime);
		
	static void ProcessBinding(const VInputAction* InputAction, const VInputBinding& InputBinding, VInputBindingState& InputBindingState, const VCore::float32 DeltaTime);
	
	static bool AreModifiersDown(const VInputBinding& InputBinding);
	static bool IsBindingActive(const VInputBinding& InputBinding);

private:
	static VCore::VMap<VCore::VString, VCore::VReferencePtr<VInputAction>> ActionBindings;
};	
}