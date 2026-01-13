// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyGroupData.generated.h"

class USpawnMethod;

USTRUCT(BlueprintType)
struct FEnemyTypeInfo
{
	GENERATED_BODY()

	/**
	 * The enemy class to spawn
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> EnemyClass;

	/**
	 * The number of enemies of this class to spawn
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInt32Range CountRange = FInt32Range(1, 1);
	

	/**
	 * Actor pivot offset for spawn location calculations
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PivotOffset = FVector::ZeroVector;
};

/**
 * Defines a group of enemies to spawn 
 */
USTRUCT(BlueprintType)
struct FEnemyGroup
{
	GENERATED_BODY()

	/**
	 * Array of enemy classes to spawn and their respective counts (how many of each to spawn per type)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemyTypeInfo> EnemyTypes;

	/**
	 * The interval between spawns of individual enemies in this group
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFloatRange SpawnIntervalRange = FFloatRange(0.0f, 0.0f);

	/**
	 * Delay before starting to spawn this enemy group
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFloatRange SpawnDelayRange = FFloatRange(0.0f, 0.0f);

	/**
	 * The method used to determine spawn locations for this enemy group
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	USpawnMethod* SpawnMethod;

	bool IsValid() const
	{
		return EnemyTypes.Num() > 0 && SpawnMethod != nullptr;
	}
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ENCOUNTERSYSTEM_API UEnemyGroupData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemyGroup> EnemyGroups;
};
