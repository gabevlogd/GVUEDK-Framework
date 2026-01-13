// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WaveSystem/WaveManager.h"
#include "NextWaveCondition.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew)
class ENCOUNTERSYSTEM_API UNextWaveCondition : public UObject
{
	GENERATED_BODY()

public:

	void Init(const UObject* WorldContextObject);

	UFUNCTION(BlueprintImplementableEvent)
	void Init();

	UFUNCTION(BlueprintImplementableEvent)
	bool Evaluate(UWaveManager* WaveManager, UWaveData* CurrentWaveData, int32 CurrentWaveIndex);

	UFUNCTION(BlueprintImplementableEvent)
	bool HasNextWave(UWaveManager* WaveManager, UWaveData* CurrentWaveData, int32 CurrentWaveIndex);

private:

	UPROPERTY()
	UWorld* World = nullptr;

#if WITH_EDITOR
	virtual bool ImplementsGetWorld() const override { return true; }
# endif

	virtual UWorld* GetWorld() const override { return World; }
};
