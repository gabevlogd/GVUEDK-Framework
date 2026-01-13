// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveExecutionModes/EndlessMode/WaveGeneration/CostBasedGroupGeneration/CostBasedGroupGenerator.h"


FWaveDefinition UCostBasedGroupGenerator::GenerateWave(const UWaveData* WaveData, const int32& WaveIndex)
{
	// 1. Create new wave definition
	FWaveDefinition NewWaveDefinition;
	NewWaveDefinition.WaveExecutionMode = WaveData->Waves[WaveIndex].WaveExecutionMode;
	NewWaveDefinition.WaveCompletionMode = WaveData->Waves[WaveIndex].WaveCompletionMode;
	NewWaveDefinition.WaveEventsHandler = WaveData->Waves[WaveIndex].WaveEventsHandler;
	NewWaveDefinition.EnemyGroupData = NewObject<UEnemyGroupData>(this);
	
	// 2. Determine budget for this wave
	int32 Budget = BaseWaveBudget;
	if (!IsValid(BudgetIncrement))
	{
		UE_LOG(LogWaveGenerator, Warning, TEXT("UCostBasedGroupGenerator::GenerateWave: BudgetIncrement not provided, budget will not increase"));
	}
	else
	{
		Budget = BudgetIncrement->Evaluate(WaveIndex, BaseWaveBudget);
	}

	// 3. Generate enemy groups until budget is exhausted
	while (Budget > 0)
	{
		FCostBasedGroup SelectedGroup = GetGroupMatchingBudget(Budget, WaveIndex);
		if (!SelectedGroup.IsValid())
		{
			UE_LOG(LogWaveGenerator, Warning, TEXT("UCostBasedGroupGenerator::GenerateWave: No valid group found for remaining budget %d, stopping generation"), Budget);
			break;
		}

		NewWaveDefinition.EnemyGroupData->EnemyGroups.Add(SelectedGroup);
		Budget -= IsValid(SelectedGroup.CostIncrement) ? SelectedGroup.CostIncrement->Evaluate(WaveIndex, SelectedGroup.BaseCost) : SelectedGroup.BaseCost;
	}

	return NewWaveDefinition;
}

FCostBasedGroup UCostBasedGroupGenerator::GetGroupMatchingBudget(const int32& RemainingBudget, const int32& WaveIndex)
{
	TArray<const FCostBasedGroup*> ValidGroups;

	for (FCostBasedGroup& Group : AvailableGroups->EnemyGroups)
	{
		const int32 GroupCost = IsValid(Group.CostIncrement) ? Group.CostIncrement->Evaluate(WaveIndex, Group.BaseCost) : Group.BaseCost;
		if (GroupCost <= RemainingBudget)
		{
			ValidGroups.Add(&Group);
		}
	}

	if (ValidGroups.Num() == 0)
		return FCostBasedGroup();

	float TotalWeight = 0.0f;
	for (const FCostBasedGroup* Group : ValidGroups)
	{
		TotalWeight += IsValid(Group->WeightIncrement) ? Group->WeightIncrement->Evaluate(WaveIndex, Group->BaseWeight) : Group->BaseWeight;
	}

	float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);
	for (const FCostBasedGroup* Group : ValidGroups)
	{
		RandomWeight -= IsValid(Group->WeightIncrement) ? Group->WeightIncrement->Evaluate(WaveIndex, Group->BaseWeight) : Group->BaseWeight;
		if (RandomWeight <= 0)
		{
			return *Group;
		}
	}

	return FCostBasedGroup();
}

void UCostBasedGroupGenerator::SimulateBudgetGeneration(const int32& TotalWaves, TArray<float>& OutBudgets) const
{
	OutBudgets.Empty();
	for (int32 WaveIndex = 0; WaveIndex < TotalWaves; ++WaveIndex)
	{
		float Budget = BaseWaveBudget;
		if (IsValid(BudgetIncrement))
		{
			Budget = BudgetIncrement->Evaluate(WaveIndex, Budget);
		}
		OutBudgets.Add(Budget);
	}
}

void UCostBasedGroupGenerator::SimulateCostGeneration(const int32& TotalWaves, TArray<float>& OutCosts) const
{
	OutCosts.Empty();
	for (int32 WaveIndex = 0; WaveIndex < TotalWaves; ++WaveIndex)
	{
		float TotalCost = 0.f;
		for (const FCostBasedGroup& Group : AvailableGroups->EnemyGroups)
		{
			float Cost = IsValid(Group.CostIncrement) ? Group.CostIncrement->Evaluate(WaveIndex, Group.BaseCost) : Group.BaseCost;
			TotalCost += Cost;
		}
		OutCosts.Add(TotalCost);
	}
}
