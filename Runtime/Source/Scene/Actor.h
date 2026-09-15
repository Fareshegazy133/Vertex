// Vertex

#pragma once

#include "Core/CoreMinimal.h"

class VActor : public VObject
{
public:
	void Update(const float32 DeltaTime);
	
	void SetActorPosition(const VVector2D& ActorPosition);
	void SetActorRotation(const VRotator2D& ActorRotation);
	void SetActorScale(const VVector2D& ActorScale);
	
	VVector2D GetActorPosition() const;
	VRotator2D GetActorRotation() const;
	VVector2D GetActorScale() const;
	
private:
	VTransform2 ActorTransform;
};