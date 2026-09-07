// Vertex

#pragma once

#include <functional>
#include "InputCodes.h"
#include "Core/Containers/Array.h"
#include "Core/Types/Pair.h"
#include "Core/Types/String.h"
#include "Game/Object.h"

namespace VInput
{
enum class EVInputTrigger : VCore::uint8
{
	/** Triggers when the input is pressed and remains active while held. */
	Press,

	/** Triggers only when the input transitions from an inactive to an active state. */
	InitialPress,

	/** Triggers when the input is pressed and released within the configured click time window. */
	Click,

	/** Triggers when two clicks occur within the configured double-click time window. */
	DoubleClick,

	/** Triggers when an active input is released. */
	Release,

	/** Triggers when the input remains active for the configured hold duration. */
	Hold,

	/** Triggers when the configured combination of inputs is active simultaneously. */
	Chord
};

struct VInputBinding
{
	VCore::VArray<VKeyCode> ModifierKeys;
	VKeyCode TriggerKey = VKey::Null;
	
	EVInputTrigger InputTrigger = EVInputTrigger::Press;
	VCore::float32 TriggerTime = 0.f;
};

struct VInputBindingState
{
	VCore::uint8 ClickCount = 0;
	VCore::float32 HeldTime = 0.f;
	VCore::float32 ClickTime = 0.f;
	bool bHoldTriggered  = false;
};

class VInputAction : public VObject
{
public:
	using VInputCallback = std::function<void()>;
	
	VInputAction() = default;
	explicit VInputAction(const VInputCallback& InInputCallback, const VCore::VString& InActionName);
	~VInputAction() override;
	
	void Execute() const;

	void AddInputBinding(const VInputBinding& InputBinding);
	void RemoveInputBinding(const VInputBinding& InputBinding);

	VCore::VArray<VCore::VPair<VInputBinding, VInputBindingState>>& GetInputBindings();
	VCore::VString GetActionName() const;
	
private:
	VCore::VArray<VCore::VPair<VInputBinding, VInputBindingState>> InputBindings;
	VInputCallback InputCallback;
	
	VCore::VString ActionName;
};
}
