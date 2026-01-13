// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SequentialMode.h"
#include "LoopingMode.generated.h"

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API ULoopingMode : public USequentialMode
{
	GENERATED_BODY()

public:

	virtual int32 GetNextWaveIndex() override;

	virtual bool HasNextWave() override;
	
};
