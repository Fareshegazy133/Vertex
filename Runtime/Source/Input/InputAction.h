// Vertex

#pragma once

#include <functional>
#include "InputCodes.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/Pair.h"
#include "Core/Types/String.h"

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
};

enum class EVInputValue : VCore::uint8
{
	Bool,
	Axis1D,
	Axis2D
};

struct VInputBinding
{
	VCore::VArray<VInputCode> ModifierInputs;
	VInputCode TriggerInput = NullInput;
	
	EVInputTrigger InputTrigger = EVInputTrigger::Press;
	EVInputValue InputValue = EVInputValue::Bool;
	
	VCore::float32 TriggerTime = 0.f;
	
	bool bRepeatWhileHeld = false;
};
 
struct VInputBindingState
{
	VCore::uint8 ClickCount = 0;
	VCore::float32 HeldTime = 0.f;
	VCore::float32 ClickTime = 0.f;
	bool bHoldTriggered  = false;
};

class VInputAction
{
public:
	using VInputBindings = VCore::VArray<VCore::VPair<VInputBinding, VInputBindingState>>;
	using VInputCallback = std::function<void()>;
	
	explicit VInputAction() = default;
	explicit VInputAction(const VInputCallback& InInputCallback, const VCore::VString& InActionName);
	
	template<typename ObjectT>
	VInputAction(void(ObjectT::*Function)() const, ObjectT* Object, const VCore::VString& InActionName);

	template<typename TObject>
	VInputAction(void(TObject::*Function)(), TObject* Object, const VCore::VString& InActionName);
	
	~VInputAction();
	
	void Execute() const;

	void AddInputBinding(const VInputBinding& InputBinding);
	void RemoveInputBinding(const VInputBinding& InputBinding);

	VInputBindings& GetInputBindings();
	VCore::VString GetActionName() const;
	
private:
	VInputBindings InputBindings;
	VInputCallback InputCallback;
	
	VCore::VString ActionName = VCore::VString::EmptyString;
};

template <typename TObject>
VInputAction::VInputAction(void(TObject::* Function)() const, TObject* Object, const VCore::VString& InActionName)
	: InputCallback([Object, Function]() { (Object->*Function)(); }), ActionName(InActionName)
{}

template <typename TObject>
VInputAction::VInputAction(void(TObject::* Function)(), TObject* Object, const VCore::VString& InActionName)
	: InputCallback([Object, Function]() { (Object->*Function)(); }), ActionName(InActionName)
{}
}