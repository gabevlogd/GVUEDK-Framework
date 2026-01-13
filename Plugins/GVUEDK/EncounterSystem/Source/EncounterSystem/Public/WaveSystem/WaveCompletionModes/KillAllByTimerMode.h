// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TimerMode.h"
#include "KillAllByTimerMode.generated.h"

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API UKillAllByTimerMode : public UTimerMode
{
	GENERATED_BODY()

public:

	virtual bool Init(UWaveCompletionMode* Template, UWaveManager* InWaveManager) override;

	virtual bool IsWaveComplete() override;
};
