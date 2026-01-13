// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WaveData.generated.h"

class UWaveEventsHandler;
class UWaveCompletionMode;
class UWaveExecutionMode;
class UEnemyGroupData;

/**
 * Defines a single wave of enemy groups along with its execution and completion modes
 */
USTRUCT(BlueprintType)
struct FWaveDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEnemyGroupData* EnemyGroupData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWaveExecutionMode* WaveExecutionMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWaveCompletionMode* WaveCompletionMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWaveEventsHandler* WaveEventsHandler;
};


UCLASS(Blueprintable, BlueprintType)
class ENCOUNTERSYSTEM_API UWaveData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FWaveDefinition> Waves;
};
