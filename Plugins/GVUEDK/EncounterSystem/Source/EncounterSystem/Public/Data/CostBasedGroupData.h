// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnemyGroupData.h"
#include "WaveSystem/WaveExecutionModes/EndlessMode/WaveGeneration/CostBasedGroupGeneration/IncrementTypes/Base/BaseIncrement.h"
#include "CostBasedGroupData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FCostBasedGroup : public FEnemyGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = "0", UIMin = "0"))
	int32 BaseCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = "0", UIMin = "0"))
	float BaseWeight = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	UBaseIncrement* CostIncrement = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	UBaseIncrement* WeightIncrement = nullptr;
};


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ENCOUNTERSYSTEM_API UCostBasedGroupData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCostBasedGroup> EnemyGroups;

};
