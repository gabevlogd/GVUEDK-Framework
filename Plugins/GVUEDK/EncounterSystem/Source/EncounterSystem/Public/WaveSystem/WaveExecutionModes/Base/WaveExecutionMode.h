// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/WaveData.h"
#include "UObject/Object.h"
#include "WaveExecutionMode.generated.h"

class UWaveManager;
/**
 * This object defines a wave mode, which is a specific set of rules or behaviors that dictate how waves of enemies are spawned and managed within the Wave System.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class ENCOUNTERSYSTEM_API UWaveExecutionMode : public UObject
{
	GENERATED_BODY()

	friend class UWaveExecutionModeFactory;
	
public:
	
	virtual bool Init(UWaveExecutionMode* Template, UWaveManager* InWaveManager, UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex);

	virtual void ExecuteWave() PURE_VIRTUAL(UWaveExecutionMode::ExecuteWave);

	virtual UWaveData* GetNextWaveData() PURE_VIRTUAL(UWaveExecutionMode::GetNextWaveData, return NULL;);

	virtual int32 GetNextWaveIndex() PURE_VIRTUAL(UWaveExecutionMode::GetNextWaveIndex, return -1;);
	
	virtual bool HasNextWave() PURE_VIRTUAL(UWaveExecutionMode::HasNextWave, return false;);
	
	
	virtual UWaveData* GetCurrentWaveData() const { return OwningWaveData; }

	virtual int32 GetCurrentWaveIndex() const { return CurrentWaveIndex; }
	
	virtual float GetDelayBeforeNextWave() const;

	virtual bool ShouldAutoStartNextWave() const { return bAutoStartNext; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	bool bAutoStartNext = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true", EditCondition = "bAutoStartNext"))
	FFloatRange DelayBeforeNextWaveRange = FFloatRange(0.f, 0.f);

	UPROPERTY()
	UWaveManager* WaveManager = nullptr;

	UPROPERTY()
	UWaveData* OwningWaveData = nullptr;

	int32 CurrentWaveIndex = 0;
};
