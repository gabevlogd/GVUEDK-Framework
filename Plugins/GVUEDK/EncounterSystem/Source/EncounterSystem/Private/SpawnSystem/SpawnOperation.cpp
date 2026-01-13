// Copyright Villains, Inc. All Rights Reserved.


#include "SpawnSystem/SpawnOperation.h"

AActor* USpawnOperation::SpawnActor_Implementation(UWorld* World, TSubclassOf<AActor> ActorClass, const FVector& Location,
	const FRotator& Rotation) const
{
	if (!IsValid(World)) return nullptr;
	if (!IsValid(ActorClass)) return nullptr;
	return World->SpawnActor<AActor>(ActorClass, Location, Rotation);
}
