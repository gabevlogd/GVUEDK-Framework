// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WaveSystem/WaveManager.h"
#include "WaveCompletionModeFactory.generated.h"

class UWaveData;

DEFINE_LOG_CATEGORY_STATIC(LogWaveCompletionModeFactory, All, All);

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API UWaveCompletionModeFactory : public UObject
{
	GENERATED_BODY()

public:

	virtual UWaveCompletionMode* CreateWaveCompletionMode(UWaveManager* InWaveManager, UWaveData* WaveData, const int32 WaveIndex);

};
