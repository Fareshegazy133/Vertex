// Vertex

#include "Scene/World.h"

void VWorld::Update(const float32 DeltaTime)
{
	for (const auto& Actor : Actors)
	{
		Actor->Update(DeltaTime);
	}
}

void VWorld::BuildRenderScene(VRenderCore::VRenderScene& Scene)
{
	Scene.ClearScene();

	for (const auto& Actor : Actors)
	{
		VTransform2D ActorTransform;
		ActorTransform.Position = Actor->GetActorPosition();
		ActorTransform.Rotation = Actor->GetActorRotation();
		ActorTransform.Scale = Actor->GetActorScale();
		
		VRenderCore::VRenderObject Object;
		Object.Transform = ActorTransform;
		Scene.AddObject(Object);
	}
}

const VArray<VUniquePtr<VActor>>& VWorld::GetActors() const
{
	return Actors;
}