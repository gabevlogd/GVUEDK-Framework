// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LoopingMode.h"
#include "RandomMode.generated.h"

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API URandomMode : public ULoopingMode
{
	GENERATED_BODY()

public:

	virtual bool Init(UWaveExecutionMode* Template, UWaveManager* InWaveManager, UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex) override;

	virtual int32 GetNextWaveIndex() override;

private:

	int32 GetRandomIndex() const;

	int32 MaxRandomIndex;

};
