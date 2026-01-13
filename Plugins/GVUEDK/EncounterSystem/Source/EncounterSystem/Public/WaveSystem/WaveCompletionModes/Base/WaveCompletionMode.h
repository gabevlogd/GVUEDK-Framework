// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WaveCompletionMode.generated.h"

class UWaveManager;
/**
 * This object defines a wave completion mode, which is a specific set of rules or behaviors that dictate how the completion of waves of enemies is determined within the Wave System.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class ENCOUNTERSYSTEM_API UWaveCompletionMode : public UObject
{
	GENERATED_BODY()

	friend class UWaveCompletionModeFactory;

public:

	virtual bool Init(UWaveCompletionMode* Template, UWaveManager* InWaveManager) PURE_VIRTUAL(UWaveCompletionMode::Init, return false;);

	virtual bool IsWaveComplete() PURE_VIRTUAL(UWaveCompletionMode::IsWaveComplete, return false;);

	virtual bool ShouldCallCompletionCheckOnEnemyDeath() const { return bCallCompletionCheckOnEnemyDeath; }

protected:
	
	UPROPERTY()
	UWaveManager* WaveManager = nullptr;
	
	bool bCallCompletionCheckOnEnemyDeath = true;
};
