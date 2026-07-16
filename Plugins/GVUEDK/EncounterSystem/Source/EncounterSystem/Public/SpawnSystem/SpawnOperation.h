// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SpawnOperation.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ENCOUNTERSYSTEM_API USpawnOperation : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	AActor* SpawnActor(UWorld* World, TSubclassOf<AActor> ActorClass, const FVector& Location, const FRotator& Rotation) const;

	AActor* SpawnActor_Implementation(UWorld* World, TSubclassOf<AActor> ActorClass, const FVector& Location, const FRotator& Rotation) const;

private:
#if WITH_EDITOR
	virtual bool ImplementsGetWorld() const override { return true; }
# endif

	virtual UWorld* GetWorld() const override { return GetOuter()->GetWorld(); }
};
