// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/WaveData.h"
#include "UObject/Object.h"
#include "WaveGenerator.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogWaveGenerator, All, All);

class UWaveData;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class ENCOUNTERSYSTEM_API UWaveGenerator : public UObject
{
	GENERATED_BODY()

public:

	virtual void Init(){}

	virtual FWaveDefinition GenerateWave(const UWaveData* WaveData, const int32& WaveIndex) PURE_VIRTUAL(UWaveGenerator::GenerateWave, return FWaveDefinition(););
};
