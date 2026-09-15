// Vertex

#pragma once

#include <concepts>
#include "Render/RenderCore/RenderScene.h"
#include "Scene/Actor.h"

class VWorld : public VObject
{
public:
	VWorld() = default;
	~VWorld() override = default;
	
	void Update(const float32 DeltaTime);
	
	void BuildRenderScene(VRenderCore::VRenderScene& Scene);
	
	template <typename ActorT, typename... Args> requires std::derived_from<VActor, ActorT>
	ActorT* SpawnActor(Args&&... Arguments);
	
	const VArray<VUniquePtr<VActor>>& GetActors() const;

private:
	VArray<VUniquePtr<VActor>> Actors;
};

template <typename ActorT, typename ... Args> requires std::derived_from<VActor, ActorT>
ActorT* VWorld::SpawnActor(Args&&... Arguments)
{
	VUniquePtr<ActorT> Actor = MakeUnique<ActorT>(std::forward<Args>(Arguments)...);
	ActorT* ActorRawPtr = Actor.Get();

	Actors.Add(std::move(Actor));
	return ActorRawPtr;
}