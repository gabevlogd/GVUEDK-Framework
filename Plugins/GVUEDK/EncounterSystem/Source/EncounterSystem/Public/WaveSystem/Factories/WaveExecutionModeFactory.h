// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/WaveData.h"
#include "UObject/Object.h"
#include "WaveExecutionModeFactory.generated.h"

class UWaveManager;
DEFINE_LOG_CATEGORY_STATIC(LogWaveExecutionModeFactory, All, All);

class UWaveExecutionMode;

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API UWaveExecutionModeFactory : public UObject
{
	GENERATED_BODY()

public:

	virtual UWaveExecutionMode* CreateWaveExecutionMode(UWaveManager* InWaveManager, UWaveData* Data, const int32 WaveIndex);
};
