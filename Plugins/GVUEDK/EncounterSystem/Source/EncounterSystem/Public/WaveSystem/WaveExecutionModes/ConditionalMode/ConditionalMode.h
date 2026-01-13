// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NextWaveCondition.h"
#include "WaveSystem/WaveExecutionModes/Base/WaveExecutionMode.h"
#include "ConditionalMode.generated.h"

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API UConditionalMode : public UWaveExecutionMode
{
	GENERATED_BODY()

public:

	virtual bool Init(UWaveExecutionMode* Template, UWaveManager* InWaveManager, UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex) override;

	virtual void ExecuteWave() override;

	virtual UWaveData* GetNextWaveData() override;

	virtual int32 GetNextWaveIndex() override;

	virtual bool HasNextWave() override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"), Instanced)
	UNextWaveCondition* NextWaveCondition;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0"))
	int32 NextWaveIndexTrue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0"))
	int32 NextWaveIndexFalse;

};
