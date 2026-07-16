// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "State/StateBase.h"
#include "StateMachineConfig.generated.h"

/**
 * 
 */
UCLASS()
class STATEMACHINESYSTEM_API UStateMachineConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Machine Config")
	FGameplayTag StateMachineTag;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Machine Config")
	TSubclassOf<UStateBase> EntryStateClass;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Machine Config")
	TArray<TSubclassOf<UStateBase>> StateClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Machine Config")
	bool bReplicateData;
};
