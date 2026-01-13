// Copyright Villains, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IncrementTypes/Base/BaseIncrement.h"
#include "WaveSystem/WaveExecutionModes/EndlessMode/WaveGeneration/Base/WaveGenerator.h"
#include "Data/CostBasedGroupData.h"
#include "CostBasedGroupGenerator.generated.h"

/**
 * 
 */
UCLASS()
class ENCOUNTERSYSTEM_API UCostBasedGroupGenerator : public UWaveGenerator
{
	GENERATED_BODY()

public:

	virtual FWaveDefinition GenerateWave(const UWaveData* WaveData, const int32& WaveIndex) override;

	virtual FCostBasedGroup GetGroupMatchingBudget(const int32& RemainingBudget, const int32& WaveIndex);

	void SimulateBudgetGeneration(const int32& TotalWaves, TArray<float>& OutBudgets) const;

	void SimulateCostGeneration(const int32& TotalWaves, TArray<float>& OutCosts) const;

	void SimulateWeightGeneration(const int32& TotalWaves, TArray<float>& OutWeights) const;

private:

	/**
	 * The base budget for the first wave generation
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true", ClampMin = "1", UIMin = "1"))
	int32 BaseWaveBudget = 10;

	/**
	 * The budget increment strategy to use for increasing the budget each wave
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Meta = (AllowPrivateAccess = "true"))
	UBaseIncrement* BudgetIncrement;

	/**
	 * Predefined groups available for cost-based generation
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UCostBasedGroupData* AvailableGroups;
};
