// Vertex

#include "Scene/Actor.h"

#include "Input/InputManager.h"

void VActor::Update(const float32 DeltaTime)
{
	if (VInput::VInputManager::IsKeyPressed(VInput::VKey::W))
	{
		SetActorPosition(GetActorPosition() + VVector2D(1.f));
	}
}

void VActor::SetActorPosition(const VVector2& ActorPosition)
{
	ActorTransform.Position = ActorPosition;
}

void VActor::SetActorRotation(const VRotator2D& ActorRotation)
{
	ActorTransform.Rotation = ActorRotation;
}

void VActor::SetActorScale(const VVector2D& ActorScale)
{
	ActorTransform.Scale = ActorScale;
}

VVector2D VActor::GetActorPosition() const
{
	return ActorTransform.Position;
}

VRotator2D VActor::GetActorRotation() const
{
	return ActorTransform.Rotation;
}

VVector2D VActor::GetActorScale() const
{
	return ActorTransform.Scale;
}