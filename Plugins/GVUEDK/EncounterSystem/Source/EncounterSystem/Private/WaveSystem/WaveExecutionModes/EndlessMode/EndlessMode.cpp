// Copyright Villains, Inc. All Rights Reserved.


#include "WaveSystem/WaveExecutionModes/EndlessMode/EndlessMode.h"

#include "Data/EnemyGroupData.h"
#include "WaveSystem/WaveManager.h"
#include "WaveSystem/WaveExecutionModes/EndlessMode/WaveGeneration/CostBasedGroupGeneration/CostBasedGroupGenerator.h"

bool UEndlessMode::Init(UWaveExecutionMode* Template, UWaveManager* InWaveManager, UWaveData* InOwningWaveData,
                        const int32 InCurrentWaveIndex)
{
	if (!Super::Init(Template, InWaveManager, InOwningWaveData, InCurrentWaveIndex))
	{
		return false;
	}

	UEndlessMode* EndlessTemplate = Cast<UEndlessMode>(Template);
	if (!IsValid(EndlessTemplate))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("UEndlessMode::Init: Template is not of type UEndlessMode"));
		return false;
	}

	if (!IsValid(EndlessTemplate->WaveGenerator))
	{
		UE_LOG(LogWaveManagerSubsystem, Error, TEXT("UEndlessMode::Init: WaveGenerator is not valid"));
		return false;
	}

	WaveGenerator = EndlessTemplate->WaveGenerator;
	WaveGenerator->Init();

	return true;
}

void UEndlessMode::ExecuteWave()
{
	// Create a deep copy of OwningWaveData to modify for the generated wave
	GeneratedWaveData = NewObject<UWaveData>(this);
	GeneratedWaveData->Waves = OwningWaveData->Waves;

	// Remove any waves beyond the current wave index
	for (int32 i = CurrentWaveIndex + 1; i < GeneratedWaveData->Waves.Num(); i++)
	{
		GeneratedWaveData->Waves.RemoveAt(i);
	}

	const FWaveDefinition NewWaveDefinition = WaveGenerator->GenerateWave(GeneratedWaveData, CurrentWaveIndex);
	GeneratedWaveData->Waves[CurrentWaveIndex] = NewWaveDefinition;

	// Prepare for the next wave (without adding the new array element for the next wave, the StartNextWave call will
	// produce an out-of-bounds error)
	GeneratedWaveData->Waves.Add(NewWaveDefinition);
	 
	
	WaveManager->WaveStarted();
}

UWaveData* UEndlessMode::GetCurrentWaveData() const
{
	return GeneratedWaveData;
}

UWaveData* UEndlessMode::GetNextWaveData()
{
	return GeneratedWaveData;
}

int32 UEndlessMode::GetNextWaveIndex()
{
	return CurrentWaveIndex + 1;
}

bool UEndlessMode::HasNextWave()
{
	return true;
}

void UEndlessMode::SimulateBudgetGeneration(const int32& TotalWaves, TArray<float>& OutBudgets) const 
{
	if (!IsValid(WaveGenerator))
	{
		UE_LOG(LogWaveManagerSubsystem, Warning, TEXT("UEndlessMode::SimulateBudgetGeneration: WaveGenerator is not valid"));
		return;
	}
	
	if (UCostBasedGroupGenerator* Generator = Cast<UCostBasedGroupGenerator>(WaveGenerator))
	{
		Generator->SimulateBudgetGeneration(TotalWaves, OutBudgets);
	}
}
