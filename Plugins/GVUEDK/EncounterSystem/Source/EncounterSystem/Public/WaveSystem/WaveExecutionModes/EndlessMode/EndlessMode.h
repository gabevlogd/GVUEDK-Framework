// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WaveGeneration/Base/WaveGenerator.h"
#include "WaveSystem/WaveExecutionModes/Base/WaveExecutionMode.h"
#include "EndlessMode.generated.h"

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API UEndlessMode : public UWaveExecutionMode
{
	GENERATED_BODY()

public:

	virtual bool Init(UWaveExecutionMode* Template, UWaveManager* InWaveManager, UWaveData* InOwningWaveData, const int32 InCurrentWaveIndex) override;

	virtual void ExecuteWave() override;

	virtual UWaveData* GetCurrentWaveData() const override;

	virtual UWaveData* GetNextWaveData() override;

	virtual int32 GetNextWaveIndex() override;

	virtual bool HasNextWave() override;

	void SimulateBudgetGeneration(const int32& TotalWaves, TArray<float>& OutBudgets) const;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Meta = (AllowPrivateAccess = "true"))
	UWaveGenerator* WaveGenerator;

	UPROPERTY()
	UWaveData* GeneratedWaveData;
};
