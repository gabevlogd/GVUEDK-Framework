// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KillTargetComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRegisterTarget, AActor*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnregisterTarget, AActor*, Target);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ENCOUNTERSYSTEM_API UKillTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	static FRegisterTarget OnRegisterTarget;
	static FUnregisterTarget OnUnregisterTarget;

public:
	
	UKillTargetComponent();

	UFUNCTION(BlueprintCallable, Category="KillTargetComponent")
	void RegisterTarget(AActor* Target);

	UFUNCTION(BlueprintCallable, Category="KillTargetComponent")
	void UnregisterTarget(AActor* Target);
};
