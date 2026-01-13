// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/WaveExecutionMode.h"
#include "SequentialMode.generated.h"

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API USequentialMode : public UWaveExecutionMode
{
	GENERATED_BODY()

public:

	virtual void ExecuteWave() override;

	virtual UWaveData* GetNextWaveData() override;

	virtual int32 GetNextWaveIndex() override;

	virtual bool HasNextWave() override;

	
};
