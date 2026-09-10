// Vertex

#include "Input/InputAction.h"

namespace VInput
{
VInputAction::VInputAction(const VInputCallback& InInputCallback, const VCore::VString& InActionName)
	: InputCallback(InInputCallback), ActionName(InActionName)
{}

VInputAction::~VInputAction()
{
	InputBindings.Empty();
	InputCallback = nullptr;
	ActionName.Clear();
}

void VInputAction::Execute() const
{
	if (!InputCallback) return;
	InputCallback();
}

void VInputAction::AddInputBinding(const VInputBinding& InputBinding)
{
	if (InputBinding.TriggerInput == VKey::Null) return;

	VCore::VPair<VInputBinding, VInputBindingState> InputBindingPair;
	InputBindingPair.First = InputBinding;
	InputBindingPair.Second = VInputBindingState();
	
	InputBindings.Add(InputBindingPair);
}

void VInputAction::RemoveInputBinding(const VInputBinding& InputBinding)
{
	for (VCore::int32 Index = 0; Index < InputBindings.Num(); Index++)
	{
		const VInputBinding& CurrentInputBinding = InputBindings[Index].First;

		if (CurrentInputBinding.TriggerInput != InputBinding.TriggerInput) continue;
		if (CurrentInputBinding.InputTrigger != InputBinding.InputTrigger) continue;
		if (CurrentInputBinding.TriggerTime != InputBinding.TriggerTime) continue;
		if (CurrentInputBinding.ModifierInputs.Num() != InputBinding.ModifierInputs.Num()) continue;

		bool bModifiersMatch = true;

		for (VCore::int32 ModifierIndex = 0; ModifierIndex < InputBinding.ModifierInputs.Num(); ModifierIndex++)
		{
			if (CurrentInputBinding.ModifierInputs[ModifierIndex] != InputBinding.ModifierInputs[ModifierIndex])
			{
				bModifiersMatch = false;
				break;
			}
		}

		if (!bModifiersMatch) continue;

		InputBindings.RemoveAt(Index);
		return;
	}
}

VInputAction::VInputBindings& VInputAction::GetInputBindings()
{
	return InputBindings;
}

VCore::VString VInputAction::GetActionName() const
{
	return ActionName;
}
}